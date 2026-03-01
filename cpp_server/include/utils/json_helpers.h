#pragma once

// Lightweight JSON extraction helpers — no external library required.
//
// These free functions operate on raw JSON strings with simple key
// lookups.  They are intentionally minimal; for complex parsing
// consider a full JSON library.

#include <string>

namespace atlas {
namespace json {

/// Escape a string for safe embedding in JSON values.
inline std::string escapeString(const std::string& input) {
    std::string result;
    result.reserve(input.size());
    for (char c : input) {
        switch (c) {
            case '\"': result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\n': result += "\\n";  break;
            case '\r': result += "\\r";  break;
            case '\t': result += "\\t";  break;
            default:
                if (static_cast<unsigned char>(c) < 0x20) {
                    // Skip other control characters
                } else {
                    result += c;
                }
                break;
        }
    }
    return result;
}

/// Extract a string value for a given key: "key":"value"
inline std::string extractString(const std::string& json,
                                 const std::string& key) {
    std::string search = "\"" + key + "\"";
    size_t pos = json.find(search);
    if (pos == std::string::npos) return "";

    pos = json.find(':', pos + search.size());
    if (pos == std::string::npos) return "";

    pos = json.find('\"', pos + 1);
    if (pos == std::string::npos) return "";

    // Handle escaped quotes inside the value
    size_t end = pos + 1;
    while (end < json.size()) {
        if (json[end] == '\\') { end += 2; continue; }
        if (json[end] == '\"') break;
        ++end;
    }
    if (end >= json.size()) return "";

    return json.substr(pos + 1, end - pos - 1);
}

/// Extract a float value for a given key: "key":123.4
inline float extractFloat(const std::string& json,
                          const std::string& key,
                          float fallback = 0.0f) {
    std::string search = "\"" + key + "\"";
    size_t pos = json.find(search);
    if (pos == std::string::npos) return fallback;

    pos = json.find(':', pos + search.size());
    if (pos == std::string::npos) return fallback;
    ++pos;

    while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t' ||
                                  json[pos] == '\n' || json[pos] == '\r'))
        ++pos;

    try {
        size_t end = pos;
        while (end < json.size() &&
               (json[end] == '-' || json[end] == '.' ||
                (json[end] >= '0' && json[end] <= '9') ||
                json[end] == 'e' || json[end] == 'E' || json[end] == '+')) {
            ++end;
        }
        return std::stof(json.substr(pos, end - pos));
    } catch (...) {
        return fallback;
    }
}

/// Extract an integer value for a given key: "key":42
inline int extractInt(const std::string& json,
                      const std::string& key,
                      int fallback = 0) {
    std::string search = "\"" + key + "\"";
    size_t pos = json.find(search);
    if (pos == std::string::npos) return fallback;

    pos = json.find(':', pos + search.size());
    if (pos == std::string::npos) return fallback;
    ++pos;

    while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t' ||
                                  json[pos] == '\n' || json[pos] == '\r'))
        ++pos;

    try {
        size_t end = pos;
        while (end < json.size() &&
               (json[end] == '-' || (json[end] >= '0' && json[end] <= '9'))) {
            ++end;
        }
        return std::stoi(json.substr(pos, end - pos));
    } catch (...) {
        return fallback;
    }
}

/// Extract a double value for a given key: "key":123.456789
inline double extractDouble(const std::string& json,
                            const std::string& key,
                            double fallback = 0.0) {
    std::string search = "\"" + key + "\"";
    size_t pos = json.find(search);
    if (pos == std::string::npos) return fallback;

    pos = json.find(':', pos + search.size());
    if (pos == std::string::npos) return fallback;
    ++pos;

    while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t' ||
                                  json[pos] == '\n' || json[pos] == '\r'))
        ++pos;

    try {
        size_t end = pos;
        while (end < json.size() &&
               (json[end] == '-' || json[end] == '.' ||
                (json[end] >= '0' && json[end] <= '9') ||
                json[end] == 'e' || json[end] == 'E' || json[end] == '+')) {
            ++end;
        }
        return std::stod(json.substr(pos, end - pos));
    } catch (...) {
        return fallback;
    }
}

/// Extract a boolean value for a given key: "key":true
inline bool extractBool(const std::string& json,
                        const std::string& key,
                        bool fallback = false) {
    std::string search = "\"" + key + "\"";
    size_t pos = json.find(search);
    if (pos == std::string::npos) return fallback;

    pos = json.find(':', pos + search.size());
    if (pos == std::string::npos) return fallback;
    ++pos;

    while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t'))
        ++pos;

    if (pos + 4 <= json.size() && json.substr(pos, 4) == "true")  return true;
    if (pos + 5 <= json.size() && json.substr(pos, 5) == "false") return false;
    return fallback;
}

/// Extract a nested JSON object for a given key: "key":{...}
inline std::string extractObject(const std::string& json,
                                 const std::string& key) {
    std::string search = "\"" + key + "\"";
    size_t pos = json.find(search);
    if (pos == std::string::npos) return "";

    pos = json.find('{', pos + search.size());
    if (pos == std::string::npos) return "";

    int depth = 0;
    for (size_t i = pos; i < json.size(); ++i) {
        if (json[i] == '{') ++depth;
        else if (json[i] == '}') {
            --depth;
            if (depth == 0) {
                return json.substr(pos, i - pos + 1);
            }
        }
    }

    return "";
}

} // namespace json
} // namespace atlas
