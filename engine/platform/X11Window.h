#pragma once
#include "PlatformWindow.h"

typedef struct _XDisplay Display;
typedef unsigned long XID;
typedef XID Window;

typedef struct __GLXcontextRec* GLXContext;

namespace atlas::platform {

class X11Window : public PlatformWindow {
public:
    X11Window() = default;
    ~X11Window() override;

    X11Window(const X11Window&) = delete;
    X11Window& operator=(const X11Window&) = delete;

    bool Init(const PlatformWindowConfig& config) override;
    void Shutdown() override;

    bool PollEvent(WindowEvent& event) override;

    void SwapBuffers() override;

    int32_t Width() const override { return m_width; }
    int32_t Height() const override { return m_height; }
    bool IsOpen() const override { return m_open; }

    Display* GetDisplay() { return m_display; }
    ::Window GetWindow() { return m_window; }

private:
    Display* m_display = nullptr;
    ::Window m_window = 0;
    GLXContext m_glContext = nullptr;
    int32_t m_width = 0;
    int32_t m_height = 0;
    bool m_open = false;
    unsigned long m_deleteAtom = 0;
};

} // namespace atlas::platform
