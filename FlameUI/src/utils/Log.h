#pragma once
#include "../flamelogger/flamelogger.h"

namespace FlameUI {
    class Log
    {
    public:
        static void Init();
        static std::shared_ptr<flamelogger::FLInstance> GetCoreLoggerInstance() { return s_CoreLoggerInstance; }
    private:
        static std::shared_ptr<flamelogger::FLInstance> s_CoreLoggerInstance;
    };
}