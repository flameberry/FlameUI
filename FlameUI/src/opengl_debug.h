#pragma once
#ifdef FL_DEBUG

#include <glad/glad.h>
#include "core_macros.h"
#include <string>

namespace FlameUI {
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
