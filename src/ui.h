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

        std::string font_path        = "Data\\SKSE\\Plugins\\imgui-skyrim\\fonts\\Atkinson-Hyperlegible-Regular-102.ttf";
        float       font_size        = 16.0f;
        bool        glyph_chn_full   = false;
        bool        glyph_chs_common = false;
        bool        glyph_greek      = false;
        bool        glyph_cyr        = false;
        bool        glyph_jap        = false;
        bool        glyph_kor        = false;
        bool        glyph_thai       = false;
        bool        glyph_viet       = false;
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
    StringMap<DrawFunc>      overlay_list;
    std::vector<std::string> overlay_order;
    StringMap<DrawFunc>      menu_list;
    std::vector<std::string> menu_order;

    bool should_setup_theme = false;
    void SetupTheme();

    bool    should_load_fonts = false;
    ImFont* main_font         = nullptr;
    void    LoadFonts();

    bool show_config = false;
    void DrawConfigWindow();

    void SaveSettings();
    void LoadSettings();

public:
    static UI* GetSingleton()
    {
        static UI ui;
        return std::addressof(ui);
    }

    inline bool IsMenuOpen() { return show_menu; }

    APIResult RegisterOverlayDrawFunc(std::string_view name, std::function<bool()> func);
    APIResult RegisterMenuDrawFunc(std::string_view name, std::function<bool()> func);

    void Init(IDXGISwapChain* swapchain, ID3D11Device* device, ID3D11DeviceContext* context);
    void Draw();
};

} // namespace Skyrim
} // namespace ImGui
