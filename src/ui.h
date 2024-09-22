#pragma once

#include "imgui.h"

#include "menu_api.h"

namespace CatMenu
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

        std::string font_path        = "Data\\SKSE\\Plugins\\catmenu\\fonts\\Atkinson-Hyperlegible-Regular-102.ttf";
        float       font_size        = 16.0f;
        bool        glyph_chn_full   = false;
        bool        glyph_chs_common = false;
        bool        glyph_greek      = false;
        bool        glyph_cyr        = false;
        bool        glyph_jap        = false;
        bool        glyph_kor        = false;
        bool        glyph_thai       = false;
        bool        glyph_viet       = false;


        // Theme by @Maksasj, edited by FiveLimbedCat
        // url: https://github.com/ocornut/imgui/issues/707#issuecomment-1494706165
        std::array<ImVec4, ImGuiCol_COUNT> theme_colors{
            ImVec4(0.9f, 0.9f, 0.9f, 0.9f),
            ImVec4(0.6f, 0.6f, 0.6f, 1.0f),
            ImVec4(0.1f, 0.1f, 0.15f, 1.0f),
            ImVec4(0.0f, 0.0f, 0.0f, 0.0f),
            ImVec4(0.05f, 0.05f, 0.1f, 0.85f),
            ImVec4(0.7f, 0.7f, 0.7f, 0.65f),
            ImVec4(0.0f, 0.0f, 0.0f, 0.0f),
            ImVec4(0.0f, 0.0f, 0.0f, 1.0f),
            ImVec4(0.9f, 0.8f, 0.8f, 0.4f),
            ImVec4(0.9f, 0.65f, 0.65f, 0.45f),
            ImVec4(0.0f, 0.0f, 0.0f, 0.83f),
            ImVec4(0.0f, 0.0f, 0.0f, 0.87f),
            ImVec4(0.4f, 0.4f, 0.8f, 0.2f),
            ImVec4(0.01f, 0.01f, 0.02f, 0.8f),
            ImVec4(0.2f, 0.25f, 0.3f, 0.6f),
            ImVec4(0.55f, 0.53f, 0.55f, 0.51f),
            ImVec4(0.56f, 0.56f, 0.56f, 1.0f),
            ImVec4(0.56f, 0.56f, 0.56f, 0.91f),
            ImVec4(0.9f, 0.9f, 0.9f, 0.83f),
            ImVec4(0.7f, 0.7f, 0.7f, 0.62f),
            ImVec4(0.3f, 0.3f, 0.3f, 0.84f),
            ImVec4(0.48f, 0.72f, 0.89f, 0.49f),
            ImVec4(0.5f, 0.69f, 0.99f, 0.68f),
            ImVec4(0.8f, 0.5f, 0.5f, 1.0f),
            ImVec4(0.3f, 0.69f, 1.0f, 0.53f),
            ImVec4(0.44f, 0.61f, 0.86f, 1.0f),
            ImVec4(0.38f, 0.62f, 0.83f, 1.0f),
            ImVec4(0.5f, 0.5f, 0.5f, 1.0f),
            ImVec4(0.7f, 0.6f, 0.6f, 1.0f),
            ImVec4(0.9f, 0.7f, 0.7f, 1.0f),
            ImVec4(1.0f, 1.0f, 1.0f, 0.85f),
            ImVec4(1.0f, 1.0f, 1.0f, 0.6f),
            ImVec4(1.0f, 1.0f, 1.0f, 0.9f),
            ImVec4(0.0f, 0.46f, 1.0f, 0.8f),   // TabHovered
            ImVec4(0.4f, 0.52f, 0.67f, 0.84f), // Tab
            ImVec4(0.2f, 0.41f, 0.68f, 1.0f),  // TabSelected
            ImVec4(0.2f, 0.41f, 0.68f, 1.0f),
            ImVec4(0.07f, 0.1f, 0.15f, 0.97f), // TabDimmed
            ImVec4(0.13f, 0.26f, 0.42f, 1.0f), // TabDimmedSelected
            ImVec4(0.13f, 0.26f, 0.42f, 1.0f),
            ImVec4(0.7f, 0.6f, 0.6f, 0.5f), // DockingPreview
            ImVec4(0.0f, 0.0f, 0.0f, 0.0f), // DockingEmptyBg
            ImVec4(1.0f, 1.0f, 1.0f, 1.0f), // PlotLines
            ImVec4(0.0f, 0.87f, 1.0f, 1.0f),
            ImVec4(0.22f, 0.26f, 0.7f, 1.0f),
            ImVec4(0.8f, 0.26f, 0.26f, 1.0f),
            ImVec4(0.48f, 0.72f, 0.89f, 0.49f),
            ImVec4(0.3f, 0.3f, 0.35f, 1.0f),
            ImVec4(0.23f, 0.23f, 0.25f, 1.0f),
            ImVec4(0.0f, 0.0f, 0.0f, 0.0f),
            ImVec4(1.0f, 1.0f, 1.0f, 0.06f),
            ImVec4(0.35f, 0.48f, 0.85f, 1.0f), // TextLink
            ImVec4(0.0f, 0.0f, 1.0f, 0.35f),   // TextSelectedBg
            ImVec4(0.8f, 0.5f, 0.5f, 1.0f),
            ImVec4(0.44f, 0.61f, 0.86f, 1.0f),
            ImVec4(0.3f, 0.3f, 0.3f, 0.56f),
            ImVec4(0.2f, 0.2f, 0.2f, 0.35f),
            ImVec4(0.2f, 0.2f, 0.2f, 0.35f),
        };
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

    void SetupTheme();

    bool    should_load_fonts = false;
    ImFont* main_font         = nullptr;
    void    LoadFonts();

    bool show_config = false;
    void DrawConfigWindow();

    bool show_theme_editor = false;
    void DrawThemeEditor();

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
} // namespace CatMenu
