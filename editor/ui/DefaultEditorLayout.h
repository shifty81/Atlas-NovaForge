#pragma once
#include <string>

namespace atlas::editor {

/// Returns the default editor layout expressed in the Atlas GUI DSL.
/// This is the bootstrap layout used when no user-customized layout
/// exists.  The editor UI is self-hosted: the editor shell itself is
/// described in the same DSL format used for game UI.
inline std::string DefaultEditorDSL() {
    return R"(layout "DefaultEditor" {
    split horizontal 0.20 {
        panel "AssetBrowser"
        split vertical 0.70 {
            panel "Viewport"
            split horizontal 0.70 {
                panel "Console"
                panel "ReplayTimeline"
            }
        }
    }
    dock right 0.22 {
        tabs {
            panel "Inspector"
            panel "ECSInspector"
            panel "DeterminismStatus"
        }
    }
    dock bottom 0.25 {
        tabs {
            panel "WorldOutliner"
            panel "Permissions"
        }
    }
})";
}

} // namespace atlas::editor
