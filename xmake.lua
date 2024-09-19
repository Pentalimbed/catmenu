-- set minimum xmake version
set_xmakever("2.8.2")

-- includes
includes("lib/commonlibsse-ng")

-- set project
set_project("imgui-skyrim")
set_version("1.0.0")
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

-- set configs
set_config("skyrim_vr", false)

-- set requires
add_requires("spdlog", { configs = { header_only = false, wchar = true, std_format = true } })

target("imgui")
    set_kind("shared")
    add_rules("utils.symbols.export_all")
    
    -- add src files
    add_files("lib/imgui/*.cpp")
    add_headerfiles("lib/imgui/*.h")
    add_includedirs("lib/imgui/", { public = true })
    
    add_files(
        "lib/imgui/backends/imgui_impl_dx11.cpp",
        "lib/imgui/backends/imgui_impl_win32.cpp")
    add_includedirs("lib/imgui/backends/")


-- targets
target("imgui-skyrim")
    set_kind("shared")

    add_cxxflags(
        "cl::/wd4200", -- zero-sized array in struct/union
        "cl::/wd4201"  -- nameless struct/union
    )

    -- add dependencies to target
    add_deps("commonlibsse-ng")
    add_deps("imgui")

    -- add commonlibsse-ng plugin
    add_rules("commonlibsse-ng.plugin", {
        name = "imgui-skyrim",
        author = "FiveLimbedCat",
        description = "Unified imgui backend for skyrim."
    })

    -- add src files
    add_files("src/**.cpp")
    add_headerfiles("src/**.h")
    add_includedirs("src")
    set_pcxxheader("src/pch.h")
