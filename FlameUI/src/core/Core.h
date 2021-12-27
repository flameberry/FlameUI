#pragma once

#ifdef FL_DEBUG
#ifdef __APPLE__
#define FL_DEBUGBREAK() abort()
#elif defined(WIN32)
#define FL_DEBUGBREAK() __debugbreak()
#endif
#else
#define FL_DEBUGBREAK()
#endif

#ifdef FL_DEBUG
#include "../utils/Log.h"

namespace flDebug {
    template<typename T, typename... Args>
    static void fl_print_msg_on_assert(const char* file, int line, const T& message, const Args&... args)
    {
        std::string msg = flamelogger::format_string(message, args...);
        std::string assert_message = flamelogger::format_string("{0}[ASSERT] Assertion failed: {1} (file: {2}, line: {3})", flamelogger::get_current_time_string(), msg, file, line);
        std::cout << FL_COLOR_RED << assert_message << FL_COLOR_DEFAULT << std::endl;
    }

    static void fl_print_msg_on_assert(const char* file, int line)
    {
        std::string assert_message = flamelogger::format_string("{0}[ASSERT] Assertion failed, file: {1}, line: {2}", flamelogger::get_current_time_string(), file, line);
        std::cout << FL_COLOR_RED << assert_message << FL_COLOR_DEFAULT << std::endl;
    }
}

#define FL_LOGGER_INIT() FlameUI::Log::Init()

#define FL_DO_ON_ASSERT(x, ...) if(!(x)) __VA_ARGS__;
#define FL_ASSERT(x, ...) FL_DO_ON_ASSERT(x, flDebug::fl_print_msg_on_assert(__FILE__, __LINE__, __VA_ARGS__), FL_DEBUGBREAK())
#define FL_BASIC_ASSERT(x) FL_DO_ON_ASSERT(x, flDebug::fl_print_msg_on_assert(__FILE__, __LINE__), FL_DEBUGBREAK())

#define FL_DO_IN_ORDER(...) __VA_ARGS__;

#define FL_LOG(...) FlameUI::Log::GetCoreLoggerInstance()->log(__VA_ARGS__)
#define FL_INFO(...) FlameUI::Log::GetCoreLoggerInstance()->info(__VA_ARGS__)
#define FL_WARN(...) FlameUI::Log::GetCoreLoggerInstance()->warn(__VA_ARGS__)
#define FL_ERROR(...) FlameUI::Log::GetCoreLoggerInstance()->error(__VA_ARGS__)

#elif defined(FL_RELEASE)

#define FL_LOGGER_INIT(project_name)

#define FL_DO_ON_ASSERT(x, ...)
#define FL_ASSERT(x, ...) if (!(x));
#define FL_BASIC_ASSERT(x) if (!(x));

#define FL_LOG(...)
#define FL_INFO(...)
#define FL_ERROR(...)
#define FL_WARN(...)

#endif

#ifdef FL_DEBUG

#include <glad/glad.h>
#include <string>

namespace FlameUI {
    /// Returns the appropriate OpenGL Error message
    static std::string GetErrorString(GLenum error)
    {
        switch (error)
        {
        case GL_INVALID_ENUM:                  return "INVALID_ENUM"; break;
        case GL_INVALID_VALUE:                 return "INVALID_VALUE"; break;
        case GL_INVALID_OPERATION:             return "INVALID_OPERATION"; break;
        case GL_STACK_OVERFLOW:                return "STACK_OVERFLOW"; break;
        case GL_STACK_UNDERFLOW:               return "STACK_UNDERFLOW"; break;
        case GL_OUT_OF_MEMORY:                 return "OUT_OF_MEMORY"; break;
        case GL_INVALID_FRAMEBUFFER_OPERATION: return "INVALID_FRAMEBUFFER_OPERATION"; break;
        default: return 0; break;
        }
    }

    /// Prints the OpenGL error message if error found
    static void GL_Error()
    {
        while (GLenum error = glGetError())
            FL_ERROR("[OpenGL Error]: {0}", GetErrorString(error));
    }
}

#define GL_ERROR() while (GLenum error = glGetError()) FL_ERROR("[OpenGL Error]: {0}", FlameUI::GetErrorString(error))

#define GL_CHECK_ERROR(x) while (glGetError() != GL_NO_ERROR);\
x;\
GL_ERROR()

#else
#define GL_CHECK_ERROR(x) x
#endif

namespace FlameUI {
    enum class QuadPosType { None = 0, QuadPosBottomLeftVertex, QuadPosCenter };
    struct Bounds
    {
        float Left, Right, Bottom, Top;
        Bounds() = default;
        Bounds(float val)
            : Left(val), Right(val), Bottom(val), Top(val)
        {
        }
        Bounds(float left, float right, float bottom, float top)
            : Left(left), Right(right), Bottom(bottom), Top(top)
        {
        }
    };
}

#define FL_QUAD_POS_BOTTOM_LEFT_VERTEX FlameUI::QuadPosType::QuadPosBottomLeftVertex
#define FL_QUAD_POS_CENTER FlameUI::QuadPosType::QuadPosCenter