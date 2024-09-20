#include "input.h"

#include <Windows.h>
#include <WinUser.h>
#include <dinput.h>

#include <imgui.h>

const ImGuiKey VirtualKeyToImGuiKey(WPARAM vkKey)
{
    switch (vkKey)
    {
        case VK_TAB:
            return ImGuiKey_Tab;
        case VK_LEFT:
            return ImGuiKey_LeftArrow;
        case VK_RIGHT:
            return ImGuiKey_RightArrow;
        case VK_UP:
            return ImGuiKey_UpArrow;
        case VK_DOWN:
            return ImGuiKey_DownArrow;
        case VK_PRIOR:
            return ImGuiKey_PageUp;
        case VK_NEXT:
            return ImGuiKey_PageDown;
        case VK_HOME:
            return ImGuiKey_Home;
        case VK_END:
            return ImGuiKey_End;
        case VK_INSERT:
            return ImGuiKey_Insert;
        case VK_DELETE:
            return ImGuiKey_Delete;
        case VK_BACK:
            return ImGuiKey_Backspace;
        case VK_SPACE:
            return ImGuiKey_Space;
        case VK_RETURN:
            return ImGuiKey_Enter;
        case VK_ESCAPE:
            return ImGuiKey_Escape;
        case VK_OEM_7:
            return ImGuiKey_Apostrophe;
        case VK_OEM_COMMA:
            return ImGuiKey_Comma;
        case VK_OEM_MINUS:
            return ImGuiKey_Minus;
        case VK_OEM_PERIOD:
            return ImGuiKey_Period;
        case VK_OEM_2:
            return ImGuiKey_Slash;
        case VK_OEM_1:
            return ImGuiKey_Semicolon;
        case VK_OEM_PLUS:
            return ImGuiKey_Equal;
        case VK_OEM_4:
            return ImGuiKey_LeftBracket;
        case VK_OEM_5:
            return ImGuiKey_Backslash;
        case VK_OEM_6:
            return ImGuiKey_RightBracket;
        case VK_OEM_3:
            return ImGuiKey_GraveAccent;
        case VK_CAPITAL:
            return ImGuiKey_CapsLock;
        case VK_SCROLL:
            return ImGuiKey_ScrollLock;
        case VK_NUMLOCK:
            return ImGuiKey_NumLock;
        case VK_SNAPSHOT:
            return ImGuiKey_PrintScreen;
        case VK_PAUSE:
            return ImGuiKey_Pause;
        case VK_NUMPAD0:
            return ImGuiKey_Keypad0;
        case VK_NUMPAD1:
            return ImGuiKey_Keypad1;
        case VK_NUMPAD2:
            return ImGuiKey_Keypad2;
        case VK_NUMPAD3:
            return ImGuiKey_Keypad3;
        case VK_NUMPAD4:
            return ImGuiKey_Keypad4;
        case VK_NUMPAD5:
            return ImGuiKey_Keypad5;
        case VK_NUMPAD6:
            return ImGuiKey_Keypad6;
        case VK_NUMPAD7:
            return ImGuiKey_Keypad7;
        case VK_NUMPAD8:
            return ImGuiKey_Keypad8;
        case VK_NUMPAD9:
            return ImGuiKey_Keypad9;
        case VK_DECIMAL:
            return ImGuiKey_KeypadDecimal;
        case VK_DIVIDE:
            return ImGuiKey_KeypadDivide;
        case VK_MULTIPLY:
            return ImGuiKey_KeypadMultiply;
        case VK_SUBTRACT:
            return ImGuiKey_KeypadSubtract;
        case VK_ADD:
            return ImGuiKey_KeypadAdd;
        case (VK_RETURN + 256):
            return ImGuiKey_KeypadEnter;
        case VK_LSHIFT:
            return ImGuiKey_LeftShift;
        case VK_LCONTROL:
            return ImGuiKey_LeftCtrl;
        case VK_LMENU:
            return ImGuiKey_LeftAlt;
        case VK_LWIN:
            return ImGuiKey_LeftSuper;
        case VK_RSHIFT:
            return ImGuiKey_RightShift;
        case VK_RCONTROL:
            return ImGuiKey_RightCtrl;
        case VK_RMENU:
            return ImGuiKey_RightAlt;
        case VK_RWIN:
            return ImGuiKey_RightSuper;
        case VK_APPS:
            return ImGuiKey_Menu;
        case '0':
            return ImGuiKey_0;
        case '1':
            return ImGuiKey_1;
        case '2':
            return ImGuiKey_2;
        case '3':
            return ImGuiKey_3;
        case '4':
            return ImGuiKey_4;
        case '5':
            return ImGuiKey_5;
        case '6':
            return ImGuiKey_6;
        case '7':
            return ImGuiKey_7;
        case '8':
            return ImGuiKey_8;
        case '9':
            return ImGuiKey_9;
        case 'A':
            return ImGuiKey_A;
        case 'B':
            return ImGuiKey_B;
        case 'C':
            return ImGuiKey_C;
        case 'D':
            return ImGuiKey_D;
        case 'E':
            return ImGuiKey_E;
        case 'F':
            return ImGuiKey_F;
        case 'G':
            return ImGuiKey_G;
        case 'H':
            return ImGuiKey_H;
        case 'I':
            return ImGuiKey_I;
        case 'J':
            return ImGuiKey_J;
        case 'K':
            return ImGuiKey_K;
        case 'L':
            return ImGuiKey_L;
        case 'M':
            return ImGuiKey_M;
        case 'N':
            return ImGuiKey_N;
        case 'O':
            return ImGuiKey_O;
        case 'P':
            return ImGuiKey_P;
        case 'Q':
            return ImGuiKey_Q;
        case 'R':
            return ImGuiKey_R;
        case 'S':
            return ImGuiKey_S;
        case 'T':
            return ImGuiKey_T;
        case 'U':
            return ImGuiKey_U;
        case 'V':
            return ImGuiKey_V;
        case 'W':
            return ImGuiKey_W;
        case 'X':
            return ImGuiKey_X;
        case 'Y':
            return ImGuiKey_Y;
        case 'Z':
            return ImGuiKey_Z;
        case VK_F1:
            return ImGuiKey_F1;
        case VK_F2:
            return ImGuiKey_F2;
        case VK_F3:
            return ImGuiKey_F3;
        case VK_F4:
            return ImGuiKey_F4;
        case VK_F5:
            return ImGuiKey_F5;
        case VK_F6:
            return ImGuiKey_F6;
        case VK_F7:
            return ImGuiKey_F7;
        case VK_F8:
            return ImGuiKey_F8;
        case VK_F9:
            return ImGuiKey_F9;
        case VK_F10:
            return ImGuiKey_F10;
        case VK_F11:
            return ImGuiKey_F11;
        case VK_F12:
            return ImGuiKey_F12;
        default:
            return ImGuiKey_None;
    };
}

const uint32_t DIKToVK(uint32_t DIK)
{
    switch (DIK)
    {
        case DIK_LEFTARROW:
            return VK_LEFT;
        case DIK_RIGHTARROW:
            return VK_RIGHT;
        case DIK_UPARROW:
            return VK_UP;
        case DIK_DOWNARROW:
            return VK_DOWN;
        case DIK_DELETE:
            return VK_DELETE;
        case DIK_END:
            return VK_END;
        case DIK_HOME:
            return VK_HOME; // pos1
        case DIK_PRIOR:
            return VK_PRIOR; // page up
        case DIK_NEXT:
            return VK_NEXT; // page down
        case DIK_INSERT:
            return VK_INSERT;
        case DIK_NUMPAD0:
            return VK_NUMPAD0;
        case DIK_NUMPAD1:
            return VK_NUMPAD1;
        case DIK_NUMPAD2:
            return VK_NUMPAD2;
        case DIK_NUMPAD3:
            return VK_NUMPAD3;
        case DIK_NUMPAD4:
            return VK_NUMPAD4;
        case DIK_NUMPAD5:
            return VK_NUMPAD5;
        case DIK_NUMPAD6:
            return VK_NUMPAD6;
        case DIK_NUMPAD7:
            return VK_NUMPAD7;
        case DIK_NUMPAD8:
            return VK_NUMPAD8;
        case DIK_NUMPAD9:
            return VK_NUMPAD9;
        case DIK_DECIMAL:
            return VK_DECIMAL;
        case DIK_NUMPADENTER:
            return (VK_RETURN + 256);
        case DIK_RMENU:
            return VK_RMENU; // right alt
        case DIK_RCONTROL:
            return VK_RCONTROL; // right control
        case DIK_LWIN:
            return VK_LWIN; // left win
        case DIK_RWIN:
            return VK_RWIN; // right win
        case DIK_APPS:
            return VK_APPS;
        default:
            return DIK;
    }
}

RE::BSEventNotifyControl ImGui::Skyrim::InputListener::ProcessEvent(RE::InputEvent* const* a_event, RE::BSTEventSource<RE::InputEvent*>* a_eventSource)
{
    if (!a_event || !a_eventSource)
        return RE::BSEventNotifyControl::kContinue;

    ImGuiIO& io = ImGui::GetIO();

    for (auto event = *a_event; event; event = event->next)
    {
        if (event->eventType == RE::INPUT_EVENT_TYPE::kChar)
        {
            io.AddInputCharacter(event->AsCharEvent()->keyCode);
            continue;
        }
        else if (event->eventType == RE::INPUT_EVENT_TYPE::kButton)
        {
            const auto button = static_cast<RE::ButtonEvent*>(event);
            if (!button || (button->IsPressed() && !button->IsDown()))
                continue;

            auto keyCode = button->GetIDCode();

            switch (button->device.get())
            {
                case RE::INPUT_DEVICE::kMouse:
                    if (keyCode > 7) // middle scroll
                        io.AddMouseWheelEvent(0, button->Value() * (keyCode == 8 ? 1 : -1));
                    else
                    {
                        if (keyCode > 5)
                            keyCode = 5;
                        io.AddMouseButtonEvent(keyCode, button->IsPressed());
                    }
                    break;

                case RE::INPUT_DEVICE::kKeyboard:
                    uint32_t key = DIKToVK(keyCode);
                    if (key == keyCode)
                        key = MapVirtualKeyEx(keyCode, MAPVK_VSC_TO_VK_EX, GetKeyboardLayout(0));
                    io.AddKeyEvent(VirtualKeyToImGuiKey(key), button->IsPressed());

                    if (key == VK_LCONTROL || key == VK_RCONTROL)
                        io.AddKeyEvent(ImGuiMod_Ctrl, button->IsPressed());
                    else if (key == VK_LSHIFT || key == VK_RSHIFT)
                        io.AddKeyEvent(ImGuiMod_Shift, button->IsPressed());
                    else if (key == VK_LMENU || key == VK_RMENU)
                        io.AddKeyEvent(ImGuiMod_Alt, button->IsPressed());
                    break;

                case RE::INPUT_DEVICE::kGamepad:
                    // not implemented yet
                    // key = GetGamepadIndex((RE::BSWin32GamepadDevice::Key)key);
                    break;

                default:
                    continue;
            }
        }
    }

    return RE::BSEventNotifyControl::kContinue;
}