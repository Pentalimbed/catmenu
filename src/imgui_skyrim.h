// Copy this file and put alongside imgui headers.

#pragma once

#include <functional>

#include "imgui.h"

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
DLL_EXPORT APIResult     RegisterMenuDrawFunc(std::string_view name, std::function<bool()> func);
DLL_EXPORT void          AddPopup(std::string_view msg);

}; // namespace Skyrim
}; // namespace ImGui