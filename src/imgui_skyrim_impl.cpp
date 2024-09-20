#include "imgui_skyrim.h"

#include "ui.h"

namespace ImGui
{
namespace Skyrim
{

ImGuiContext* GetContext()
{
    return UI::GetSingleton()->GetContext();
}

APIResult RegisterMenuDrawFunc(std::string_view name, std::function<bool()> func)
{
    return UI::GetSingleton()->RegisterMenuDrawFunc(name, func);
}

void AddPopup(std::string_view)
{
}

} // namespace Skyrim
} // namespace ImGui