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
        FL_TIMER_SCOPE("add_text");
        FlameUI::Renderer::AddText("Flameberry", { 0, 0 }, 1.0f, { 1.0f, 1.0f, 0.0f, 1.0f });
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
