#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

namespace atlas::production {

enum class CookResult {
    Success,
    SourceNotFound,
    CompileError,
    WriteError
};

struct CookEntry {
    std::string sourceId;
    std::string sourcePath;
    std::string outputPath;
    CookResult result = CookResult::Success;
};

struct CookStats {
    uint32_t totalAssets = 0;
    uint32_t cookedAssets = 0;
    uint32_t failedAssets = 0;
    uint32_t skippedAssets = 0;
};

class AssetCooker {
public:
    using ProgressCallback = std::function<void(const std::string& assetId, uint32_t current, uint32_t total)>;

    void SetOutputDir(const std::string& dir);
    const std::string& OutputDir() const;

    void SetStripEditorData(bool strip);
    bool StripEditorData() const;

    // Cook a single asset from source path to output
    CookResult CookAsset(const std::string& sourceId, const std::string& sourcePath);

    // Cook all assets from a directory
    CookStats CookAll(const std::string& sourceDir);

    // Set a progress callback
    void SetProgressCallback(ProgressCallback cb);

    // Get the cook log
    const std::vector<CookEntry>& CookLog() const;

    // Clear cook log
    void ClearLog();

private:
    std::string m_outputDir = "./build/cooked";
    bool m_stripEditorData = true;
    std::vector<CookEntry> m_cookLog;
    ProgressCallback m_progressCb;
};

}
