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

    glm::vec4 white(1.0f);
    glm::vec4 yellow(1.0f, 1.0f, 0.0f, 1.0f);
    glm::vec4 purple(1.0f, 0.0f, 1.0f, 1.0f);
    glm::vec4 blue(0.0f, 1.0f, 1.0f, 1.0f);
    glm::vec4 black(0.0f, 0.0f, 0.0f, 1.0f);
    glm::vec4 dark_blue(0.0f, 0.0f, 1.0f, 1.0f);
    glm::vec4 red(1.0f, 0.0f, 0.0f, 1.0f);

    std::shared_ptr<FlameUI::Panel> panel = FlameUI::Panel::Create("panel", { 0, 0 }, { 200, 550 }, white);
    std::shared_ptr<FlameUI::Panel> panelOne = FlameUI::Panel::Create("panelOne", { 0, 0 }, { 250, 450 }, yellow);
    std::shared_ptr<FlameUI::Panel> panelTwo = FlameUI::Panel::Create("panelTwo", { 0, 0 }, { 300, 600 }, purple);
    std::shared_ptr<FlameUI::Panel> panelThree = FlameUI::Panel::Create("panelThree", { 0, 0 }, { 100, 400 }, blue);

    panel->AddButton("Click Me!", { 100, 50 }, yellow, "");
    panel->AddButton("Click Me Too!", { 100, 50 }, black, "");

    FlameUI::_FlameUI::AddPanel(panel);
    FlameUI::_FlameUI::AddPanel(panelOne);
    FlameUI::_FlameUI::AddPanel(panelTwo);
    FlameUI::_FlameUI::AddPanel(panelThree);

    FlameUI::_FlameUI::Init();

    while (window->IsRunning())
    {
        FL_TIMER_SCOPE("Per_Frame");
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        FlameUI::Renderer::OnUpdate();
        FlameUI::_FlameUI::OnUpdate();
        FlameUI::Renderer::OnDraw();

        window->OnUpdate();
    }
    FlameUI::Renderer::CleanUp();
    glfwTerminate();

    FL_FLUSH_TIMINGS();
}
