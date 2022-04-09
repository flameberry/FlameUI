#include "SandboxApp.h"
#include "FlameUI.h"

namespace Flameberry {
    SandboxApp* SandboxApp::s_App;
    SandboxApp::SandboxApp()
    {
        s_App = this;
        m_FlameUILayer.OnAttach();
    }

    void SandboxApp::Run()
    {
        while (m_Window.IsRunning())
        {
            glClearColor(50.0f / 255.0f, 50.0f / 255.0f, 50.0f / 255.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            m_FlameUILayer.OnRender();
            m_Window.OnUpdate();
        }
    }

    SandboxApp::~SandboxApp()
    {
        FlameUI::Renderer::CleanUp();
        glfwTerminate();
    }
}