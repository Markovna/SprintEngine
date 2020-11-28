#pragma once

#include "spdlog/spdlog.h"
#include <iostream>


namespace sprint {

namespace log {

namespace details {

spdlog::logger CreateLogger(std::string name);

template<class Logger>
struct LogBase {
public:
    template<typename ...Args>
    static void Info(const std::string &str, const Args &...args) {
        Logger::Get()->info(str, args...);
    }

    template<typename... Args>
    static void Warning(const std::string &str, const Args &... args) {
        Logger::Get()->warn(str, args...);
    }

    template<typename... Args>
    static void Error(const std::string &str, const Args &... args) {
        Logger::Get()->error(str, args...);
    }
};

struct CoreLog : LogBase<CoreLog> {
    static spdlog::logger *Get() {
        static auto logger = std::make_unique<spdlog::logger>(CreateLogger("ENGINE"));
        return logger.get();
    }
};

struct ClientLog : details::LogBase<ClientLog> {
    static spdlog::logger *Get() {
        static auto logger = std::make_unique<spdlog::logger>(CreateLogger("APP"));
        return logger.get();
    }
};

}

using core = details::CoreLog;
using app = details::ClientLog;

}

}