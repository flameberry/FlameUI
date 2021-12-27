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
        /// Constructor that takes in the scope name, which will be written in `Timer.log` file
        Timer(const std::string& scopeName);
        ~Timer();

        /// Flushes all the timings named in the file `Timer.log`
        static void FlushTimings();
    private:
        struct ScopeInfo
        {
            /// Name of the scope that will be displayed in `Timer.log` file to refer to a process
            std::string ScopeName;
            float       Duration;

            ScopeInfo(const std::string& scopeName, float duration)
                : ScopeName(scopeName), Duration(duration)
            {
            }
        };
    private:
        /// Stores all the scopes that have been recorded in the program
        static std::vector<ScopeInfo>                               s_TimerScopeData;
        /// Name of the scope, that the `Timer` is currently in 
        std::string                                                 m_ScopeName;
        /// Start Time point, measured when constructor of this class is called
        std::chrono::time_point<std::chrono::high_resolution_clock> m_Start;
    };
}

/// Macros for the `Timer` class, making it easier to time things
#define FL_TIMER_SCOPE(scope_name) FlameUI::Timer timer(scope_name)
#define FL_FLUSH_TIMINGS() FlameUI::Timer::FlushTimings()
#else
#define FL_TIMER_SCOPE(scope_name)
#define FL_FLUSH_TIMINGS()
#endif