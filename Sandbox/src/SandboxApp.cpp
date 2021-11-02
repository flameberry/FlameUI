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
        FL_TIMER_SCOPE("add_text");
        FlameUI::Renderer::AddText("~!@#$%^&*()_+{}|:\"<>?1234567890-=[]\\;',./", { -300, 0 }, font_scale, { 1.0f, 1.0f, 0.0f, 1.0f });
        FlameUI::Renderer::AddText("ABCDEFGHIJKLMNOPQRSTUVWXYZ", { -300, 40 }, font_scale, { 1.0f, 1.0f, 0.0f, 1.0f });
        FlameUI::Renderer::AddText("abcdefghijklmnopqrstuvwxyz", { -300, 80 }, font_scale, { 1.0f, 1.0f, 0.0f, 1.0f });
        FlameUI::Renderer::AddText("pause", { -300, -30 }, font_scale, { 1.0f, 1.0f, 0.0f, 1.0f });
    }
    {
        FL_TIMER_SCOPE("add_quads");
        FlameUI::Renderer::AddQuad(nullptr, { 0, -100 }, { 100, 100 }, { 1.0f, 1.0f, 1.0f, 1.0f }, FL_PROJECT_DIR + std::string("Sandbox/resources/textures/ChernoLogo.png"));
        FlameUI::Renderer::AddQuad(nullptr, { -200, -100 }, { 100, 100 }, { 1.0f, 1.0f, 1.0f, 1.0f }, FL_PROJECT_DIR + std::string("Sandbox/resources/textures/Checkerboard.png"));
        FlameUI::Renderer::AddQuad(nullptr, { 200, -100 }, { 100, 100 }, { 1.0f, 1.0f, 1.0f, 1.0f }, FL_PROJECT_DIR + std::string("Sandbox/resources/textures/pause_icon.png"));
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
