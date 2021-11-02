#pragma once
#define FL_PROFILING

#ifdef FL_PROFILING
#include <chrono>
#include <string>
#include <vector>

namespace FlameUI {
    class Timer
    {
    public:
        Timer(const std::string& scopeName);
        ~Timer();

        static void FlushTimings();
    private:
        struct ScopeInfo
        {
            std::string ScopeName;
            float Duration;

            ScopeInfo(const std::string& scopeName, float duration)
                : ScopeName(scopeName), Duration(duration)
            {
            }
        };
    private:
        static std::vector<ScopeInfo> s_TimerScopeData;
        std::string m_ScopeName;
        std::chrono::time_point<std::chrono::high_resolution_clock> m_Start;
    };
}

#define FL_TIMER_SCOPE(scope_name) FlameUI::Timer timer(scope_name)
#define FL_FLUSH_TIMINGS() FlameUI::Timer::FlushTimings()
#else
#define FL_TIMER_SCOPE(scope_name)
#define FL_FLUSH_TIMINGS()
#endif