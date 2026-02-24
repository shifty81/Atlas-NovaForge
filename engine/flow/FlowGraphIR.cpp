#include "FlowGraphIR.h"
#include <sstream>
#include <cmath>
#include <iomanip>
#include <set>

namespace atlas::flow {

// --- JSON helpers (matches GraphSerializer pattern) ---

static std::string EscapeString(const std::string& s) {
    std::string out;
    out.reserve(s.size() + 2);
    for (char c : s) {
        switch (c) {
            case '"':  out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\n': out += "\\n";  break;
            case '\r': out += "\\r";  break;
            case '\t': out += "\\t";  break;
            default:   out += c;      break;
        }
    }
    return out;
}

static std::string UnescapeString(const std::string& s) {
    std::string out;
    out.reserve(s.size());
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '\\' && i + 1 < s.size()) {
            char next = s[i + 1];
            switch (next) {
                case '"':  out += '"';  ++i; break;
                case '\\': out += '\\'; ++i; break;
                case 'n':  out += '\n'; ++i; break;
                case 'r':  out += '\r'; ++i; break;
                case 't':  out += '\t'; ++i; break;
                default:   out += s[i]; break;
            }
        } else {
            out += s[i];
        }
    }
    return out;
}

static void SkipWhitespace(const std::string& json, size_t& pos) {
    while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t' ||
           json[pos] == '\n' || json[pos] == '\r')) {
        ++pos;
    }
}

static std::string ParseQuotedString(const std::string& json, size_t& pos) {
    if (pos >= json.size() || json[pos] != '"') return "";
    ++pos;
    std::string result;
    while (pos < json.size() && json[pos] != '"') {
        if (json[pos] == '\\' && pos + 1 < json.size()) {
            result += json[pos];
            result += json[pos + 1];
            pos += 2;
        } else {
            result += json[pos];
            ++pos;
        }
    }
    if (pos < json.size()) ++pos;
    return UnescapeString(result);
}

static std::string ParseValue(const std::string& json, size_t& pos) {
    SkipWhitespace(json, pos);
    if (pos >= json.size()) return "";

    if (json[pos] == '"') {
        return ParseQuotedString(json, pos);
    }

    std::string result;
    while (pos < json.size() && json[pos] != ',' && json[pos] != '}' &&
           json[pos] != ']' && json[pos] != ' ' && json[pos] != '\n' &&
           json[pos] != '\r' && json[pos] != '\t') {
        result += json[pos];
        ++pos;
    }
    return result;
}

static void SkipNestedStructure(const std::string& json, size_t& pos) {
    char open = json[pos];
    char close = (open == '[') ? ']' : '}';
    int depth = 1;
    ++pos;
    while (pos < json.size() && depth > 0) {
        if (json[pos] == open) ++depth;
        else if (json[pos] == close) --depth;
        else if (json[pos] == '"') {
            ++pos;
            while (pos < json.size() && json[pos] != '"') {
                if (json[pos] == '\\') ++pos;
                ++pos;
            }
        }
        ++pos;
    }
}

static std::string FormatFloat(float value) {
    if (std::isnan(value) || std::isinf(value)) return "0";
    std::ostringstream ss;
    ss << std::setprecision(6) << value;
    return ss.str();
}

// --- FlowGraphIR::ToJSON ---

std::string FlowGraphIR::ToJSON() const {
    std::ostringstream ss;
    ss << "{";
    ss << "\"version\":" << version;
    ss << ",\"name\":\"" << EscapeString(name) << "\"";
    ss << ",\"graphType\":\"" << EscapeString(graphType) << "\"";

    // Nodes
    ss << ",\"nodes\":[";
    for (size_t i = 0; i < nodes.size(); ++i) {
        if (i > 0) ss << ",";
        const auto& n = nodes[i];
        ss << "{";
        ss << "\"id\":" << n.id;
        ss << ",\"type\":\"" << EscapeString(n.type) << "\"";
        ss << ",\"category\":\"" << EscapeString(n.category) << "\"";
        ss << ",\"posX\":" << FormatFloat(n.posX);
        ss << ",\"posY\":" << FormatFloat(n.posY);
        ss << ",\"properties\":[";
        for (size_t j = 0; j < n.properties.size(); ++j) {
            if (j > 0) ss << ",";
            ss << "[\"" << EscapeString(n.properties[j].first)
               << "\",\"" << EscapeString(n.properties[j].second) << "\"]";
        }
        ss << "]";
        ss << "}";
    }
    ss << "]";

    // Edges
    ss << ",\"edges\":[";
    for (size_t i = 0; i < edges.size(); ++i) {
        if (i > 0) ss << ",";
        const auto& e = edges[i];
        ss << "{";
        ss << "\"fromNode\":" << e.fromNode;
        ss << ",\"fromPort\":" << e.fromPort;
        ss << ",\"toNode\":" << e.toNode;
        ss << ",\"toPort\":" << e.toPort;
        ss << "}";
    }
    ss << "]";

    ss << "}";
    return ss.str();
}

// --- FlowGraphIR::FromJSON ---

// Parse a single node object starting at '{'
static FlowNodeIR ParseNodeObject(const std::string& json, size_t& pos) {
    FlowNodeIR node;
    SkipWhitespace(json, pos);
    if (pos >= json.size() || json[pos] != '{') return node;
    ++pos;

    while (pos < json.size()) {
        SkipWhitespace(json, pos);
        if (pos >= json.size() || json[pos] == '}') { ++pos; break; }
        if (json[pos] == ',') { ++pos; continue; }

        std::string key = ParseQuotedString(json, pos);
        SkipWhitespace(json, pos);
        if (pos < json.size() && json[pos] == ':') ++pos;
        SkipWhitespace(json, pos);

        if (key == "properties" && pos < json.size() && json[pos] == '[') {
            // Parse array of [key, value] pairs
            ++pos;
            while (pos < json.size()) {
                SkipWhitespace(json, pos);
                if (pos >= json.size() || json[pos] == ']') { ++pos; break; }
                if (json[pos] == ',') { ++pos; continue; }
                if (json[pos] == '[') {
                    ++pos;
                    SkipWhitespace(json, pos);
                    std::string pkey = ParseQuotedString(json, pos);
                    SkipWhitespace(json, pos);
                    if (pos < json.size() && json[pos] == ',') ++pos;
                    SkipWhitespace(json, pos);
                    std::string pval = ParseQuotedString(json, pos);
                    SkipWhitespace(json, pos);
                    if (pos < json.size() && json[pos] == ']') ++pos;
                    node.properties.emplace_back(pkey, pval);
                }
            }
        } else if (pos < json.size() && (json[pos] == '[' || json[pos] == '{')) {
            SkipNestedStructure(json, pos);
        } else {
            std::string value = ParseValue(json, pos);
            if (key == "id") {
                try { node.id = static_cast<uint32_t>(std::stoul(value)); }
                catch (...) {}
            } else if (key == "type") {
                node.type = value;
            } else if (key == "category") {
                node.category = value;
            } else if (key == "posX") {
                try { node.posX = std::stof(value); }
                catch (...) {}
            } else if (key == "posY") {
                try { node.posY = std::stof(value); }
                catch (...) {}
            }
        }
    }
    return node;
}

// Parse a single edge object starting at '{'
static FlowEdge ParseEdgeObject(const std::string& json, size_t& pos) {
    FlowEdge edge{};
    SkipWhitespace(json, pos);
    if (pos >= json.size() || json[pos] != '{') return edge;
    ++pos;

    while (pos < json.size()) {
        SkipWhitespace(json, pos);
        if (pos >= json.size() || json[pos] == '}') { ++pos; break; }
        if (json[pos] == ',') { ++pos; continue; }

        std::string key = ParseQuotedString(json, pos);
        SkipWhitespace(json, pos);
        if (pos < json.size() && json[pos] == ':') ++pos;
        SkipWhitespace(json, pos);

        if (pos < json.size() && (json[pos] == '[' || json[pos] == '{')) {
            SkipNestedStructure(json, pos);
        } else {
            std::string value = ParseValue(json, pos);
            try {
                if (key == "fromNode") edge.fromNode = static_cast<uint32_t>(std::stoul(value));
                else if (key == "fromPort") edge.fromPort = static_cast<uint16_t>(std::stoul(value));
                else if (key == "toNode") edge.toNode = static_cast<uint32_t>(std::stoul(value));
                else if (key == "toPort") edge.toPort = static_cast<uint16_t>(std::stoul(value));
            } catch (...) {}
        }
    }
    return edge;
}

FlowGraphIR FlowGraphIR::FromJSON(const std::string& json) {
    FlowGraphIR ir;
    size_t pos = 0;
    SkipWhitespace(json, pos);
    if (pos >= json.size() || json[pos] != '{') return ir;
    ++pos;

    while (pos < json.size()) {
        SkipWhitespace(json, pos);
        if (pos >= json.size() || json[pos] == '}') break;
        if (json[pos] == ',') { ++pos; continue; }

        std::string key = ParseQuotedString(json, pos);
        SkipWhitespace(json, pos);
        if (pos < json.size() && json[pos] == ':') ++pos;
        SkipWhitespace(json, pos);

        if (key == "nodes" && pos < json.size() && json[pos] == '[') {
            ++pos;
            while (pos < json.size()) {
                SkipWhitespace(json, pos);
                if (pos >= json.size() || json[pos] == ']') { ++pos; break; }
                if (json[pos] == ',') { ++pos; continue; }
                if (json[pos] == '{') {
                    ir.nodes.push_back(ParseNodeObject(json, pos));
                }
            }
        } else if (key == "edges" && pos < json.size() && json[pos] == '[') {
            ++pos;
            while (pos < json.size()) {
                SkipWhitespace(json, pos);
                if (pos >= json.size() || json[pos] == ']') { ++pos; break; }
                if (json[pos] == ',') { ++pos; continue; }
                if (json[pos] == '{') {
                    ir.edges.push_back(ParseEdgeObject(json, pos));
                }
            }
        } else if (pos < json.size() && (json[pos] == '[' || json[pos] == '{')) {
            SkipNestedStructure(json, pos);
        } else {
            std::string value = ParseValue(json, pos);
            if (key == "version") {
                try { ir.version = static_cast<uint32_t>(std::stoul(value)); }
                catch (...) {}
            } else if (key == "name") {
                ir.name = value;
            } else if (key == "graphType") {
                ir.graphType = value;
            }
        }
    }
    return ir;
}

// --- FlowGraphIR::Validate ---

bool FlowGraphIR::Validate() const {
    if (version == 0) return false;
    if (name.empty()) return false;

    // Check for duplicate node IDs
    std::set<uint32_t> ids;
    for (const auto& n : nodes) {
        if (n.id == 0) return false;
        if (!ids.insert(n.id).second) return false;
        if (n.type.empty()) return false;
    }

    // Validate edge references
    for (const auto& e : edges) {
        if (ids.find(e.fromNode) == ids.end()) return false;
        if (ids.find(e.toNode) == ids.end()) return false;
    }

    return true;
}

// --- FlowGraphIR::Migrate ---

FlowGraphIR FlowGraphIR::Migrate(const FlowGraphIR& old, uint32_t targetVersion) {
    FlowGraphIR result = old;
    // Currently only version 1 exists; future migrations go here
    result.version = targetVersion;
    return result;
}

} // namespace atlas::flow
