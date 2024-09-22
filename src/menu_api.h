// Copy this file and put alongside imgui headers.

#pragma once

#include <functional>

#include <imgui.h>
#include <ImGuiNotify.hpp>


namespace CatMenu
{

enum class APIResult : uint8_t
{
    OK,
    AlreadyRegistered,
    NotRegistered,
};

class APIBase
{
public:
    virtual REL::Version GetVersion() = 0;

    virtual ImGuiContext* GetContext()                                                      = 0;
    virtual APIResult     RegisterOverlayDrawFunc(const RE::BSString& name, bool (*func)()) = 0;
    virtual APIResult     RegisterMenuDrawFunc(const RE::BSString& name, bool (*func)())    = 0;
    virtual void          InsertNotification(const ImGuiToast& toast)                       = 0;
};

extern "C" __declspec(dllexport) APIBase* GetAPI();


} // namespace CatMenu