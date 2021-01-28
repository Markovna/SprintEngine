#pragma once

#include <chrono>
#include <stack>
#include <vector>
#include <thread>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <sparse_map.h>
#include "log.h"
#include "macro.h"

namespace sprint {

#define SPRINT_PROFILE 0

namespace profiler {

class Profiler {
private:
    struct ScopeInfo {
        const char *name = nullptr;
        std::chrono::steady_clock::time_point start;
    };

    struct ThreadContext {
        std::vector<ScopeInfo> scopes;
        std::thread::id id;
    };

public:
    Profiler() = default;
    ~Profiler() {
        std::ofstream file_stream;
        file_stream.open("results.json");
        assert(file_stream.is_open() && "Couldn't open profiler results file.");
        std::lock_guard lock(mutex_);
        file_stream << R"({"otherData": {},"traceEvents":[{})" << out_stream_.str() << "]}";
    }

    void BeginScope(const char *name) {
        GetThreadContext().scopes.push_back({name, std::chrono::steady_clock::now()});
    }

    void EndScope() {
        auto& ctx = GetThreadContext();
        auto& scope = ctx.scopes.back();
        Write(ctx.id, scope.name, scope.start, std::chrono::steady_clock::now() - scope.start);
        ctx.scopes.pop_back();
    }
private:
    ThreadContext& GetThreadContext() {
        thread_local ThreadContext* ctx = [&](){
            ThreadContext& new_ctx = contexts_.emplace_back();
            new_ctx.id = std::this_thread::get_id();
            new_ctx.scopes.reserve(64);
            return &new_ctx;
        }();
        return *ctx;
    }

    void Write(std::thread::id thread_id,
               const char *name,
               std::chrono::steady_clock::time_point start,
               std::chrono::nanoseconds duration) {

        std::lock_guard lock(mutex_);
        out_stream_ << std::setprecision(3) << std::fixed;
        out_stream_ << ",{";
        out_stream_ << R"("cat":"function",)";
        out_stream_ << "\"dur\":" << std::chrono::duration_cast<std::chrono::microseconds>(duration).count() << ',';
        out_stream_ << R"("name":")" << name << "\",";
        out_stream_ << R"("ph":"X",)";
        out_stream_ << "\"pid\":0,";
        out_stream_ << R"("tid":")" << thread_id << "\",";
        out_stream_ << "\"ts\":" << std::chrono::time_point_cast<std::chrono::microseconds>(start).time_since_epoch().count();
        out_stream_ << "}";
    }

private:
    std::vector<ThreadContext> contexts_;
    std::stringstream out_stream_;
    std::mutex mutex_;
};

static Profiler &get() {
    static Profiler profiler;
    return profiler;
}

struct Scope {
public:
    explicit Scope(const char *name) { get().BeginScope(name); }
    ~Scope() { get().EndScope(); }
};

};

#ifndef SPRINT_PROFILE
#define SPRINT_PROFILE 0
#endif

#if SPRINT_PROFILE
#   define SPRINT_PROFILE_SCOPE(__name) ::sprint::profiler::Scope SPRINT_CONCAT(_profile_scope, __LINE__)(__name);
#   define SPRINT_PROFILE_FUNCTION() SPRINT_PROFILE_SCOPE(SPRINT_FUNCTION)
#else
#   define SPRINT_PROFILE_SCOPE(__name)
#   define SPRINT_PROFILE_FUNCTION()
#endif

};
