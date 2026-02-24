#include "CrashHandler.h"
#include <fstream>
#include <sstream>

namespace atlas::core {

void CrashHandler::AddBreadcrumb(const std::string& category, const std::string& message) {
    Breadcrumb bc;
    bc.timestamp = m_nextTimestamp++;
    bc.category = category;
    bc.message = message;
    m_breadcrumbs.push_back(std::move(bc));
    while (m_breadcrumbs.size() > m_maxBreadcrumbs) {
        m_breadcrumbs.erase(m_breadcrumbs.begin());
    }
}

void CrashHandler::SetMaxBreadcrumbs(size_t max) {
    m_maxBreadcrumbs = max;
    while (m_breadcrumbs.size() > m_maxBreadcrumbs) {
        m_breadcrumbs.erase(m_breadcrumbs.begin());
    }
}

size_t CrashHandler::MaxBreadcrumbs() const {
    return m_maxBreadcrumbs;
}

const std::vector<Breadcrumb>& CrashHandler::Breadcrumbs() const {
    return m_breadcrumbs;
}

void CrashHandler::ClearBreadcrumbs() {
    m_breadcrumbs.clear();
}

void CrashHandler::SetEngineVersion(const std::string& version) {
    m_engineVersion = version;
}

void CrashHandler::SetPlatform(const std::string& platform) {
    m_platform = platform;
}

void CrashHandler::SetCurrentTick(uint64_t tick) {
    m_currentTick = tick;
}

CrashReport CrashHandler::GenerateReport(const std::string& reason, const std::string& stackHint) const {
    CrashReport report;
    report.reason = reason;
    report.stackHint = stackHint;
    report.breadcrumbs = m_breadcrumbs;
    report.tickAtCrash = m_currentTick;
    report.engineVersion = m_engineVersion;
    report.platform = m_platform;
    return report;
}

bool CrashHandler::SaveReport(const CrashReport& report, const std::string& path) const {
    std::ofstream file(path);
    if (!file.is_open()) return false;

    file << "=== Atlas Crash Report ===\n";
    file << "Engine: " << report.engineVersion << "\n";
    file << "Platform: " << report.platform << "\n";
    file << "Tick: " << report.tickAtCrash << "\n";
    file << "Reason: " << report.reason << "\n";
    if (!report.stackHint.empty()) {
        file << "Stack: " << report.stackHint << "\n";
    }
    file << "\n--- Breadcrumbs (" << report.breadcrumbs.size() << ") ---\n";
    for (const auto& bc : report.breadcrumbs) {
        file << "[" << bc.timestamp << "] [" << bc.category << "] " << bc.message << "\n";
    }

    return true;
}

void CrashHandler::SetCrashCallback(CrashCallback cb) {
    m_crashCb = std::move(cb);
}

void CrashHandler::ReportCrash(const std::string& reason, const std::string& outputPath) {
    CrashReport report = GenerateReport(reason);

    if (!outputPath.empty()) {
        SaveReport(report, outputPath);
    }

    if (m_crashCb) {
        m_crashCb(report);
    }
}

}
