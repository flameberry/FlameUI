#pragma once
#include <GLFW/glfw3.h>
#include <memory>

namespace FlameBerry {
    class Window
    {
    public:
        Window(uint32_t width, uint32_t height, const char* title, bool vsync);
        ~Window();

        static std::shared_ptr<Window> Create(uint32_t width = 1280, uint32_t height = 720, const char* title = "FlameBerry Engine", bool vsync = true);
        void OnUpdate() const;
        inline GLFWwindow* GetNativeWindow() const { return m_Window; }

        void UpdateDimensions();

        uint32_t GetWidth();
        uint32_t GetHeight();

        bool IsRunning() const;
    private:
        struct vec2
        {
            uint32_t X, Y;
        };
    private:
        GLFWwindow* m_Window;
    private:
        vec2 m_WindowDimensions;
        const char* m_Title;
        bool m_Vsync;
    };
}
