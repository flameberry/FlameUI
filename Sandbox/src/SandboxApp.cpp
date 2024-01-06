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

            FlameUI::Renderer::Begin();
            // FlameUI::Renderer::AddQuad({ 0, 0, 0 }, { 3.5f, 0.2f }, FL_WHITE, FL_ELEMENT_TYPE_GENERAL_INDEX, FlameUI::UnitType::OPENGL_UNITS);
            m_FlameUILayer.OnRender();
            FlameUI::Renderer::End();

            m_Window.OnUpdate();
        }
    }

    SandboxApp::~SandboxApp()
    {
        FlameUI::Renderer::CleanUp();
        glfwTerminate();
    }
}