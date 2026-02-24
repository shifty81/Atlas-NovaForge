#pragma once
#include "../ui/EditorPanel.h"
#include "../../engine/production/GamePackager.h"
#include "../../engine/ui/UIDrawList.h"
#include <string>

namespace atlas::editor {

enum class BuildTarget {
    Client,
    Server,
};

enum class BuildMode {
    Debug,
    Development,
    Release,
};

struct PackageSettings {
    BuildTarget target = BuildTarget::Client;
    BuildMode mode = BuildMode::Debug;
    bool singleExe = false;
    bool includeMods = false;
    bool stripEditorData = true;
    std::string outputPath = "./build/output";
    std::string sourceDir = "assets";
};

class GamePackagerPanel : public EditorPanel {
public:
    const char* Name() const override { return "Game Packager"; }
    void Draw() override;

    const PackageSettings& Settings() const { return m_settings; }
    PackageSettings& Settings() { return m_settings; }

    std::string SettingsSummary() const;

    /// Execute the full packaging pipeline using current settings.
    production::PackageReport Build();

    /// Get the last build report.
    const production::PackageReport& LastReport() const { return m_lastReport; }

    /// Whether a build is currently in progress.
    bool IsBuilding() const { return m_building; }

    /// Current build stage name (e.g., "Cook", "Bundle").
    const std::string& CurrentStage() const { return m_currentStage; }

    /// Current build progress (0.0 to 1.0).
    float CurrentProgress() const { return m_currentProgress; }

    const atlas::ui::UIDrawList& GetDrawList() const { return m_drawList; }

private:
    PackageSettings m_settings;
    production::GamePackager m_packager;
    production::PackageReport m_lastReport;
    bool m_building = false;
    std::string m_currentStage;
    float m_currentProgress = 0.0f;
    atlas::ui::UIDrawList m_drawList;
};

}
