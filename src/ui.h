#pragma once

#include "imgui.h"

#include "imgui_skyrim.h"

namespace ImGui
{
namespace Skyrim
{

struct DrawFunc
{
    std::string           name;
    std::function<bool()> func;
    bool                  enabled = true; // only for menu funcs
};

class UI
{
private:
    bool accept_registers_ = false;

    bool        show_menu_ = false;
    inline void Toggle(std::optional<bool> enabled = std::nullopt)
    {
        auto& io = ImGui::GetIO();
        io.ClearInputCharacters();
        io.ClearInputKeys();
        show_menu_ = enabled.value_or(!show_menu_);
    }

    std::mutex               list_mutex_;
    StringMap<DrawFunc>      draw_list_  = {};
    std::vector<std::string> draw_order_ = {};
    StringMap<DrawFunc>      menu_list_  = {};
    std::vector<std::string> menu_order_ = {};

    void DrawMenu();

public:
    static UI* GetSingleton()
    {
        static UI ui;
        return std::addressof(ui);
    }

    ImGuiContext* GetContext();
    APIResult     RegisterDrawFunc(std::string_view name, std::function<bool()> func);
    APIResult     RegisterMenuDrawFunc(std::string_view name, std::function<bool()> func);

    void Init(IDXGISwapChain* swapchain, ID3D11Device* device, ID3D11DeviceContext* context);
    void Draw();
};

} // namespace Skyrim
} // namespace ImGui
