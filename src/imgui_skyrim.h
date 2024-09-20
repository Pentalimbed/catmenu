// Copy this file and put alongside imgui headers.

#pragma once

#include <functional>

#include <imgui.h>
#include <ImGuiNotify.hpp>

#define DLL_EXPORT __declspec(dllexport)

namespace ImGui
{
namespace Skyrim
{

enum class APIResult : uint8_t
{
    OK,
    AlreadyRegistered,
    NotRegistered,
};

DLL_EXPORT ImGuiContext* GetContext();
DLL_EXPORT APIResult     RegisterOverlayDrawFunc(const RE::BSString& name, bool (*func)());
DLL_EXPORT APIResult     RegisterMenuDrawFunc(const RE::BSString& name, bool (*func)());
DLL_EXPORT void          InsertNotification(const ImGuiToast& toast);

} // namespace Skyrim
} // namespace ImGui