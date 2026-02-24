#include "EditorLayout.h"
#include <algorithm>
#include <cstring>

namespace atlas::editor {

void EditorLayout::RegisterPanel(EditorPanel* panel) {
    m_panels.push_back(panel);
}

bool EditorLayout::ClosePanel(const std::string& name) {
    for (auto* panel : m_panels) {
        if (name == panel->Name()) {
            if (!panel->IsClosable()) {
                return false;
            }
            panel->SetVisible(false);
            return true;
        }
    }
    return false;
}

EditorPanel* EditorLayout::FindPanel(const std::string& name) const {
    for (auto* panel : m_panels) {
        if (name == panel->Name()) {
            return panel;
        }
    }
    return nullptr;
}

void EditorLayout::Draw() {
    DrawNode(m_root);
}

void EditorLayout::DrawNode(DockNode& node) {
    if (node.split == DockSplit::None) {
        if (node.panel && node.panel->IsVisible()) {
            node.panel->Draw();
        }
        return;
    }

    if (node.a) DrawNode(*node.a);
    if (node.b) DrawNode(*node.b);
}

}
