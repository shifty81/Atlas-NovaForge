#pragma once
#include "PlatformWindow.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

typedef struct HGLRC__* HGLRC;

namespace atlas::platform {

class Win32Window : public PlatformWindow {
public:
    Win32Window() = default;
    ~Win32Window() override;

    Win32Window(const Win32Window&) = delete;
    Win32Window& operator=(const Win32Window&) = delete;

    bool Init(const PlatformWindowConfig& config) override;
    void Shutdown() override;

    bool PollEvent(WindowEvent& event) override;

    void SwapBuffers() override;

    int32_t Width() const override { return m_width; }
    int32_t Height() const override { return m_height; }
    bool IsOpen() const override { return m_open; }

    HWND GetHWND() { return m_hwnd; }
    HDC GetHDC() { return m_hdc; }

private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    HWND m_hwnd = nullptr;
    HDC m_hdc = nullptr;
    HGLRC m_glContext = nullptr;
    HINSTANCE m_hInstance = nullptr;
    int32_t m_width = 0;
    int32_t m_height = 0;
    bool m_open = false;

    // Buffered events from WindowProc
    static constexpr int MAX_EVENTS = 64;
    WindowEvent m_eventQueue[MAX_EVENTS] = {};
    int m_eventHead = 0;
    int m_eventTail = 0;

    void PushEvent(const WindowEvent& event);
};

} // namespace atlas::platform

#endif // _WIN32
