#pragma once

#include <Windows.h>

namespace CatMenu
{

struct WndProcHook
{
    static LRESULT        thunk(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static inline WNDPROC func;
};

struct D3DInitHook
{
    static void                                    thunk();
    static inline REL::Relocation<decltype(thunk)> func;

    static inline void install()
    {
        stl::write_thunk_call<D3DInitHook>(REL::RelocationID(75595, 77226).address() + REL::Relocate(0x50, 0x2BC));
    }

    static inline std::atomic<bool> initialized = false;
};
} // namespace CatMenu