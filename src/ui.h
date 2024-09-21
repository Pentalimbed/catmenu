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
public:
    struct Settings
    {
        int toggle_key = ImGuiKey_Backslash;
    };

private:
    bool        show_menu = false;
    inline void Toggle(std::optional<bool> enabled = std::nullopt)
    {
        auto& io = ImGui::GetIO();
        io.ClearInputCharacters();
        io.ClearInputKeys();
        show_menu = enabled.value_or(!show_menu);
    }

    Settings settings;

    std::mutex               list_mutex;
    StringMap<DrawFunc>      overlay_list  = {};
    std::vector<std::string> overlay_order = {};
    StringMap<DrawFunc>      menu_list     = {};
    std::vector<std::string> menu_order    = {};

    void SetupTheme();

    bool show_config = false;
    void DrawConfigWindow();

public:
    static UI* GetSingleton()
    {
        static UI ui;
        return std::addressof(ui);
    }

    ImGuiContext* GetContext();
    APIResult     RegisterOverlayDrawFunc(std::string_view name, std::function<bool()> func);
    APIResult     RegisterMenuDrawFunc(std::string_view name, std::function<bool()> func);

    void Init(IDXGISwapChain* swapchain, ID3D11Device* device, ID3D11DeviceContext* context);
    void Draw();
};

} // namespace Skyrim
} // namespace ImGui
