#include <iostream>
#include "FlameUI.h"
#include "Window.h"

int main()
{
    std::shared_ptr<FlameBerry::Window> window = FlameBerry::Window::Create();

    FlameUI::Renderer::SetUIFont(FL_PROJECT_DIR + std::string("FlameUI/resources/fonts/OpenSans-Regular.ttf"));
    {
        FL_TIMER_SCOPE("renderer_init");
        FlameUI::Renderer::Init();
    }

    {
        FL_TIMER_SCOPE("add_quads");
        FlameUI::Renderer::AddQuad(nullptr, FL_QUAD_POS_BOTTOM_LEFT_VERTEX, { 0, 0 }, { 100, 100 }, { 1.0f, 1.0f, 1.0f, 1.0f }, "");
        FlameUI::Renderer::AddText("Come on man!", { 10, 10 }, 1.0f, { 1.0f, 0.0f, 1.0f, 1.0f });
        FlameUI::Renderer::AddQuad(nullptr, FL_QUAD_POS_CENTER, { 0, 0 }, { 100, 100 }, { 1.0f, 1.0f, 0.0f, 1.0f }, "");
    }

    while (window->IsRunning())
    {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        FlameUI::Renderer::OnResize(window->GetWidth(), window->GetHeight());
        FlameUI::Renderer::OnDraw();

        window->OnUpdate();
    }
    FlameUI::Renderer::CleanUp();
    glfwTerminate();

    FL_FLUSH_TIMINGS();
}
