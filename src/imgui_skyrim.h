#include <functional>

#include "imgui.h"

#define DLL_EXPORT __declspec(dllexport)

namespace ImGui
{
namespace Skyrim
{

DLL_EXPORT ImGuiContext* GetContext();
DLL_EXPORT void          RegisterMenuDrawFunc(std::function<void()> func);

}; // namespace Skyrim
}; // namespace ImGui