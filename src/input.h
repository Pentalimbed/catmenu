#pragma once

#include <imgui.h>

namespace CatMenu
{

class InputHandler
{
private:
    ImGuiKey last_key_pressed = ImGuiKey_None;

    struct KeyEvent
    {
        explicit inline KeyEvent(const RE::ButtonEvent* a_event) :
            id_code(a_event->GetIDCode()),
            device(a_event->GetDevice()),
            type(a_event->GetEventType()),
            value(a_event->Value()),
            held_down_secs(a_event->HeldDuration()) {}

        explicit inline KeyEvent(const RE::CharEvent* a_event) :
            id_code(a_event->keyCode),
            device(a_event->GetDevice()),
            type(a_event->GetEventType()) {}

        [[nodiscard]] constexpr bool IsPressed() const noexcept { return value > 0.0F; }
        [[nodiscard]] constexpr bool IsRepeating() const noexcept { return held_down_secs > 0.0F; }
        [[nodiscard]] constexpr bool IsDown() const noexcept { return IsPressed() && (held_down_secs == 0.0F); }
        [[nodiscard]] constexpr bool IsHeld() const noexcept { return IsPressed() && IsRepeating(); }
        [[nodiscard]] constexpr bool IsUp() const noexcept { return (value == 0.0F) && IsRepeating(); }

        uint32_t             id_code;
        RE::INPUT_DEVICE     device;
        RE::INPUT_EVENT_TYPE type;
        float                value          = 0;
        float                held_down_secs = 0;
    };
    std::mutex            event_queue_mutex;
    std::vector<KeyEvent> event_queue;

public:
    static InputHandler* GetSingleton()
    {
        static InputHandler obj;
        return std::addressof(obj);
    }

    inline ImGuiKey GetLastKeyPressed() { return last_key_pressed; }
    void            ProcessEvents();

    struct BSInputDeviceManager_PollInputDevices
    {
        static void                                    thunk(RE::BSTEventSource<RE::InputEvent*>* a_dispatcher, RE::InputEvent* const* a_events);
        static inline REL::Relocation<decltype(thunk)> func;
    };
};


} // namespace CatMenu