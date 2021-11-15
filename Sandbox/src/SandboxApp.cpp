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

    glm::ivec2 string_dimensions;
    FlameUI::Renderer::AddText("FlameBerry", { 0, 0 }, 5.0f, { 1.0f, 1.0f, 0.0f, 1.0f });
    FlameUI::Renderer::AddText("string", { -100, 0 }, 0.6f, { 1.0f, 1.0f, 1.0f, 1.0f }, &string_dimensions);
    FlameUI::Renderer::AddText("Umm", { 0, 0 }, 1.0f, glm::vec4(1.0f));

    FL_LOG("String width: {0}, height: {1}", string_dimensions.x, string_dimensions.y);

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
