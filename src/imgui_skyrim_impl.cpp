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

APIResult RegisterOverlayDrawFunc(const RE::BSString& name, bool (*func)())
{
    return UI::GetSingleton()->RegisterOverlayDrawFunc(name, func);
}

APIResult RegisterMenuDrawFunc(const RE::BSString& name, bool (*func)())
{
    return UI::GetSingleton()->RegisterMenuDrawFunc(name, func);
}

void InsertNotification(const ImGuiToast& toast)
{
    ImGui::InsertNotification(toast);
}

} // namespace Skyrim
} // namespace ImGui