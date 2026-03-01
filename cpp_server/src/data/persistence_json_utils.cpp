#include "data/world_persistence.h"
#include <string>

namespace atlas {
namespace data {

std::string WorldPersistence::extractString(const std::string& json,
                                            const std::string& key) {
    std::string search = "\"" + key + "\"";
    size_t pos = json.find(search);
    if (pos == std::string::npos) return "";

    pos = json.find(':', pos + search.size());
    if (pos == std::string::npos) return "";

    pos = json.find('\"', pos + 1);
    if (pos == std::string::npos) return "";

    size_t end = json.find('\"', pos + 1);
    if (end == std::string::npos) return "";

    return json.substr(pos + 1, end - pos - 1);
}

float WorldPersistence::extractFloat(const std::string& json,
                                     const std::string& key,
                                     float fallback) {
    size_t pos = json.find(key);
    if (pos == std::string::npos) return fallback;

    pos += key.size();
    while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t')) ++pos;

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

int WorldPersistence::extractInt(const std::string& json,
                                 const std::string& key,
                                 int fallback) {
    size_t pos = json.find(key);
    if (pos == std::string::npos) return fallback;

    pos += key.size();
    while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t')) ++pos;

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

double WorldPersistence::extractDouble(const std::string& json,
                                       const std::string& key,
                                       double fallback) {
    size_t pos = json.find(key);
    if (pos == std::string::npos) return fallback;

    pos += key.size();
    while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t')) ++pos;

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

bool WorldPersistence::extractBool(const std::string& json,
                                   const std::string& key,
                                   bool fallback) {
    size_t pos = json.find(key);
    if (pos == std::string::npos) return fallback;

    pos += key.size();
    while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t')) ++pos;

    if (pos + 4 <= json.size() && json.substr(pos, 4) == "true")  return true;
    if (pos + 5 <= json.size() && json.substr(pos, 5) == "false") return false;
    return fallback;
}

std::string WorldPersistence::extractObject(const std::string& json,
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

} // namespace data
} // namespace atlas
