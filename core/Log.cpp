#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include "Log.h"


namespace sprint {

std::unique_ptr<spdlog::logger> Log::core_logger_;
std::unique_ptr<spdlog::logger> Log::client_logger_;

void Log::Init() {
    auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("log", true);

    stdout_sink->set_pattern("%^[%T] %n: %v%$");
    file_sink->set_pattern("[%T] [%l] %n: %v");

    std::vector<spdlog::sink_ptr> sinks {stdout_sink, file_sink};

    core_logger_ = std::make_unique<spdlog::logger>("ENGINE", begin(sinks), end(sinks));
    core_logger_->set_level(spdlog::level::trace);
    core_logger_->flush_on(spdlog::level::trace);

    client_logger_ = std::make_unique<spdlog::logger>("APP", begin(sinks), end(sinks));
    client_logger_->set_level(spdlog::level::trace);
    client_logger_->flush_on(spdlog::level::trace);
}

}