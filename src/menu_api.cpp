#include "menu_api.h"

#include "ui.h"

namespace CatMenu
{

ImGuiContext*
GetContext()
{
    return ImGui::GetCurrentContext();
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
} // namespace CatMenu