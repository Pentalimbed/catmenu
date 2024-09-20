#include "ui.h"

#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

namespace ImGui
{
namespace Skyrim
{

ImGuiContext* UI::GetContext()
{
    return ImGui::GetCurrentContext();
}

APIResult UI::RegisterDrawFunc(std::string_view name, std::function<bool()> func)
{
    std::lock_guard list_lock{list_mutex_};

    if (accept_registers_)
    {
        logger::warn("Trying to register draw func {} before imgui is initialised. Aborted.", name);
        return APIResult::NotRegistered;
    }

    if (auto it = menu_list_.find(name); it == menu_list_.end())
    {
        logger::warn("Trying to register draw func {} which already exists. Aborted.", name);
        return APIResult::AlreadyRegistered;
    }

    std::string name_str{name};
    draw_list_.insert({name_str, DrawFunc{name_str, func, true}});
    draw_order_.push_back(name_str);
    logger::warn("Draw func {} registered.", name);
    return APIResult::OK;
}

APIResult UI::RegisterMenuDrawFunc(std::string_view name, std::function<bool()> func)
{
    std::lock_guard list_lock{list_mutex_};

    if (accept_registers_)
    {
        logger::warn("Trying to register menu draw func {} before imgui is initialised. Aborted.", name);
        return APIResult::NotRegistered;
    }

    if (auto it = menu_list_.find(name); it == menu_list_.end())
    {
        logger::warn("Trying to register menu draw func {} which already exists. Aborted.", name);
        return APIResult::AlreadyRegistered;
    }

    std::string name_str{name};
    menu_list_.insert({name_str, DrawFunc{name_str, func, false}});
    menu_order_.push_back(name_str);
    logger::warn("Menu draw func {} registered.", name);
    return APIResult::OK;
}

void UI::Init(IDXGISwapChain* swapchain, ID3D11Device* device, ID3D11DeviceContext* context)
{
    logger::info("Initializing ImGui...");

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    auto& imgui_io = ImGui::GetIO();

    imgui_io.ConfigFlags  = ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_DockingEnable;
    imgui_io.BackendFlags = ImGuiBackendFlags_HasMouseCursors | ImGuiBackendFlags_RendererHasVtxOffset;

    // imgui_io.Fonts->AddFontFromFileTTF("Data\\Interface\\CommunityShaders\\Fonts\\Atkinson-Hyperlegible-Regular-102.ttf", 24);

    DXGI_SWAP_CHAIN_DESC desc;
    swapchain->GetDesc(&desc);

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(desc.OutputWindow);
    ImGui_ImplDX11_Init(device, context);

    accept_registers_ = true;
}

void UI::Draw()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    {
        std::lock_guard list_lock{list_mutex_};
        for (std::string_view name : draw_order_)
            draw_list_.at(name).func();

        ImGui::ShowDemoWindow();

        for (std::string_view name : draw_order_)
            draw_list_.at(name).func();
    }

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

} // namespace Skyrim
} // namespace ImGui
