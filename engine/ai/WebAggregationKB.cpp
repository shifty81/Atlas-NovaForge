#include "WebAggregationKB.h"
#include <algorithm>
#include <sstream>
#include <unordered_set>
#include <cctype>

namespace atlas::ai {

// --- JSON helpers (matches BuildManifest pattern) ---

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
    if (json[pos] == '"') return ParseQuotedString(json, pos);
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

static std::string ToLower(const std::string& s) {
    std::string result = s;
    for (auto& c : result) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return result;
}

// --- Entry management ---

uint64_t WebAggregationKB::AddEntry(const KBEntry& entry) {
    uint64_t id = m_nextID++;
    KBEntry stored = entry;
    stored.id = id;
    m_entries[id] = stored;
    return id;
}

void WebAggregationKB::RemoveEntry(uint64_t id) {
    m_entries.erase(id);
}

const KBEntry* WebAggregationKB::GetEntry(uint64_t id) const {
    auto it = m_entries.find(id);
    if (it == m_entries.end()) return nullptr;
    return &it->second;
}

size_t WebAggregationKB::EntryCount() const {
    return m_entries.size();
}

// --- Search ---

KBSearchResult WebAggregationKB::Search(const std::string& query, size_t maxResults) const {
    KBSearchResult result;
    result.query = query;
    std::string lowerQuery = ToLower(query);

    for (const auto& kv : m_entries) {
        const auto& entry = kv.second;
        std::string lowerTitle = ToLower(entry.title);
        std::string lowerContent = ToLower(entry.content);
        if (lowerTitle.find(lowerQuery) != std::string::npos ||
            lowerContent.find(lowerQuery) != std::string::npos) {
            result.entries.push_back(entry);
        }
    }

    result.totalMatches = result.entries.size();

    std::sort(result.entries.begin(), result.entries.end(),
        [](const KBEntry& a, const KBEntry& b) {
            return a.relevanceScore > b.relevanceScore;
        });

    if (result.entries.size() > maxResults) {
        result.entries.resize(maxResults);
    }
    return result;
}

KBSearchResult WebAggregationKB::SearchByCategory(const std::string& category) const {
    KBSearchResult result;
    result.query = category;
    for (const auto& kv : m_entries) {
        if (kv.second.category == category) {
            result.entries.push_back(kv.second);
        }
    }
    result.totalMatches = result.entries.size();
    return result;
}

KBSearchResult WebAggregationKB::SearchByTag(const std::string& tag) const {
    KBSearchResult result;
    result.query = tag;
    for (const auto& kv : m_entries) {
        for (const auto& t : kv.second.tags) {
            if (t == tag) {
                result.entries.push_back(kv.second);
                break;
            }
        }
    }
    result.totalMatches = result.entries.size();
    return result;
}

// --- Categories and Tags ---

std::vector<std::string> WebAggregationKB::ListCategories() const {
    std::unordered_set<std::string> cats;
    for (const auto& kv : m_entries) {
        if (!kv.second.category.empty()) {
            cats.insert(kv.second.category);
        }
    }
    return std::vector<std::string>(cats.begin(), cats.end());
}

size_t WebAggregationKB::CategoryCount() const {
    return ListCategories().size();
}

std::vector<std::string> WebAggregationKB::ListTags() const {
    std::unordered_set<std::string> tagSet;
    for (const auto& kv : m_entries) {
        for (const auto& t : kv.second.tags) {
            tagSet.insert(t);
        }
    }
    return std::vector<std::string>(tagSet.begin(), tagSet.end());
}

// --- Export/Import JSON ---

std::string WebAggregationKB::ExportJSON() const {
    std::ostringstream ss;
    ss << "{\"entries\":[";
    bool first = true;
    for (const auto& kv : m_entries) {
        if (!first) ss << ",";
        first = false;
        const auto& e = kv.second;
        ss << "{";
        ss << "\"id\":" << e.id;
        ss << ",\"source\":\"" << EscapeString(e.source) << "\"";
        ss << ",\"title\":\"" << EscapeString(e.title) << "\"";
        ss << ",\"content\":\"" << EscapeString(e.content) << "\"";
        ss << ",\"category\":\"" << EscapeString(e.category) << "\"";
        ss << ",\"timestamp\":" << e.timestamp;
        ss << ",\"relevanceScore\":" << e.relevanceScore;
        ss << ",\"tags\":[";
        for (size_t i = 0; i < e.tags.size(); ++i) {
            if (i > 0) ss << ",";
            ss << "\"" << EscapeString(e.tags[i]) << "\"";
        }
        ss << "]";
        ss << "}";
    }
    ss << "]}";
    return ss.str();
}

bool WebAggregationKB::ImportJSON(const std::string& json) {
    size_t pos = 0;
    SkipWhitespace(json, pos);
    if (pos >= json.size() || json[pos] != '{') return false;
    ++pos;

    while (pos < json.size()) {
        SkipWhitespace(json, pos);
        if (pos >= json.size() || json[pos] == '}') break;
        if (json[pos] == ',') { ++pos; continue; }

        std::string key = ParseQuotedString(json, pos);
        SkipWhitespace(json, pos);
        if (pos < json.size() && json[pos] == ':') ++pos;
        SkipWhitespace(json, pos);

        if (key == "entries" && pos < json.size() && json[pos] == '[') {
            ++pos;
            while (pos < json.size()) {
                SkipWhitespace(json, pos);
                if (pos >= json.size() || json[pos] == ']') { ++pos; break; }
                if (json[pos] == ',') { ++pos; continue; }
                if (json[pos] == '{') {
                    ++pos;
                    KBEntry entry;
                    while (pos < json.size()) {
                        SkipWhitespace(json, pos);
                        if (pos >= json.size() || json[pos] == '}') { ++pos; break; }
                        if (json[pos] == ',') { ++pos; continue; }

                        std::string k = ParseQuotedString(json, pos);
                        SkipWhitespace(json, pos);
                        if (pos < json.size() && json[pos] == ':') ++pos;
                        SkipWhitespace(json, pos);

                        if (k == "tags" && pos < json.size() && json[pos] == '[') {
                            ++pos;
                            while (pos < json.size()) {
                                SkipWhitespace(json, pos);
                                if (pos >= json.size() || json[pos] == ']') { ++pos; break; }
                                if (json[pos] == ',') { ++pos; continue; }
                                entry.tags.push_back(ParseValue(json, pos));
                            }
                        } else if (pos < json.size() && (json[pos] == '[' || json[pos] == '{')) {
                            SkipNestedStructure(json, pos);
                        } else {
                            std::string val = ParseValue(json, pos);
                            try {
                                if (k == "id") entry.id = std::stoull(val);
                                else if (k == "source") entry.source = val;
                                else if (k == "title") entry.title = val;
                                else if (k == "content") entry.content = val;
                                else if (k == "category") entry.category = val;
                                else if (k == "timestamp") entry.timestamp = std::stoull(val);
                                else if (k == "relevanceScore") entry.relevanceScore = std::stod(val);
                            } catch (...) {}
                        }
                    }
                    m_entries[entry.id] = entry;
                    if (entry.id >= m_nextID) m_nextID = entry.id + 1;
                }
            }
        } else if (pos < json.size() && (json[pos] == '[' || json[pos] == '{')) {
            SkipNestedStructure(json, pos);
        } else {
            ParseValue(json, pos);
        }
    }
    return true;
}

void WebAggregationKB::Clear() {
    m_entries.clear();
    m_nextID = 1;
}

} // namespace atlas::ai
