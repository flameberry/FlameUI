#include <iostream>
#include "FlameUI.h"
#include "Window.h"

int main()
{
    FlameBerry::Window window;

    FlameUI::Renderer::SetUIFont(FL_PROJECT_DIR + std::string("FlameUI/resources/fonts/OpenSans-Regular.ttf"));
    FlameUI::Renderer::Init(window.GetNativeWindow());

    glm::vec4 white(1.0f);
    glm::vec4 yellow(1.0f, 1.0f, 0.0f, 1.0f);
    glm::vec4 purple(1.0f, 0.0f, 1.0f, 1.0f);
    glm::vec4 blue(0.0f, 1.0f, 1.0f, 1.0f);
    glm::vec4 black(0.0f, 0.0f, 0.0f, 1.0f);
    glm::vec4 dark_blue(0.0f, 0.0f, 1.0f, 1.0f);
    glm::vec4 red(1.0f, 0.0f, 0.0f, 1.0f);

    // FlameUI::Renderer::AddText("Welcome to the glorius Flameberry Engine!", { -600, 0 }, 2.0f, yellow);

    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    const char* monitorName = glfwGetMonitorName(primaryMonitor);
    FL_INFO("Monitor name is '{0}'", monitorName);

    FlameUI::PanelCreateInfo panel_create_info{};
    panel_create_info.title = "panel";
    panel_create_info.position = { 0, 0 };
    panel_create_info.dimensions = { 200, 550 };
    panel_create_info.color = white;

    FlameUI::PanelCreateInfo panel_one_create_info{};
    panel_one_create_info.title = "panelOne";
    panel_one_create_info.position = { 0, 0 };
    panel_one_create_info.dimensions = { 250, 450 };
    panel_one_create_info.color = yellow;

    FlameUI::PanelCreateInfo panel_two_create_info{};
    panel_two_create_info.title = "panelTwo";
    panel_two_create_info.position = { 0, 0 };
    panel_two_create_info.dimensions = { 300, 600 };
    panel_two_create_info.color = purple;

    FlameUI::PanelCreateInfo panel_three_create_info{};
    panel_three_create_info.title = "panelThree";
    panel_three_create_info.position = { 0, 0 };
    panel_three_create_info.dimensions = { 100, 400 };
    panel_three_create_info.color = blue;

    FlameUI::Panel panel(panel_create_info);
    FlameUI::Panel panelOne(panel_one_create_info);
    FlameUI::Panel panelTwo(panel_two_create_info);
    FlameUI::Panel panelThree(panel_three_create_info);

    std::vector<FlameUI::Panel> panels = { panel, panelOne, panelTwo, panelThree };
    FlameUI::EventPipeline::Prepare(panels);

    while (window.IsRunning())
    {
        FL_TIMER_SCOPE("Per_Frame");
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        FlameUI::Renderer::OnUpdate();
        FlameUI::EventPipeline::Execute();
        FlameUI::Renderer::OnDraw();

        window.OnUpdate();
    }
    FlameUI::Renderer::CleanUp();
    glfwTerminate();

    FL_FLUSH_TIMINGS();
}
