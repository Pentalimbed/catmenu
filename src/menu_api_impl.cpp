#include "menu_api_impl.h"

#include "ui.h"


namespace CatMenu
{

REL::Version API::GetVersion()
{
    return SKSE::PluginDeclaration::GetSingleton()->GetVersion();
}
ImGuiContext* API::GetContext()
{
    return ImGui::GetCurrentContext();
}

APIResult API::RegisterOverlayDrawFunc(const RE::BSString& name, bool (*func)())
{
    return UI::GetSingleton()->RegisterOverlayDrawFunc(name, func);
}

APIResult API::RegisterMenuDrawFunc(const RE::BSString& name, bool (*func)())
{
    return UI::GetSingleton()->RegisterMenuDrawFunc(name, func);
}

void API::InsertNotification(const ImGuiToast& toast)
{
    ImGui::InsertNotification(toast);
}

} // namespace CatMenu
