#pragma once
#include <GLFW/glfw3.h>
#include <memory>
#include <glm/glm.hpp>

namespace FlameBerry {
    class Window
    {
    public:
        Window(uint32_t width = 1280, uint32_t height = 720, const char* title = "FlameUI", bool vsync = true);
        ~Window();

        static std::shared_ptr<Window> Create(uint32_t width = 1280, uint32_t height = 720, const char* title = "FlameBerry Engine", bool vsync = true);
        void OnUpdate() const;
        inline GLFWwindow* GetNativeWindow() const { return m_Window; }

        void UpdateDimensions();

        uint32_t GetWidth();
        uint32_t GetHeight();

        bool IsRunning() const;
    private:
        GLFWwindow* m_Window;
    private:
        glm::vec2 m_WindowDimensions;
        const char* m_Title;
        bool m_Vsync;
    };
}
