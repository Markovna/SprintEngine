#pragma once

#include "spdlog/spdlog.h"


namespace sprint {

class Log {
public:
    template<typename ...Args>
    inline static void Info(const std::string &str, const Args& ...args) {
        client_logger_->info(str, args...);
    }

    template<typename ...Args>
    inline static void Warning(const std::string &str, const Args& ...args) {
        client_logger_->warn(str, args...);
    }

    template<typename ...Args>
    inline static void Error(const std::string &str, const Args& ...args) {
        client_logger_->error(str, args...);
    }

    template<typename ...Args>
    inline static void CoreInfo(const std::string &str, const Args& ...args) {
        core_logger_->info(str, args...);
    }

    template<typename ...Args>
    inline static void CoreWarning(const std::string &str, const Args& ...args) {
        core_logger_->warn(str, args...);
    }

    template<typename ...Args>
    inline static void CoreError(const std::string &str, const Args& ...args) {
        core_logger_->error(str, args...);
    }

    static void Init();

private:
    static std::unique_ptr<spdlog::logger> core_logger_;
    static std::unique_ptr<spdlog::logger> client_logger_;
};


}