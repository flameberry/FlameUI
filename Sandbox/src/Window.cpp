#include "Window.h"
#include <glad/glad.h>
#include <iostream>

namespace FlameBerry {
    std::shared_ptr<Window> Window::Create(uint32_t width, uint32_t height, const char* title, bool vsync)
    {
        return std::make_shared<Window>(width, height, title, vsync);
    }

    Window::Window(uint32_t width, uint32_t height, const char* title, bool vsync)
        : m_WindowDimensions({ width, height }), m_Title(title), m_Vsync(vsync)
    {
        if (!glfwInit())
            std::cout << "Failed to Initialize GLFW!" << std::endl;

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif

        m_Window = glfwCreateWindow(width, height, "FlameUI", NULL, NULL);

        if (!m_Window)
            std::cout << "Window is null!" << std::endl;

        glfwMakeContextCurrent(m_Window);
        m_Vsync ? glfwSwapInterval(1) : glfwSwapInterval(0);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
        }

        glViewport(0, 0, width, height);

        glfwSetWindowUserPointer(m_Window, &m_WindowDimensions);

        glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
            {
                vec2& windowDimensions = *(vec2*)glfwGetWindowUserPointer(window);
                windowDimensions.X = width;
                windowDimensions.Y = height;
            }
        );
    }

    uint32_t Window::GetWidth()
    {
        return m_WindowDimensions.X;
    }

    uint32_t Window::GetHeight()
    {
        return m_WindowDimensions.Y;
    }


    void Window::UpdateDimensions()
    {
        glfwGetWindowSize(m_Window, (int*)&m_WindowDimensions.X, (int*)&m_WindowDimensions.Y);
    }

    Window::~Window()
    {
        glfwDestroyWindow(m_Window);
    }

    void Window::OnUpdate() const
    {
        glfwSwapBuffers(m_Window);
        glfwPollEvents();
    }

    bool Window::IsRunning() const
    {
        return !(glfwWindowShouldClose(m_Window));
    }
}
