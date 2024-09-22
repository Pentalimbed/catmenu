#include "menu_api.h"

namespace CatMenu
{

class API : public APIBase
{
public:
    static API* GetSingleton()
    {
        static API api;
        return std::addressof(api);
    }

    virtual REL::Version GetVersion() override;

    virtual ImGuiContext* GetContext() override;
    virtual APIResult     RegisterOverlayDrawFunc(const RE::BSString& name, bool (*func)()) override;
    virtual APIResult     RegisterMenuDrawFunc(const RE::BSString& name, bool (*func)()) override;
    virtual void          InsertNotification(const ImGuiToast& toast) override;
};

} // namespace CatMenu