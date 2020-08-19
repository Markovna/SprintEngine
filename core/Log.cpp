#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include "Log.h"

std::unique_ptr<spdlog::logger> Log::s_CoreLogger;
std::unique_ptr<spdlog::logger> Log::s_ClientLogger;

void Log::Init() {
    auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("log", true);

    stdout_sink->set_pattern("%^[%T] %n: %v%$");
    file_sink->set_pattern("[%T] [%l] %n: %v");

    std::vector<spdlog::sink_ptr> sinks {stdout_sink, file_sink};

    s_CoreLogger = std::make_unique<spdlog::logger>("ENGINE", begin(sinks), end(sinks));
    s_CoreLogger->set_level(spdlog::level::trace);
    s_CoreLogger->flush_on(spdlog::level::trace);

    s_ClientLogger = std::make_unique<spdlog::logger>("APP", begin(sinks), end(sinks));
    s_ClientLogger->set_level(spdlog::level::trace);
    s_ClientLogger->flush_on(spdlog::level::trace);
}