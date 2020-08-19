#pragma once

#include "spdlog/spdlog.h"

class Log {
public:
    template<typename ...Args>
    inline static void Info(const std::string &str, const Args& ...args) {
        s_ClientLogger->info(str, args...);
    }

    template<typename ...Args>
    inline static void Warning(const std::string &str, const Args& ...args) {
        s_ClientLogger->warn(str, args...);
    }

    template<typename ...Args>
    inline static void Error(const std::string &str, const Args& ...args) {
        s_ClientLogger->error(str, args...);
    }

    template<typename ...Args>
    inline static void CoreInfo(const std::string &str, const Args& ...args) {
        s_CoreLogger->info(str, args...);
    }

    template<typename ...Args>
    inline static void CoreWarning(const std::string &str, const Args& ...args) {
        s_CoreLogger->warn(str, args...);
    }

    template<typename ...Args>
    inline static void CoreError(const std::string &str, const Args& ...args) {
        s_CoreLogger->error(str, args...);
    }

    static void Init();

private:
    static std::unique_ptr<spdlog::logger> s_CoreLogger;
    static std::unique_ptr<spdlog::logger> s_ClientLogger;
};


