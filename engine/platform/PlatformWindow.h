#pragma once
#include <cstdint>
#include <string>

namespace atlas::platform {

/// Modifier key bitmask constants for WindowEvent::modifiers.
static constexpr uint8_t kModCtrl  = 1;
static constexpr uint8_t kModShift = 2;
static constexpr uint8_t kModAlt   = 4;

/// Platform-independent virtual key codes for non-printable keys.
/// On X11 these map to KeySym values; on Win32 to VK_ constants.
/// The platform window implementation translates native codes to these.
static constexpr uint32_t kKeyF3 = 0x100 + 3;

struct WindowEvent {
    enum class Type : uint8_t {
        NoEvent,
        Close,
        Resize,
        KeyDown,
        KeyUp,
        MouseMove,
        MouseButtonDown,
        MouseButtonUp,
        TextInput,
        ScrollWheel
    };

    Type type = Type::NoEvent;
    int32_t width = 0;
    int32_t height = 0;
    uint32_t keyCode = 0;
    int32_t mouseX = 0;
    int32_t mouseY = 0;
    uint8_t mouseButton = 0;
    char textChar = 0;
    uint8_t modifiers = 0; ///< Bitmask: 1=Ctrl, 2=Shift, 4=Alt
    float scrollDelta = 0.0f; ///< Scroll wheel delta (positive = down)
};

struct PlatformWindowConfig {
    std::string title = "Atlas Engine";
    int32_t width = 1280;
    int32_t height = 720;
    bool resizable = true;
};

class PlatformWindow {
public:
    virtual ~PlatformWindow() = default;

    virtual bool Init(const PlatformWindowConfig& config) = 0;
    virtual void Shutdown() = 0;

    virtual bool PollEvent(WindowEvent& event) = 0;

    virtual void SwapBuffers() = 0;

    virtual int32_t Width() const = 0;
    virtual int32_t Height() const = 0;
    virtual bool IsOpen() const = 0;
};

} // namespace atlas::platform
