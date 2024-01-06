#include "Input.h"
#include "renderer/Renderer.h"

namespace FlameUI {
    GLFWwindow* Input::m_GLFWwindowCache = NULL;
    glm::dvec2  Input::m_CursorPos;

    bool Input::IsKey(uint16_t key, uint16_t action)
    {
        if (glfwGetKey(GetCachedWindow(), key) == action)
            return true;
        return false;
    }

    bool Input::IsMouseButton(uint16_t button, uint16_t action)
    {
        if (glfwGetMouseButton(GetCachedWindow(), button) == action)
            return true;
        return false;
    }

    bool Input::IsCursorInRect2D(const Rect2D& rect2D)
    {
        glm::vec2 cursor_pos = LatestCursorPos();
        if (cursor_pos.x >= rect2D.l && cursor_pos.x <= rect2D.r && cursor_pos.y >= rect2D.b && cursor_pos.y <= rect2D.t)
            return true;
        return false;
    }

    const glm::vec2& Input::LatestCursorPos()
    {
        double x, y;
        glfwGetCursorPos(GetCachedWindow(), &x, &y);
        m_CursorPos.x = x - Renderer::GetViewportSize().x / Renderer::GetWindowContentScale().x / 2.0f;
        m_CursorPos.y = -y + Renderer::GetViewportSize().y / Renderer::GetWindowContentScale().y / 2.0f;
        return m_CursorPos;
    }

    GLFWwindow* Input::GetCachedWindow()
    {
        if (!m_GLFWwindowCache)
            m_GLFWwindowCache = Renderer::GetUserGLFWwindow();
        return m_GLFWwindowCache;
    }
}