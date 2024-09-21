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
        case SKSE::MessagingInterface::kDataLoaded:
            logger::info("Game: data loaded.");
            stl::write_thunk_call<ImGui::Skyrim::hk_PollInputDevices>(REL::RelocationID(67315, 68617).address() + REL::Relocate(0x7B, 0x7B, 0x81));
            logger::info("hk_PollInputDevices hooked.");
            ImGui::Skyrim::UI::GetSingleton()->RegisterMenuDrawFunc("Demo Window", []() { ImGui::ShowDemoWindow(); return true; });
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

    ImGui::Skyrim::D3DInitHook::install();

    auto messaging = SKSE::GetMessagingInterface();
    if (!messaging->RegisterListener("SKSE", ProcessMessage))
        return false;

    logger::info("{} loaded.", plugin->GetName());

    return true;
}
