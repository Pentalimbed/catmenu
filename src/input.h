#pragma once

#include <imgui.h>

namespace CatMenu
{

struct BSInputDeviceManager_PollInputDevices
{
    static void                                    thunk(RE::BSTEventSource<RE::InputEvent*>* a_dispatcher, RE::InputEvent* const* a_events);
    static inline REL::Relocation<decltype(thunk)> func;
};

ImGuiKey GetLastKeyPressed();
} // namespace CatMenu