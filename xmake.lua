-- set minimum xmake version
set_xmakever("2.8.2")

-- includes
includes("lib/commonlibsse-ng")

-- set project
set_project("catmenu")
set_version("2.0.0")
set_license("MIT")

-- set defaults
set_languages("c++23")
set_warnings("allextra", "error")
set_defaultmode("releasedbg")

-- add rules
add_rules("mode.debug", "mode.releasedbg")
add_rules("plugin.vsxmake.autoupdate")

-- set policies
set_policy("package.requires_lock", true)

-- set runtimes
set_runtimes("MD")

-- set encodings
set_encodings("utf-8") -- msvc: /utf-8
set_encodings("source:utf-8", "target:utf-8")

-- set configs
set_config("skyrim_se", true)
set_config("skyrim_ae", true)
set_config("skyrim_vr", false) -- no xmake support it seems

-- set requires
add_requires("spdlog", { configs = { header_only = false, wchar = true, std_format = true } })
add_requires("unordered_dense")
add_requires("nlohmann_json")
add_requires("magic_enum")

target("imgui")
    set_kind("shared")
    
    -- add src files
    add_files("lib/imgui/*.cpp")
    add_headerfiles("lib/imgui/*.h")
    add_includedirs("lib/imgui/", { public = true })
    
    add_files(
        "lib/imgui/backends/imgui_impl_dx11.cpp",
        "lib/imgui/backends/imgui_impl_win32.cpp")
    add_includedirs("lib/imgui/backends/", { public = true })

    add_files("lib/imgui/misc/cpp/imgui_stdlib.cpp")
    add_includedirs("lib/imgui/misc/cpp/", { public = true })

    add_defines("IMGUI_API=__declspec(dllexport)") -- export symbols


-- targets
target("catmenu")
    set_kind("shared")

    add_defines("UNICODE")
    add_cxxflags(
        "cl::/wd4200", -- zero-sized array in struct/union
        "cl::/wd4201"  -- nameless struct/union
    )

    -- add dependencies to target
    add_deps("commonlibsse-ng")
    add_deps("imgui")
    add_defines("IMGUI_API=__declspec(dllimport)") -- import symbols

    add_packages("spdlog", "unordered_dense", "nlohmann_json", "magic_enum")

    add_includedirs("lib")
    add_headerfiles("lib/detours/Detours.h")
    add_links("lib/detours/Release/detours.lib")

    -- add commonlibsse-ng plugin
    add_rules("commonlibsse-ng.plugin", {
        name = "CatMenu",
        author = "FiveLimbedCat",
        description = "Unified imgui backend for skyrim."
    })

    -- add src files
    add_files("src/**.cpp")
    add_headerfiles("src/**.h")
    add_includedirs("src")
    set_pcxxheader("src/pch.h")
