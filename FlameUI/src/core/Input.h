#pragma once
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include "Core.h"

namespace FlameUI {
    class Input
    {
    public:
        static bool IsKey(uint16_t key, uint16_t action);
        static bool IsMouseButton(uint16_t button, uint16_t action);
        static bool IsCursorInRect2D(const Rect2D& rect2D);
        static const glm::vec2& LatestCursorPos();
    private:
        static GLFWwindow* GetCachedWindow();
    private:
        static glm::dvec2  m_CursorPos;
        static GLFWwindow* m_GLFWwindowCache;
    };
}