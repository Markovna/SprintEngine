#pragma once

#include <chrono>
#include <stack>
#include <vector>
#include <thread>
#include <sstream>
#include <fstream>
#include <iomanip>
#include "macro.h"

namespace sprint {

#define SPRINT_PROFILE 1

namespace profiler {

class Profiler {
private:

    void WriteHeader() {
        out_stream_ << R"({"otherData": {},"traceEvents":[{})";
        out_stream_.flush();
    }

    void WriteFooter() {
        out_stream_ << "]}";
        out_stream_.flush();
    }

    void Write(std::thread::id thread_id,
               const char *name,
               std::chrono::steady_clock::time_point start,
               std::chrono::nanoseconds duration) {

        std::lock_guard lock(mutex_);
        std::stringstream json;
        json << std::setprecision(3) << std::fixed;
        json << ",{";
        json << R"("cat":"function",)";
        json << "\"dur\":" << std::chrono::duration_cast<std::chrono::microseconds>(duration).count() << ',';
        json << R"("name":")" << name << "\",";
        json << R"("ph":"X",)";
        json << "\"pid\":0,";
        json << R"("tid":")" << thread_id << "\",";
        json << "\"ts\":" << std::chrono::time_point_cast<std::chrono::microseconds>(start).time_since_epoch().count();
        json << "}";

        out_stream_ << json.str();
        out_stream_.flush();
    }

    struct ScopeInfo {
        const char *name = nullptr;
        std::chrono::steady_clock::time_point start;
    };

    struct ThreadContext {
        std::stack<ScopeInfo> scopes;
        std::thread::id id;
    };

    ThreadContext& GetThreadContext() {
        thread_local ThreadContext* ctx = [&](){
            ThreadContext& new_ctx = contexts_.emplace_back();
            new_ctx.id = std::this_thread::get_id();
            return &new_ctx;
        }();
        return *ctx;
    }

public:

    Profiler() {
        std::lock_guard lock(mutex_);
        out_stream_.open("results.json");
        assert(out_stream_.is_open() && "Couldn't open profiler results file.");
        WriteHeader();
    }

    ~Profiler() {
        std::lock_guard lock(mutex_);
        WriteFooter();
        out_stream_.close();
    }

    void BeginScope(const char *name) {
        GetThreadContext().scopes.push({name, std::chrono::steady_clock::now()});
    }

    void EndScope() {
        auto& ctx = GetThreadContext();
        auto& scope = ctx.scopes.top();
        auto finish = std::chrono::steady_clock::now();
        auto dur = finish - scope.start;

        Write(ctx.id, scope.name, scope.start, dur);

        ctx.scopes.pop();
    }

private:
    std::vector<ThreadContext> contexts_;
    std::ofstream out_stream_;
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


