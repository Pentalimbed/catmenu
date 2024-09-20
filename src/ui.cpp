#include "ui.h"

#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <ImGuiNotify.hpp>

namespace ImGui
{
namespace Skyrim
{

ImGuiContext* UI::GetContext()
{
    return ImGui::GetCurrentContext();
}

APIResult UI::RegisterOverlayDrawFunc(std::string_view name, std::function<bool()> func)
{
    std::lock_guard list_lock{list_mutex_};

    if (auto it = menu_list_.find(name); it != menu_list_.end()) {
        logger::warn("Trying to register draw func {} which already exists. Aborted.", name);
        return APIResult::AlreadyRegistered;
    }

    std::string name_str{name};
    overlay_list_.insert({name_str, DrawFunc{name_str, func, true}});
    overlay_order_.push_back(name_str);

    logger::info("Draw func {} registered.", name);
    ImGui::InsertNotification({ImGuiToastType::Success, 7000, "Overlay [%s] registered.", name.data()});

    return APIResult::OK;
}

APIResult UI::RegisterMenuDrawFunc(std::string_view name, std::function<bool()> func)
{
    std::lock_guard list_lock{list_mutex_};

    if (auto it = menu_list_.find(name); it != menu_list_.end()) {
        logger::warn("Trying to register menu draw func {} which already exists. Aborted.", name);
        return APIResult::AlreadyRegistered;
    }

    std::string name_str{name};
    menu_list_.insert({name_str, DrawFunc{name_str, func, false}});
    menu_order_.push_back(name_str);
    logger::info("Menu draw func {} registered.", name);
    ImGui::InsertNotification({ImGuiToastType::Success, 7000, "Menu [%s] registered.", name.data()});

    return APIResult::OK;
}

void UI::Init(IDXGISwapChain* swapchain, ID3D11Device* device, ID3D11DeviceContext* context)
{
    logger::info("Initializing ImGui...");

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // Setup Platform/Renderer backends
    DXGI_SWAP_CHAIN_DESC desc;
    swapchain->GetDesc(&desc);

    ImGui_ImplWin32_Init(desc.OutputWindow);
    ImGui_ImplDX11_Init(device, context);

    auto& imgui_io        = ImGui::GetIO();
    imgui_io.ConfigFlags  = ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_DockingEnable;
    imgui_io.BackendFlags = ImGuiBackendFlags_HasMouseCursors | ImGuiBackendFlags_RendererHasVtxOffset;

    SetupTheme();

    // fonts
    float baseFontSize = 24.0f;
    float iconFontSize = baseFontSize * 2.0f / 3.0f; // FontAwesome fonts need to have their sizes reduced by 2.0f/3.0f in order to align correctly

    // imgui_io.Fonts->AddFontFromFileTTF("Data\\Interface\\CommunityShaders\\Fonts\\Atkinson-Hyperlegible-Regular-102.ttf", 24);
    imgui_io.Fonts->AddFontDefault();

    // add font awesome 6
    static const ImWchar icons_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
    ImFontConfig         icons_config;
    icons_config.MergeMode            = true;
    icons_config.PixelSnapH           = true;
    icons_config.FontDataOwnedByAtlas = true;

    imgui_io.Fonts->AddFontFromFileTTF("Data\\SKSE\\Plugins\\imgui-skyrim\\fonts\\fa-solid-900.ttf", iconFontSize, &icons_config, icons_ranges);

    auto ver = SKSE::PluginDeclaration::GetSingleton()->GetVersion();
    ImGui::InsertNotification({ImGuiToastType::Success, 7000,
                               std::format("ImGui-Skyrim ({}.{}.{}) initialized.", ver.major(), ver.minor(), ver.patch()).c_str()});
}

void UI::Draw()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    {
        std::lock_guard list_lock{list_mutex_};
        for (std::string_view name : overlay_order_)
            overlay_list_.at(name).func();

        if (ImGui::IsKeyPressed(ImGuiKey_Backslash, false))
            Toggle();

        auto& io           = ImGui::GetIO();
        io.MouseDrawCursor = show_menu_;

        if (show_menu_) {
            const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + main_viewport->Size.x * 0.1f,
                                           main_viewport->WorkPos.y + main_viewport->Size.y * 0.1f),
                                    ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowSize(ImVec2(main_viewport->Size.x * 0.2f, main_viewport->Size.y * 0.6f), ImGuiCond_FirstUseEver);
            if (ImGui::Begin("List of Menus", &show_menu_, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {

                if (ImGui::Button("OFC"))
                    ImGui::InsertNotification({ImGuiToastType::Success, 7000, "Bump."});

                if (ImGui::BeginListBox("##Menus", {-FLT_MIN, -FLT_MIN})) {
                    for (std::string_view name : menu_order_) {
                        auto& menu_func = menu_list_.at(name);
                        ImGui::Selectable(menu_func.name.c_str(), &menu_func.enabled);
                    }
                }
                ImGui::End();
            }
            if (!show_menu_)
                Toggle(false);

            for (std::string_view name : menu_order_)
                if (auto& menu_func = menu_list_.at(name); menu_func.enabled)
                    menu_func.enabled = menu_func.func();
        }

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.f);
        ImGui::RenderNotifications(); // <-- Here we render all notifications
        ImGui::PopStyleVar(1);
    }

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void UI::SetupTheme()
{
    auto& style  = ImGui::GetStyle();
    auto& colors = style.Colors;

    style.WindowTitleAlign = ImVec2(0.5, 0.5);
    style.FramePadding     = ImVec2(4, 4);

    ImVec4 palette[] = {
        {22.f / 255.f, 38.f / 255.f, 46.f / 255.f, 1.0f},
        {46.f / 255.f, 71.f / 255.f, 86.f / 255.f, 1.0f},
        {60.f / 255.f, 122.f / 255.f, 137.f / 255.f, 1.0f},
        {159.f / 255.f, 162.f / 255.f, 178.f / 255.f, 1.0f},
        {64.f / 255.f, 98.f / 255.f, 119.f / 255.f, 1.0f} // slightly brighter version of [1]
    };

    // Window
    colors[ImGuiCol_WindowBg]          = ImVec4{palette[0].x, palette[0].y, palette[0].z, 0.784f};
    colors[ImGuiCol_ResizeGrip]        = ImVec4{palette[2].x, palette[2].y, palette[2].z, 0.5f};
    colors[ImGuiCol_ResizeGripHovered] = ImVec4{palette[3].x, palette[3].y, palette[3].z, 0.75f};
    colors[ImGuiCol_ResizeGripActive]  = palette[1];

    // Header
    colors[ImGuiCol_Header]        = palette[4];
    colors[ImGuiCol_HeaderHovered] = palette[3];
    colors[ImGuiCol_HeaderActive]  = palette[2];

    // Title
    colors[ImGuiCol_TitleBg]          = palette[1];
    colors[ImGuiCol_TitleBgActive]    = palette[1];
    colors[ImGuiCol_TitleBgCollapsed] = palette[1];

    // Frame Background
    colors[ImGuiCol_FrameBg]        = palette[1];
    colors[ImGuiCol_FrameBgHovered] = palette[3];
    colors[ImGuiCol_FrameBgActive]  = palette[2];

    // Button
    colors[ImGuiCol_Button]        = palette[1];
    colors[ImGuiCol_ButtonHovered] = palette[3];
    colors[ImGuiCol_ButtonActive]  = palette[2];

    // Tab
    colors[ImGuiCol_Tab]                = palette[1];
    colors[ImGuiCol_TabHovered]         = ImVec4{0.38f, 0.38f, 0.38f, 1.0f};
    colors[ImGuiCol_TabActive]          = ImVec4{0.28f, 0.28f, 0.28f, 1.0f};
    colors[ImGuiCol_TabUnfocused]       = palette[1];
    colors[ImGuiCol_TabUnfocusedActive] = palette[2];
}

} // namespace Skyrim
} // namespace ImGui