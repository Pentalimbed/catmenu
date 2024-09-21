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
    std::lock_guard list_lock{list_mutex};

    if (auto it = menu_list.find(name); it != menu_list.end()) {
        logger::warn("Trying to register draw func {} which already exists. Aborted.", name);
        return APIResult::AlreadyRegistered;
    }

    std::string name_str{name};
    overlay_list.insert({name_str, DrawFunc{name_str, func, true}});
    overlay_order.push_back(name_str);

    logger::info("Draw func {} registered.", name);
    ImGui::InsertNotification({ImGuiToastType::Success, 5000, "Overlay: %s registered.", name.data()});

    return APIResult::OK;
}

APIResult UI::RegisterMenuDrawFunc(std::string_view name, std::function<bool()> func)
{
    std::lock_guard list_lock{list_mutex};

    if (auto it = menu_list.find(name); it != menu_list.end()) {
        logger::warn("Trying to register menu draw func {} which already exists. Aborted.", name);
        return APIResult::AlreadyRegistered;
    }

    std::string name_str{name};
    menu_list.insert({name_str, DrawFunc{name_str, func, false}});
    menu_order.push_back(name_str);
    logger::info("Menu draw func {} registered.", name);
    ImGui::InsertNotification({ImGuiToastType::Success, 5000, "Menu: %s registered.", name.data()});

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

    logger::info("ImGui initialized.");

    ///////////////////////// CONFIG
    auto& imgui_io = ImGui::GetIO();

    imgui_io.ConfigFlags  = ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_DockingEnable;
    imgui_io.BackendFlags = ImGuiBackendFlags_HasMouseCursors | ImGuiBackendFlags_RendererHasVtxOffset;

    SetupTheme();

    ///////////////////////// FONTS
    float base_font_size = 20.0f;
    // float icon_font_size = base_font_size * 2.0f / 3.0f; // FontAwesome fonts need to have their sizes reduced by 2.0f/3.0f in order to align correctly

    // add atkinson hyperlegible
    imgui_io.Fonts->AddFontFromFileTTF("Data\\SKSE\\Plugins\\imgui-skyrim\\fonts\\Atkinson-Hyperlegible-Regular-102.ttf", base_font_size);

    // add font awesome 6
    static const ImWchar icons_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
    ImFontConfig         icons_config;
    icons_config.MergeMode            = true;
    icons_config.PixelSnapH           = true;
    icons_config.FontDataOwnedByAtlas = true;

    imgui_io.Fonts->AddFontFromFileTTF("Data\\SKSE\\Plugins\\imgui-skyrim\\fonts\\fa-solid-900.ttf", base_font_size, &icons_config, icons_ranges);

    auto ver = SKSE::PluginDeclaration::GetSingleton()->GetVersion();
    auto msg = std::format("ImGui-Skyrim ({}.{}.{}) initialized.", ver.major(), ver.minor(), ver.patch());
    logger::info("{}", msg);
    ImGui::InsertNotification({ImGuiToastType::Success, 5000, msg.c_str()});
}


void UI::SetupTheme()
{
    auto& style  = ImGui::GetStyle();
    auto& colors = style.Colors;

    // https://github.com/ocornut/imgui/issues/707#issuecomment-1494706165

    style.WindowRounding    = 5.3f;
    style.FrameRounding     = 2.3f;
    style.ScrollbarRounding = 0;

    colors[ImGuiCol_Text]                 = ImVec4(0.90f, 0.90f, 0.90f, 0.90f);
    colors[ImGuiCol_TextDisabled]         = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    colors[ImGuiCol_WindowBg]             = ImVec4(0.09f, 0.09f, 0.15f, 1.00f);
    colors[ImGuiCol_ChildBg]              = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_PopupBg]              = ImVec4(0.05f, 0.05f, 0.10f, 0.85f);
    colors[ImGuiCol_Border]               = ImVec4(0.70f, 0.70f, 0.70f, 0.65f);
    colors[ImGuiCol_BorderShadow]         = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg]              = ImVec4(0.00f, 0.00f, 0.01f, 1.00f);
    colors[ImGuiCol_FrameBgHovered]       = ImVec4(0.90f, 0.80f, 0.80f, 0.40f);
    colors[ImGuiCol_FrameBgActive]        = ImVec4(0.90f, 0.65f, 0.65f, 0.45f);
    colors[ImGuiCol_TitleBg]              = ImVec4(0.00f, 0.00f, 0.00f, 0.83f);
    colors[ImGuiCol_TitleBgCollapsed]     = ImVec4(0.40f, 0.40f, 0.80f, 0.20f);
    colors[ImGuiCol_TitleBgActive]        = ImVec4(0.00f, 0.00f, 0.00f, 0.87f);
    colors[ImGuiCol_MenuBarBg]            = ImVec4(0.01f, 0.01f, 0.02f, 0.80f);
    colors[ImGuiCol_ScrollbarBg]          = ImVec4(0.20f, 0.25f, 0.30f, 0.60f);
    colors[ImGuiCol_ScrollbarGrab]        = ImVec4(0.55f, 0.53f, 0.55f, 0.51f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.56f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.56f, 0.56f, 0.56f, 0.91f);
    colors[ImGuiCol_CheckMark]            = ImVec4(0.90f, 0.90f, 0.90f, 0.83f);
    colors[ImGuiCol_SliderGrab]           = ImVec4(0.70f, 0.70f, 0.70f, 0.62f);
    colors[ImGuiCol_SliderGrabActive]     = ImVec4(0.30f, 0.30f, 0.30f, 0.84f);
    colors[ImGuiCol_Button]               = ImVec4(0.48f, 0.72f, 0.89f, 0.49f);
    colors[ImGuiCol_ButtonHovered]        = ImVec4(0.50f, 0.69f, 0.99f, 0.68f);
    colors[ImGuiCol_ButtonActive]         = ImVec4(0.80f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_Header]               = ImVec4(0.30f, 0.69f, 1.00f, 0.53f);
    colors[ImGuiCol_HeaderHovered]        = ImVec4(0.44f, 0.61f, 0.86f, 1.00f);
    colors[ImGuiCol_HeaderActive]         = ImVec4(0.38f, 0.62f, 0.83f, 1.00f);
    colors[ImGuiCol_Separator]            = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_SeparatorHovered]     = ImVec4(0.70f, 0.60f, 0.60f, 1.00f);
    colors[ImGuiCol_SeparatorActive]      = ImVec4(0.90f, 0.70f, 0.70f, 1.00f);
    colors[ImGuiCol_ResizeGrip]           = ImVec4(1.00f, 1.00f, 1.00f, 0.85f);
    colors[ImGuiCol_ResizeGripHovered]    = ImVec4(1.00f, 1.00f, 1.00f, 0.60f);
    colors[ImGuiCol_ResizeGripActive]     = ImVec4(1.00f, 1.00f, 1.00f, 0.90f);
    colors[ImGuiCol_PlotLines]            = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]     = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogram]        = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TextSelectedBg]       = ImVec4(0.00f, 0.00f, 1.00f, 0.35f);
    colors[ImGuiCol_ModalWindowDimBg]     = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);

    // style.WindowTitleAlign = ImVec2(0.5, 0.5);
    // style.FramePadding     = ImVec2(4, 4);

    // ImVec4 palette[] = {
    //     {22.f / 255.f, 38.f / 255.f, 46.f / 255.f, 1.0f},
    //     {46.f / 255.f, 71.f / 255.f, 86.f / 255.f, 1.0f},
    //     {60.f / 255.f, 122.f / 255.f, 137.f / 255.f, 1.0f},
    //     {159.f / 255.f, 162.f / 255.f, 178.f / 255.f, 1.0f},
    //     {64.f / 255.f, 98.f / 255.f, 119.f / 255.f, 1.0f} // slightly brighter version of [1]
    // };

    // // Window
    // colors[ImGuiCol_WindowBg]          = ImVec4{palette[0].x, palette[0].y, palette[0].z, 0.784f};
    // colors[ImGuiCol_ResizeGrip]        = ImVec4{palette[2].x, palette[2].y, palette[2].z, 0.5f};
    // colors[ImGuiCol_ResizeGripHovered] = ImVec4{palette[3].x, palette[3].y, palette[3].z, 0.75f};
    // colors[ImGuiCol_ResizeGripActive]  = palette[1];

    // // Header
    // colors[ImGuiCol_Header]        = palette[4];
    // colors[ImGuiCol_HeaderHovered] = palette[3];
    // colors[ImGuiCol_HeaderActive]  = palette[2];

    // // Title
    // colors[ImGuiCol_TitleBg]          = palette[1];
    // colors[ImGuiCol_TitleBgActive]    = palette[1];
    // colors[ImGuiCol_TitleBgCollapsed] = palette[1];
    // colors[ImGuiCol_MenuBarBg]        = palette[1];

    // // Frame Background
    // colors[ImGuiCol_FrameBg]        = palette[1];
    // colors[ImGuiCol_FrameBgHovered] = palette[3];
    // colors[ImGuiCol_FrameBgActive]  = palette[2];

    // // Button
    // colors[ImGuiCol_Button]        = palette[1];
    // colors[ImGuiCol_ButtonHovered] = palette[3];
    // colors[ImGuiCol_ButtonActive]  = palette[2];

    // // Tab
    // colors[ImGuiCol_Tab]                = palette[1];
    // colors[ImGuiCol_TabHovered]         = ImVec4{0.38f, 0.38f, 0.38f, 1.0f};
    // colors[ImGuiCol_TabActive]          = ImVec4{0.28f, 0.28f, 0.28f, 1.0f};
    // colors[ImGuiCol_TabUnfocused]       = palette[1];
    // colors[ImGuiCol_TabUnfocusedActive] = palette[2];
}

void UI::Draw()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    {
        std::lock_guard list_lock{list_mutex};

        for (std::string_view name : overlay_order)
            overlay_list.at(name).func();

        if (ImGui::IsKeyPressed((ImGuiKey)settings.toggle_key, false))
            Toggle();

        auto& io           = ImGui::GetIO();
        io.MouseDrawCursor = show_menu;

        if (show_menu) {
            // menu bar
            if (ImGui::BeginMainMenuBar()) {
                if (ImGui::BeginMenu("Windows")) {
                    for (std::string_view name : menu_order) {
                        auto& menu_func = menu_list.at(name);
                        ImGui::MenuItem(menu_func.name.c_str(), nullptr, &menu_func.enabled);
                    }

                    ImGui::Separator();

                    if (ImGui::MenuItem("Close Windows"))
                        for (auto& [_, menu_func] : menu_list)
                            menu_func.enabled = false;
                    if (ImGui::IsItemHovered())
                        ImGui::SetTooltip("Close all menu windows.");

                    if (ImGui::MenuItem("Close"))
                        Toggle(false);
                    if (ImGui::IsItemHovered())
                        ImGui::SetTooltip("Close imgui-skyrim and resume game.");

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Settings")) {
                    if (ImGui::MenuItem("General"))
                        show_config = !show_config;

                    if (ImGui::MenuItem("Palette Editor")) {
                    }

                    ImGui::Separator();

                    ImGui::MenuItem("Save");
                    ImGui::MenuItem("Load");

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("About")) {
                    if (ImGui::MenuItem("Info")) {
                        auto ver = SKSE::PluginDeclaration::GetSingleton()->GetVersion();
                        auto msg = std::format(
                            "ImGui-Skyrim version {}.{}.{}.\n"
                            "Programmed by FiveLimbedCat/ProfJack.",
                            ver.major(), ver.minor(), ver.patch());
                        ImGui::InsertNotification({ImGuiToastType::Info, 10000, msg.c_str()});
                    }
                    ImGui::EndMenu();
                }

                ImGui::EndMainMenuBar();
            }

            // set up dockspace
            constexpr ImGuiWindowFlags dockspace_flags =
                ImGuiWindowFlags_NoNav |
                ImGuiWindowFlags_NoDocking |
                ImGuiWindowFlags_NoInputs |
                ImGuiWindowFlags_NoSavedSettings |
                ImGuiWindowFlags_NoBackground |
                ImGuiWindowFlags_NoDecoration;

            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);

            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
            if (ImGui::Begin("Main", NULL, dockspace_flags))
                ImGui::DockSpace(ImGui::GetID("Dockspace"), ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
            ImGui::End();
            ImGui::PopStyleVar();

            // default menus
            if (show_config)
                DrawConfigWindow();

            // registered menus
            for (std::string_view name : menu_order)
                if (auto& menu_func = menu_list.at(name); menu_func.enabled)
                    menu_func.enabled = menu_func.func();
        }

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);   // Disable round borders
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f); // Disable borders
        ImGui::RenderNotifications();                             // <-- Here we render all notifications
        ImGui::PopStyleVar(2);
    }

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void UI::DrawConfigWindow()
{
    if (!ImGui::Begin("ImGui-Skyrim Config", &show_config)) {
        ImGui::End();
        return;
    }

    ImGui::End();
}

} // namespace Skyrim
} // namespace ImGui