#include <iostream>
#include "FlameUI.h"
#include "Window.h"

int main()
{
    glm::vec4 white{ 1.0f };
    glm::vec4 yellow{ 1.0f, 1.0f, 0.0f, 1.0f };
    glm::vec4 purple{ 1.0f, 0.0f, 1.0f, 1.0f };
    glm::vec4 blue{ 0.0f, 1.0f, 1.0f, 1.0f };
    glm::vec4 black{ 0.0f, 0.0f, 0.0f, 1.0f };
    glm::vec4 dark_blue{ 0.0f, 0.0f, 1.0f, 1.0f };
    glm::vec4 red{ 1.0f, 0.0f, 0.0f, 1.0f };
    glm::vec4 pink{ 1.0f, 157.0f / 255.0f, 207.0f / 255.0f, 1.0f };

    FlameBerry::Window window;

    FlameUI::ThemeInfo themeInfo{};
    themeInfo.panelTitleBarColor = { 0.15f, 0.1505f, 0.151f, 1.0f };

    FlameUI::RendererInitInfo rendererInitInfo{};
    rendererInitInfo.userWindow = window.GetNativeWindow();
    rendererInitInfo.enableFontRendering = false;
    rendererInitInfo.themeInfo = &themeInfo;

    FlameUI::Renderer::Init(rendererInitInfo);

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
        glClearColor(pink.x, pink.y, pink.z, pink.w);
        glClear(GL_COLOR_BUFFER_BIT);

        FlameUI::Renderer::OnUpdate();
        FlameUI::EventPipeline::Execute();
        FlameUI::Renderer::OnDraw();

        window.OnUpdate();
    }
    FlameUI::Renderer::CleanUp();
    glfwTerminate();
}
