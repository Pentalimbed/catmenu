#pragma once

#include <imgui.h>

namespace ImGui
{
namespace Skyrim
{

struct hk_PollInputDevices
{
    static void                                    thunk(RE::BSTEventSource<RE::InputEvent*>* a_dispatcher, RE::InputEvent* const* a_events);
    static inline REL::Relocation<decltype(thunk)> func;
};

ImGuiKey GetLastKeyPressed();

} // namespace Skyrim
} // namespace ImGui