#include "Log.h"

namespace FlameUI {
    std::shared_ptr<flamelogger::FLInstance> Log::s_CoreLoggerInstance;

    void Log::Init()
    {
        s_CoreLoggerInstance = flamelogger::FLInstance::Create("FLAMEUI");
        s_CoreLoggerInstance->SetLogLevel(flamelogger::LogLevel::TRACE);
    }
}