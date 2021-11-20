#include <iostream>
#include "FlameUI.h"
#include "Window.h"

int main()
{
    std::shared_ptr<FlameBerry::Window> window = FlameBerry::Window::Create();
    FlameUI::Renderer::SetUIFont(FL_PROJECT_DIR + std::string("FlameUI/resources/fonts/OpenSans-Regular.ttf"));
    {
        FL_TIMER_SCOPE("renderer_init");
        FlameUI::Renderer::Init(window->GetNativeWindow());
    }

    std::shared_ptr<FlameUI::Panel> panel = FlameUI::Panel::Create("panel", { 0, 0 }, { 200, 550 }, glm::vec4(1.0f), "");
    std::shared_ptr<FlameUI::Panel> panelOne = FlameUI::Panel::Create("panelOne", { 0, 0 }, { 250, 450 }, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), "");
    std::shared_ptr<FlameUI::Panel> panelTwo = FlameUI::Panel::Create("panelTwo", { 0, 0 }, { 300, 600 }, glm::vec4(1.0f, 0.0f, 1.0f, 1.0f), "");
    std::shared_ptr<FlameUI::Panel> panelThree = FlameUI::Panel::Create("panelThree", { 0, 0 }, { 100, 400 }, glm::vec4(0.0f, 1.0f, 1.0f, 1.0f), "");

    FlameUI::_FlameUI::AddPanel(panel);
    FlameUI::_FlameUI::AddPanel(panelOne);
    FlameUI::_FlameUI::AddPanel(panelTwo);
    FlameUI::_FlameUI::AddPanel(panelThree);
    // uint32_t quadid1;
    // uint32_t quadid2;
    // uint32_t quadid3;
    // uint32_t quadid4;
    // FlameUI::Renderer::AddQuad(&quadid1, FL_QUAD_POS_CENTER, { 0, 0 }, { 200, 200 }, { 1.0f, 0.0f, 1.0f, 1.0f }, FL_PROJECT_DIR"Sandbox/resources/textures/Checkerboard.png");
    // FlameUI::Renderer::AddQuad(&quadid2, FL_QUAD_POS_CENTER, { 200, 0 }, { 100, 100 }, { 1.0f, 0.0f, 1.0f, 1.0f }, "");

    // FlameUI::Renderer::AddQuad(&quadid3, FL_QUAD_POS_CENTER, { -200, 0 }, { 100, 100 }, { 1.0f, 0.0f, 1.0f, 1.0f }, FL_PROJECT_DIR"Sandbox/resources/textures/pause_icon.png");
    // FlameUI::Renderer::AddQuad(&quadid4, FL_QUAD_POS_CENTER, { 100, -200 }, { 200, 200 }, { 1.0f, 1.0f, 0.0f, 1.0f }, "");

    // FlameUI::Renderer::AddText("Hello", { -630, 0 }, 1.0f, { 1.0f, 0.0f, 1.0f, 1.0f });

    FlameUI::_FlameUI::Init();

    while (window->IsRunning())
    {
        FL_TIMER_SCOPE("Per_Frame");
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        FlameUI::Renderer::OnUpdate();
        FlameUI::_FlameUI::CheckFocus();
        FlameUI::_FlameUI::OnUpdate();

        // static bool once_1 = true;
        // if (glfwGetKey(window->GetNativeWindow(), GLFW_KEY_1) == GLFW_PRESS)
        // {
        //     if (once_1)
        //     {
        //         FlameUI::Renderer::RemoveQuad(&quadid1);
        //         once_1 = false;
        //     }
        // }
        // if (glfwGetKey(window->GetNativeWindow(), GLFW_KEY_1) == GLFW_RELEASE)
        //     once_1 = true;

        // static bool once_2 = true;
        // if (glfwGetKey(window->GetNativeWindow(), GLFW_KEY_2) == GLFW_PRESS)
        // {
        //     if (once_2)
        //     {
        //         FlameUI::Renderer::RemoveQuad(&quadid2);
        //         once_2 = false;
        //     }
        // }
        // if (glfwGetKey(window->GetNativeWindow(), GLFW_KEY_2) == GLFW_RELEASE)
        //     once_2 = true;

        // static bool once_3 = true;
        // if (glfwGetKey(window->GetNativeWindow(), GLFW_KEY_3) == GLFW_PRESS)
        // {
        //     if (once_3)
        //     {
        //         FlameUI::Renderer::RemoveQuad(&quadid3);
        //         once_3 = false;
        //     }
        // }
        // if (glfwGetKey(window->GetNativeWindow(), GLFW_KEY_3) == GLFW_RELEASE)
        //     once_3 = true;

        // static bool once_4 = true;
        // if (glfwGetKey(window->GetNativeWindow(), GLFW_KEY_4) == GLFW_PRESS)
        // {
        //     if (once_4)
        //     {
        //         FlameUI::Renderer::RemoveQuad(&quadid4);
        //         once_4 = false;
        //     }
        // }
        // if (glfwGetKey(window->GetNativeWindow(), GLFW_KEY_4) == GLFW_RELEASE)
        //     once_4 = true;

        FlameUI::Renderer::OnDraw();

        window->OnUpdate();
    }
    FlameUI::Renderer::CleanUp();
    glfwTerminate();

    FL_FLUSH_TIMINGS();
}
