#include "render.h"

#include "ui.h"

#include <Detours/Detours.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

namespace ImGui
{
namespace Skyrim
{

LRESULT WndProcHook::thunk(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    auto& io = ImGui::GetIO();
    if (uMsg == WM_KILLFOCUS) {
        io.ClearInputCharacters();
        io.ClearInputKeys();
    }

    return func(hWnd, uMsg, wParam, lParam);
}


decltype(&IDXGISwapChain::Present) ptr_IDXGISwapChain_Present;
HRESULT WINAPI                     hk_IDXGISwapChain_Present(IDXGISwapChain* This, UINT SyncInterval, UINT Flags)
{
    UI::GetSingleton()->Draw();
    return (This->*ptr_IDXGISwapChain_Present)(SyncInterval, Flags);
}

void D3DInitHook::thunk()
{
    func();
    logger::info("D3DInit Hooked!");

    auto manager = RE::BSGraphics::Renderer::GetSingleton();

    auto context   = reinterpret_cast<ID3D11DeviceContext*>(manager->GetRuntimeData().context);
    auto swapchain = reinterpret_cast<IDXGISwapChain*>(manager->GetRuntimeData().renderWindows->swapChain);
    auto device    = reinterpret_cast<ID3D11Device*>(manager->GetRuntimeData().forwarder);

    UI::GetSingleton()->Init(swapchain, device, context);

    logger::info("Hooking swapchain present...");
    *(uintptr_t*)&ptr_IDXGISwapChain_Present = Detours::X64::DetourClassVTable(*(uintptr_t*)swapchain, &hk_IDXGISwapChain_Present, 8);

    logger::info("Hooking WndProc...");
    DXGI_SWAP_CHAIN_DESC desc;
    swapchain->GetDesc(&desc);
    WndProcHook::func = reinterpret_cast<WNDPROC>(SetWindowLongPtrA(desc.OutputWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProcHook::thunk)));
}

} // namespace Skyrim
} // namespace ImGui