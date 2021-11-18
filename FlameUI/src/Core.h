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
#include <flameberrylogger/fby_logger.h>

#define FL_LOG(...) fby_logger::log(__VA_ARGS__)
#define FL_DO_ON_ASSERT(x, ...) if(!(x)) __VA_ARGS__;
#define FL_ASSERT(x, ...) FL_DO_ON_ASSERT(x, printf("Assertion Failed: "), fby_logger::log(__VA_ARGS__), fby_logger::log("File: {0}\nLine: {1}\nFunction: {2}()", __FILE__, __LINE__, __FUNCTION__), FL_DEBUGBREAK())
#define FL_BASIC_ASSERT(x) FL_DO_ON_ASSERT(x, fby_logger::log("Assertion Failed: "), fby_logger::log("File: {0}\nLine: {1}\nFunction: {2}()", __FILE__, __LINE__, __FUNCTION__), FL_DEBUGBREAK())
#define FL_ERROR(...) FL_DO_ON_ASSERT(0, fby_logger::log(__VA_ARGS__), FL_DEBUGBREAK())

#elif defined(FL_RELEASE)

#define FL_LOG(...)
#define FL_DO_ON_ASSERT(x, ...)
#define FL_ASSERT(x, ...) if (!(x));
#define FL_BASIC_ASSERT(x) if (!(x));
#define FL_ERROR(...)

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
    enum class QuadPosType
    {
        None = 0, QuadPosBottomLeftVertex, QuadPosCenter
    };

    struct Bounds
    {
        float Left, Right, Bottom, Top;
    };
}

#define FL_QUAD_POS_BOTTOM_LEFT_VERTEX FlameUI::QuadPosType::QuadPosBottomLeftVertex
#define FL_QUAD_POS_CENTER FlameUI::QuadPosType::QuadPosCenter