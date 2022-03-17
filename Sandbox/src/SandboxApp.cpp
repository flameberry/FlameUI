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
            glClearColor(FL_PINK.x, FL_PINK.y, FL_PINK.z, FL_PINK.w);
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