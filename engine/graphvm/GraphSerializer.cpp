#include "GraphSerializer.h"
#include <stdexcept>
#include <cmath>
#include <iomanip>

namespace atlas::graphvm {

// --- JsonBuilder ---

std::string JsonBuilder::EscapeString(const std::string& s) {
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

void JsonBuilder::PrependComma() {
    if (m_needComma) {
        m_ss << ",";
    }
    m_needComma = true;
}

void JsonBuilder::WriteKey(const std::string& key) {
    PrependComma();
    m_ss << "\"" << EscapeString(key) << "\":";
}

void JsonBuilder::BeginObject() {
    m_commaStack.push_back(m_needComma);
    m_needComma = false;
    m_ss << "{";
}

void JsonBuilder::EndObject() {
    m_ss << "}";
    if (!m_commaStack.empty()) {
        m_needComma = m_commaStack.back();
        m_commaStack.pop_back();
    }
    m_needComma = true;
}

void JsonBuilder::BeginArray(const std::string& key) {
    WriteKey(key);
    m_commaStack.push_back(m_needComma);
    m_needComma = false;
    m_ss << "[";
}

void JsonBuilder::EndArray() {
    m_ss << "]";
    if (!m_commaStack.empty()) {
        m_needComma = m_commaStack.back();
        m_commaStack.pop_back();
    }
    m_needComma = true;
}

void JsonBuilder::AddString(const std::string& key, const std::string& value) {
    WriteKey(key);
    m_ss << "\"" << EscapeString(value) << "\"";
}

void JsonBuilder::AddInt(const std::string& key, int64_t value) {
    WriteKey(key);
    m_ss << value;
}

void JsonBuilder::AddFloat(const std::string& key, float value) {
    WriteKey(key);
    if (std::isnan(value) || std::isinf(value)) {
        m_ss << "0";
    } else {
        m_ss << std::setprecision(6) << value;
    }
}

void JsonBuilder::AddBool(const std::string& key, bool value) {
    WriteKey(key);
    m_ss << (value ? "true" : "false");
}

void JsonBuilder::BeginArrayObject() {
    PrependComma();
    m_commaStack.push_back(m_needComma);
    m_needComma = false;
    m_ss << "{";
}

std::string JsonBuilder::Build() const {
    return m_ss.str();
}

// --- JsonReader ---

std::string JsonReader::UnescapeString(const std::string& s) {
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

void JsonReader::SkipWhitespace(const std::string& json, size_t& pos) {
    while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t' ||
           json[pos] == '\n' || json[pos] == '\r')) {
        ++pos;
    }
}

std::string JsonReader::ParseQuotedString(const std::string& json, size_t& pos) {
    if (pos >= json.size() || json[pos] != '"') return "";
    ++pos; // skip opening quote
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
    if (pos < json.size()) ++pos; // skip closing quote
    return UnescapeString(result);
}

std::string JsonReader::ParseValue(const std::string& json, size_t& pos) {
    SkipWhitespace(json, pos);
    if (pos >= json.size()) return "";

    if (json[pos] == '"') {
        return ParseQuotedString(json, pos);
    }

    // Parse non-string value (number, bool, null)
    std::string result;
    while (pos < json.size() && json[pos] != ',' && json[pos] != '}' &&
           json[pos] != ']' && json[pos] != ' ' && json[pos] != '\n' &&
           json[pos] != '\r' && json[pos] != '\t') {
        result += json[pos];
        ++pos;
    }
    return result;
}

JsonReader::JsonObject JsonReader::ParseObject(const std::string& json, size_t& pos) {
    JsonObject obj;
    SkipWhitespace(json, pos);
    if (pos >= json.size() || json[pos] != '{') return obj;
    ++pos; // skip '{'

    while (pos < json.size()) {
        SkipWhitespace(json, pos);
        if (pos >= json.size() || json[pos] == '}') { ++pos; break; }
        if (json[pos] == ',') { ++pos; continue; }

        std::string key = ParseQuotedString(json, pos);
        SkipWhitespace(json, pos);
        if (pos < json.size() && json[pos] == ':') ++pos;
        SkipWhitespace(json, pos);

        // Check if value is an array or object (skip them for flat objects)
        if (pos < json.size() && (json[pos] == '[' || json[pos] == '{')) {
            // Skip nested structure
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
        } else {
            std::string value = ParseValue(json, pos);
            obj.values[key] = value;
        }
    }
    return obj;
}

std::vector<JsonReader::JsonObject> JsonReader::ParseArray(const std::string& json, size_t& pos) {
    std::vector<JsonObject> arr;
    SkipWhitespace(json, pos);
    if (pos >= json.size() || json[pos] != '[') return arr;
    ++pos; // skip '['

    while (pos < json.size()) {
        SkipWhitespace(json, pos);
        if (pos >= json.size() || json[pos] == ']') { ++pos; break; }
        if (json[pos] == ',') { ++pos; continue; }

        if (json[pos] == '{') {
            arr.push_back(ParseObject(json, pos));
        } else {
            // Skip non-object array elements
            ParseValue(json, pos);
        }
    }
    return arr;
}

bool JsonReader::Parse(const std::string& json) {
    m_values.clear();
    m_arrays.clear();

    size_t pos = 0;
    SkipWhitespace(json, pos);
    if (pos >= json.size() || json[pos] != '{') return false;
    ++pos; // skip '{'

    while (pos < json.size()) {
        SkipWhitespace(json, pos);
        if (pos >= json.size() || json[pos] == '}') break;
        if (json[pos] == ',') { ++pos; continue; }

        std::string key = ParseQuotedString(json, pos);
        SkipWhitespace(json, pos);
        if (pos < json.size() && json[pos] == ':') ++pos;
        SkipWhitespace(json, pos);

        if (pos < json.size() && json[pos] == '[') {
            m_arrays[key] = ParseArray(json, pos);
        } else if (pos < json.size() && json[pos] == '{') {
            // Skip nested objects at top level
            ParseObject(json, pos);
        } else {
            m_values[key] = ParseValue(json, pos);
        }
    }
    return true;
}

std::string JsonReader::GetString(const std::string& key) const {
    auto it = m_values.find(key);
    if (it == m_values.end()) return "";
    return it->second;
}

int64_t JsonReader::GetInt(const std::string& key) const {
    auto it = m_values.find(key);
    if (it == m_values.end()) return 0;
    try { return std::stoll(it->second); }
    catch (...) { return 0; }
}

float JsonReader::GetFloat(const std::string& key) const {
    auto it = m_values.find(key);
    if (it == m_values.end()) return 0.0f;
    try { return std::stof(it->second); }
    catch (...) { return 0.0f; }
}

bool JsonReader::GetBool(const std::string& key) const {
    auto it = m_values.find(key);
    if (it == m_values.end()) return false;
    return it->second == "true";
}

bool JsonReader::HasKey(const std::string& key) const {
    return m_values.count(key) > 0 || m_arrays.count(key) > 0;
}

std::vector<JsonReader::JsonObject> JsonReader::GetArray(const std::string& key) const {
    auto it = m_arrays.find(key);
    if (it == m_arrays.end()) return {};
    return it->second;
}

} // namespace atlas::graphvm
