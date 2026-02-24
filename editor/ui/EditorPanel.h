#pragma once
#include <string>

namespace atlas::editor {

class EditorPanel {
public:
    virtual ~EditorPanel() = default;
    virtual const char* Name() const = 0;
    virtual void Draw() = 0;
    virtual bool IsVisible() const { return m_visible; }
    virtual void SetVisible(bool visible) { m_visible = visible; }

    /// Returns true if this panel cannot be closed by the user.
    /// Non-closable panels may be collapsed or tabbed but never removed
    /// from the dock hierarchy.
    bool IsClosable() const { return m_closable; }
    void SetClosable(bool closable) { m_closable = closable; }

private:
    bool m_visible = true;
    bool m_closable = true;
};

}
