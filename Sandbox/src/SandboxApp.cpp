#include <iostream>
#include "FlameUI.h"
#include "Window.h"

int main()
{
    std::shared_ptr<FlameBerry::Window> window = FlameBerry::Window::Create();

    FlameUI::Font openSansFont;
    openSansFont.FilePath = FL_PROJECT_DIR + std::string("FlameUI/resources/fonts/OpenSans-Regular.ttf");

    openSansFont.PixelRange = 8.0f;
    openSansFont.Strength = 0.5f;

    FlameUI::Renderer::SetUIFont(openSansFont);
    float font_scale = 1.0f;
    {
        FL_TIMER_SCOPE("renderer_init");
        FlameUI::Renderer::Init();
    }

    {
        FL_TIMER_SCOPE("add_quads");
        FlameUI::Renderer::AddText("FlameBerry", { -50, 0 }, 1.0f, { 1.0f, 1.0f, 0.0f, 1.0f });
        FlameUI::Renderer::AddQuad(nullptr, { 0, -120 }, { 100, 100 }, { 1.0f, 1.0f, 1.0f, 1.0f }, FL_PROJECT_DIR + std::string("Sandbox/resources/textures/pause_icon.png"));
        FlameUI::Renderer::AddText("Engine", { -50, 50 }, 1.0f, { 1.0f, 1.0f, 0.0f, 1.0f });
        for (uint8_t i = 0; i < 20; i++)
        {
            FlameUI::Renderer::AddQuad(nullptr, { 0 + 10 * i, 300 }, { 100, 100 }, { 1.0f, 0.0f, 1.0f, 1.0f }, FL_PROJECT_DIR + std::string("Sandbox/resources/textures/Checkerboard.png"));
            FlameUI::Renderer::AddText("Engine", { -50 + 10 * i, -300 }, 1.0f, { 1.0f, 1.0f, 0.0f, 1.0f });
        }
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
