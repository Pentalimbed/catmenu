#include "ui.h"

#include "input.h"

#include <magic_enum.hpp>
#include <nlohmann/json.hpp>
#include <imgui_internal.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <imgui_stdlib.h>
#include <ImGuiNotify.hpp>

namespace nlohmann
{

void to_json(json& j, const ImVec4& v)
{
    j = json{v.x, v.y, v.z, v.w};
}
void from_json(const json& j, ImVec4& v)
{
    std::array<float, 4> temp = j;
    v                         = {temp[0], temp[1], temp[2], temp[3]};
}

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(
    CatMenu::UI::Settings,
    toggle_key,
    font_path,
    font_size,
    glyph_chn_full,
    glyph_chs_common,
    glyph_cyr,
    glyph_greek,
    glyph_jap,
    glyph_kor,
    glyph_thai,
    glyph_viet,
    theme_colors)
} // namespace nlohmann

// copied from imgui_impl_dx11.cpp

struct ImGui_ImplDX11_Data
{
    ID3D11Device*             pd3dDevice;
    ID3D11DeviceContext*      pd3dDeviceContext;
    IDXGIFactory*             pFactory;
    ID3D11Buffer*             pVB;
    ID3D11Buffer*             pIB;
    ID3D11VertexShader*       pVertexShader;
    ID3D11InputLayout*        pInputLayout;
    ID3D11Buffer*             pVertexConstantBuffer;
    ID3D11PixelShader*        pPixelShader;
    ID3D11SamplerState*       pFontSampler;
    ID3D11ShaderResourceView* pFontTextureView;
    ID3D11RasterizerState*    pRasterizerState;
    ID3D11BlendState*         pBlendState;
    ID3D11DepthStencilState*  pDepthStencilState;
    int                       VertexBufferSize;
    int                       IndexBufferSize;

    ImGui_ImplDX11_Data()
    {
        memset((void*)this, 0, sizeof(*this));
        VertexBufferSize = 5000;
        IndexBufferSize  = 10000;
    }
};

static ImGui_ImplDX11_Data* ImGui_ImplDX11_GetBackendData()
{
    return ImGui::GetCurrentContext() ? (ImGui_ImplDX11_Data*)ImGui::GetIO().BackendRendererUserData : nullptr;
}

static void ImGui_ImplDX11_CreateFontsTexture()
{
    // Build texture atlas
    ImGuiIO&             io = ImGui::GetIO();
    ImGui_ImplDX11_Data* bd = ImGui_ImplDX11_GetBackendData();
    unsigned char*       pixels;
    int                  width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

    // Upload texture to graphics system
    {
        D3D11_TEXTURE2D_DESC desc;
        ZeroMemory(&desc, sizeof(desc));
        desc.Width            = width;
        desc.Height           = height;
        desc.MipLevels        = 1;
        desc.ArraySize        = 1;
        desc.Format           = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.Usage            = D3D11_USAGE_DEFAULT;
        desc.BindFlags        = D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags   = 0;

        ID3D11Texture2D*       pTexture = nullptr;
        D3D11_SUBRESOURCE_DATA subResource;
        subResource.pSysMem          = pixels;
        subResource.SysMemPitch      = desc.Width * 4;
        subResource.SysMemSlicePitch = 0;
        bd->pd3dDevice->CreateTexture2D(&desc, &subResource, &pTexture);
        IM_ASSERT(pTexture != nullptr);

        // Create texture view
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        ZeroMemory(&srvDesc, sizeof(srvDesc));
        srvDesc.Format                    = DXGI_FORMAT_R8G8B8A8_UNORM;
        srvDesc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels       = desc.MipLevels;
        srvDesc.Texture2D.MostDetailedMip = 0;
        bd->pd3dDevice->CreateShaderResourceView(pTexture, &srvDesc, &bd->pFontTextureView);
        pTexture->Release();
    }

    // Store our identifier
    io.Fonts->SetTexID((ImTextureID)bd->pFontTextureView);

    // Create texture sampler
    // (Bilinear sampling is required by default. Set 'io.Fonts->Flags |= ImFontAtlasFlags_NoBakedLines' or 'style.AntiAliasedLinesUseTex = false' to allow point/nearest sampling)
    {
        D3D11_SAMPLER_DESC desc;
        ZeroMemory(&desc, sizeof(desc));
        desc.Filter         = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        desc.AddressU       = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.AddressV       = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.AddressW       = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.MipLODBias     = 0.f;
        desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
        desc.MinLOD         = 0.f;
        desc.MaxLOD         = 0.f;
        bd->pd3dDevice->CreateSamplerState(&desc, &bd->pFontSampler);
    }
}

void ImGui_ImplDX11_ReCreateFontsTexture()
{
    ImGui_ImplDX11_Data* bd = ImGui_ImplDX11_GetBackendData();

    if (bd->pFontSampler) {
        bd->pFontSampler->Release();
        bd->pFontSampler = NULL;
    }
    if (bd->pFontTextureView) {
        bd->pFontTextureView->Release();
        bd->pFontTextureView = NULL;
        ImGui::GetIO().Fonts->SetTexID(NULL);
    }
    ImGui_ImplDX11_CreateFontsTexture();
}

namespace CatMenu
{

constexpr auto g_config_path = "Data\\SKSE\\Plugins\\catmenu\\settings.json"sv;

std::string KeyToString(ImGuiKey key)
{
    bool ctrl  = (int)key & (int)ImGuiMod_Ctrl;
    bool shift = (int)key & (int)ImGuiMod_Shift;
    bool alt   = (int)key & (int)ImGuiMod_Alt;
    key        = ImGuiKey{(int)key & ~(int)ImGuiMod_Mask_};
    return (ctrl ? "Ctrl + "s : ""s) + (shift ? "Shift + "s : ""s) + (alt ? "Alt + "s : ""s) + std::string(ImGui::GetKeyName(ImGuiKey{key}));
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

    logger::info("Overlay: {} registered.", name);
    ImGui::InsertNotification({ImGuiToastType::Success, 5000, "Overlay: %s registered.", name.data()});

    return APIResult::OK;
}

APIResult UI::RegisterMenuDrawFunc(std::string_view name, std::function<bool()> func)
{
    std::lock_guard list_lock{list_mutex};

    if (auto it = menu_list.find(name); it != menu_list.end()) {
        logger::warn("Trying to register menu: {}, which already exists. Aborted.", name);
        return APIResult::AlreadyRegistered;
    }

    std::string name_str{name};
    menu_list.insert({name_str, DrawFunc{name_str, func, false}});
    menu_order.push_back(name_str);
    logger::info("Menu: {} registered.", name);
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
    LoadSettings();

    auto& io = ImGui::GetIO();

    io.ConfigFlags  = ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_DockingEnable;
    io.BackendFlags = ImGuiBackendFlags_HasMouseCursors | ImGuiBackendFlags_RendererHasVtxOffset;

    ///////////////////////// POST
    auto ver = SKSE::PluginDeclaration::GetSingleton()->GetVersion();
    auto msg = std::format("CatMenu ({}.{}.{}) initialized.", ver.major(), ver.minor(), ver.patch());
    logger::info("{}", msg);
    ImGui::InsertNotification({ImGuiToastType::Success, 5000, msg.c_str()});
}


void UI::SetupTheme()
{
    auto& style  = ImGui::GetStyle();
    auto& colors = style.Colors;

    style.WindowRounding    = 5.3f;
    style.FrameRounding     = 2.3f;
    style.ScrollbarRounding = 0;
    style.FrameBorderSize   = 1;
    style.TabBarBorderSize  = 1;

    std::copy(settings.theme_colors.begin(), settings.theme_colors.end(), std::span(colors).begin());
}

void UI::LoadFonts()
{
    should_load_fonts = false;

    auto& io         = ImGui::GetIO();
    bool  is_rebuild = io.Fonts->IsBuilt();

    io.Fonts->Clear();

    // main font
    ImVector<ImWchar>        ranges;
    ImFontGlyphRangesBuilder builder;
    builder.AddRanges(io.Fonts->GetGlyphRangesDefault());
    if (settings.glyph_chn_full) builder.AddRanges(io.Fonts->GetGlyphRangesChineseFull());
    if (settings.glyph_chs_common) builder.AddRanges(io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
    if (settings.glyph_cyr) builder.AddRanges(io.Fonts->GetGlyphRangesCyrillic());
    if (settings.glyph_greek) builder.AddRanges(io.Fonts->GetGlyphRangesGreek());
    if (settings.glyph_jap) builder.AddRanges(io.Fonts->GetGlyphRangesJapanese());
    if (settings.glyph_kor) builder.AddRanges(io.Fonts->GetGlyphRangesKorean());
    if (settings.glyph_thai) builder.AddRanges(io.Fonts->GetGlyphRangesThai());
    if (settings.glyph_viet) builder.AddRanges(io.Fonts->GetGlyphRangesVietnamese());
    builder.BuildRanges(&ranges);

    main_font = io.Fonts->AddFontFromFileTTF(settings.font_path.c_str(), settings.font_size, NULL, ranges.Data);
    if (!main_font) {
        auto msg = std::format("Failed to load main font at {}\nUsing backup font.", settings.font_path);
        logger::error("{}", msg);
        ImGui::InsertNotification({ImGuiToastType::Error, 5000, msg.c_str()});

        io.Fonts->AddFontDefault();

        return;
    }

    // add font awesome 6
    static const ImWchar icons_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
    ImFontConfig         icons_config;
    icons_config.MergeMode            = true;
    icons_config.PixelSnapH           = true;
    icons_config.FontDataOwnedByAtlas = true;

    if (!io.Fonts->AddFontFromFileTTF("Data\\SKSE\\Plugins\\catmenu\\fonts\\fa-solid-900.ttf", settings.font_size, &icons_config, icons_ranges)) {
        auto msg = std::format("Failed to load icon font at {}\nPlease verify the intergrity of mod files.", settings.font_path);
        logger::error("{}", msg);
        ImGui::InsertNotification({ImGuiToastType::Error, 5000, msg.c_str()});
    }

    io.Fonts->Build();

    if (is_rebuild)
        ImGui_ImplDX11_ReCreateFontsTexture();

    logger::info("Font {} built.", settings.font_path);
}

void UI::Draw()
{
    if (should_load_fonts)
        LoadFonts();

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    if (main_font)
        ImGui::PushFont(main_font);

    {
        std::lock_guard list_lock{list_mutex};

        for (std::string_view name : overlay_order) {
            ImGui::PushID(name.data());
            overlay_list.at(name).func();
            ImGui::PopID();
        }

        if (ImGui::IsKeyPressed(ImGuiKey{settings.toggle_key}, false))
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

                    if (ImGui::MenuItem("Close Windows")) {
                        show_config = false;
                        for (auto& [_, menu_func] : menu_list)
                            menu_func.enabled = false;
                    }
                    if (ImGui::IsItemHovered())
                        ImGui::SetTooltip("Close all windows.");

                    if (ImGui::MenuItem("Close"))
                        Toggle(false);
                    if (ImGui::IsItemHovered())
                        ImGui::SetTooltip("Close catmenu and resume game.");

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Settings")) {
                    ImGui::MenuItem("General", nullptr, &show_config);
                    ImGui::MenuItem("Theme Editor", nullptr, &show_theme_editor);

                    ImGui::Separator();

                    if (ImGui::MenuItem("Save Settings"))
                        SaveSettings();
                    if (ImGui::MenuItem("Load Settings"))
                        LoadSettings();

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("About")) {
                    if (ImGui::MenuItem("Info")) {
                        auto ver = SKSE::PluginDeclaration::GetSingleton()->GetVersion();
                        auto msg = std::format(
                            "CatMenu version {}.{}.{}.\n"
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
            if (show_theme_editor)
                DrawThemeEditor();

            // registered menus
            for (std::string_view name : menu_order)
                if (auto& menu_func = menu_list.at(name); menu_func.enabled) {
                    ImGui::PushID(name.data());
                    menu_func.enabled = menu_func.func();
                    ImGui::PopID();
                }
        }

        ImGui::RenderNotifications(); // <-- Here we render all notifications
    }

    if (main_font)
        ImGui::PopFont();

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void UI::DrawConfigWindow()
{
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos({viewport->WorkSize.x * 0.1f, viewport->WorkSize.y * 0.2f}, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize({viewport->WorkSize.x * 0.3f, viewport->WorkSize.y * 0.5f}, ImGuiCond_FirstUseEver);

    if (!ImGui::Begin("[CatMenu] Configuration", &show_config)) {
        ImGui::End();
        return;
    }

    // toggle key
    auto key_name = KeyToString(ImGuiKey{settings.toggle_key});
    ImGui::InputTextWithHint("Toggle Button", "Click to set keyboard shortcut", &key_name,
                             ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_NoUndoRedo | ImGuiInputTextFlags_NoHorizontalScroll);
    if (ImGui::IsItemActive()) {
        const auto last_key_pressed = GetLastKeyPressed();
        if (last_key_pressed != ImGuiKey_None) {
            settings.toggle_key = last_key_pressed;
            ImGui::ClearActiveID();
        }
    }
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Click in the field and press any key to change the shortcut.");

    // fonts
    ImGui::SeparatorText("Font");

    if (ImGui::Button("Reload Font", ImVec2(-FLT_MIN, 0)))
        should_load_fonts = true;

    ImGui::InputTextWithHint("Font Path", "path to a ttf/otf font e.g. \"C:/Windows/Fonts/Arial.ttf\"", &settings.font_path);

    ImGui::SliderFloat("Font Size", &settings.font_size, 8.0f, 40.0f, "%.1f");
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Control how big the font is.\nOnly applies to custom fonts.");

    if (ImGui::TreeNodeEx("Extra Glyphs", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImGui::BeginTable("Extra Glyphs Table", 2, ImGuiTableFlags_Hideable)) {
            ImGui::TableNextColumn();
            ImGui::Checkbox("Chinese-Full", &settings.glyph_chn_full);
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("全套漢字");
            ImGui::TableNextColumn();
            ImGui::Checkbox("Chinese-Common Simplified", &settings.glyph_chs_common);
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("常用简体汉字");

            ImGui::TableNextColumn();
            ImGui::Checkbox("Cyrillic", &settings.glyph_cyr);
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Кириллица/Кирилиця/Кірыліца");
            ImGui::TableNextColumn();
            ImGui::Checkbox("Greek", &settings.glyph_greek);
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Ελληνικό αλφάβητο");

            ImGui::TableNextColumn();
            ImGui::Checkbox("Japanese", &settings.glyph_jap);
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("日本語の仮名と漢字");
            ImGui::TableNextColumn();
            ImGui::Checkbox("Korean", &settings.glyph_kor);
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("한글");

            ImGui::TableNextColumn();
            ImGui::Checkbox("Thai", &settings.glyph_thai);
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("อักษรไทย");
            ImGui::TableNextColumn();
            ImGui::Checkbox("Vietnamese", &settings.glyph_viet);
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("chữ Quốc ngữ");


            ImGui::EndTable();
        }
        ImGui::TreePop();
    }

    ImGui::End();
}

void UI::DrawThemeEditor()
{
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos({viewport->WorkSize.x * 0.5f, viewport->WorkSize.y * 0.2f}, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize({viewport->WorkSize.x * 0.3f, viewport->WorkSize.y * 0.5f}, ImGuiCond_FirstUseEver);

    if (!ImGui::Begin("[CatMenu] Theme Editor", &show_theme_editor)) {
        ImGui::End();
        return;
    }

    ImGui::TextWrapped("Note: Some mods may want to use their own themes, which cannot be edited here.");

    ImGui::SeparatorText("Colours");

    auto& style  = ImGui::GetStyle();
    auto& colors = style.Colors;
    for (int i = 0; i < ImGuiCol_COUNT; ++i) {
        auto color_name = magic_enum::enum_name((ImGuiCol_)i);
        if (ImGui::ColorEdit4(color_name.data() + 9, &settings.theme_colors[i].x))
            colors[i] = settings.theme_colors[i];
    }

    ImGui::End();
}

void UI::SaveSettings()
{
    std::ofstream o(g_config_path.data());
    if (!o.is_open()) {
        auto msg = std::format("Unable to save config file to {}", g_config_path);
        logger::error("{}", msg);
        ImGui::InsertNotification({ImGuiToastType::Error, 5000, msg.c_str()});
        return;
    }

    nlohmann::json settings_json = settings;
    o << settings_json.dump(4);

    // post
    auto msg = std::format("Successfully saved config file to {}", g_config_path);
    logger::info("{}", msg);
    ImGui::InsertNotification({ImGuiToastType::Success, 5000, msg.c_str()});
}

void UI::LoadSettings()
{
    std::ifstream i(g_config_path.data());
    if (!i.is_open()) {
        auto msg = std::format("Unable to load config file at {}\nAll settings are reset to default.", g_config_path);
        logger::error("{}", msg);
        ImGui::InsertNotification({ImGuiToastType::Error, 5000, msg.c_str()});
        return;
    }

    nlohmann::json settings_json;
    try {
        i >> settings_json;
    } catch (const nlohmann::json::parse_error& e) {
        auto msg = std::format("Error parsing config file at {}\nError: {}\nAll settings are reset to default.", g_config_path, e.what());
        logger::error("{}", msg);
        ImGui::InsertNotification({ImGuiToastType::Error, 5000, msg.c_str()});
        return;
    }

    settings = settings_json;

    // post
    auto msg = std::format("Successfully loaded config file at {}", g_config_path);
    logger::info("{}", msg);
    ImGui::InsertNotification({ImGuiToastType::Success, 5000, msg.c_str()});

    should_load_fonts = true;
    SetupTheme();
}
} // namespace CatMenu