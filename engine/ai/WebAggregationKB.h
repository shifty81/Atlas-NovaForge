#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>

namespace atlas::ai {

struct KBEntry {
    uint64_t id = 0;
    std::string source;
    std::string title;
    std::string content;
    std::string category;
    std::vector<std::string> tags;
    uint64_t timestamp = 0;
    double relevanceScore = 0.0;
};

struct KBSearchResult {
    std::vector<KBEntry> entries;
    std::string query;
    size_t totalMatches = 0;
};

class WebAggregationKB {
public:
    uint64_t AddEntry(const KBEntry& entry);
    void RemoveEntry(uint64_t id);
    const KBEntry* GetEntry(uint64_t id) const;
    size_t EntryCount() const;

    KBSearchResult Search(const std::string& query, size_t maxResults = 10) const;
    KBSearchResult SearchByCategory(const std::string& category) const;
    KBSearchResult SearchByTag(const std::string& tag) const;

    std::vector<std::string> ListCategories() const;
    size_t CategoryCount() const;

    std::vector<std::string> ListTags() const;

    std::string ExportJSON() const;
    bool ImportJSON(const std::string& json);

    void Clear();

private:
    std::unordered_map<uint64_t, KBEntry> m_entries;
    uint64_t m_nextID = 1;
};

} // namespace atlas::ai
