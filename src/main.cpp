#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/msvc_sink.h>

#include "ui.h"
#include "render.h"
#include "input.h"

void InitLogging()
{
    auto path = logger::log_directory();
    if (!path)
        return;

    const auto plugin = SKSE::PluginDeclaration::GetSingleton();
    *path /= std::format("{}.log", plugin->GetName());

    std::vector<spdlog::sink_ptr> sinks{
        std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true),
        std::make_shared<spdlog::sinks::msvc_sink_mt>()};

    auto logger = std::make_shared<spdlog::logger>("global", sinks.begin(), sinks.end());
    logger->set_level(spdlog::level::info);
    logger->flush_on(spdlog::level::info);

    spdlog::set_default_logger(std::move(logger));
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S] [%l] [%t] [%s:%#] %v");
}

void ProcessMessage(SKSE::MessagingInterface::Message* a_msg)
{
    switch (a_msg->type) {
        case SKSE::MessagingInterface::kPostPostLoad:
            logger::info("Game: data loaded.");
            stl::write_thunk_call<CatMenu::BSInputDeviceManager_PollInputDevices>(REL::VariantID(67315, 68617, 0xC519E0).address() + REL::Relocate(0x7B, 0x7B, 0x81));
            logger::info("BSInputDeviceManager_PollInputDevices hooked.");
            break;
        default:
            break;
    }
}

SKSEPluginLoad(const SKSE::LoadInterface* a_skse)
{
    InitLogging();

    const auto plugin = SKSE::PluginDeclaration::GetSingleton();
    logger::info("{} v{} is loading...", plugin->GetName(), plugin->GetVersion());

    SKSE::Init(a_skse);

    CatMenu::D3DInitHook::install();

    auto messaging = SKSE::GetMessagingInterface();
    if (!messaging->RegisterListener("SKSE", ProcessMessage))
        return false;

    logger::info("{} loaded.", plugin->GetName());

    return true;
}
