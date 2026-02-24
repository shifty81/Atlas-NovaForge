#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <functional>

namespace atlas::core {

struct Breadcrumb {
    uint64_t timestamp = 0;
    std::string category;
    std::string message;
};

struct CrashReport {
    std::string reason;
    std::string stackHint;
    std::vector<Breadcrumb> breadcrumbs;
    uint64_t tickAtCrash = 0;
    std::string engineVersion;
    std::string platform;
};

class CrashHandler {
public:
    using CrashCallback = std::function<void(const CrashReport&)>;

    // Breadcrumb trail
    void AddBreadcrumb(const std::string& category, const std::string& message);
    void SetMaxBreadcrumbs(size_t max);
    size_t MaxBreadcrumbs() const;
    const std::vector<Breadcrumb>& Breadcrumbs() const;
    void ClearBreadcrumbs();

    // Crash context
    void SetEngineVersion(const std::string& version);
    void SetPlatform(const std::string& platform);
    void SetCurrentTick(uint64_t tick);

    // Generate crash report
    CrashReport GenerateReport(const std::string& reason, const std::string& stackHint = "") const;

    // Save crash report to file
    bool SaveReport(const CrashReport& report, const std::string& path) const;

    // Callback on crash
    void SetCrashCallback(CrashCallback cb);

    // Trigger crash report (generates + saves + calls callback)
    void ReportCrash(const std::string& reason, const std::string& outputPath = "");

private:
    std::vector<Breadcrumb> m_breadcrumbs;
    size_t m_maxBreadcrumbs = 100;
    std::string m_engineVersion = "0.0.0";
    std::string m_platform = "Unknown";
    uint64_t m_currentTick = 0;
    uint64_t m_nextTimestamp = 1;
    CrashCallback m_crashCb;
};

}
