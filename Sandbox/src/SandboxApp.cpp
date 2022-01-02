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

    FlameUI::PanelCreateInfo panelCreateInfo{};
    panelCreateInfo.position = { 0.0f, 0.0f };
    panelCreateInfo.dimensions = { 200.0f, 550.0f };
    panelCreateInfo.color = purple;

    FlameUI::Panel panel(panelCreateInfo);

    FlameUI::EventPipeline::Prepare({ panel });

    while (window.IsRunning())
    {
        glClearColor(pink.x, pink.y, pink.z, pink.w);
        glClear(GL_COLOR_BUFFER_BIT);

        FlameUI::Renderer::Begin();
        FlameUI::EventPipeline::Execute();
        FlameUI::Renderer::End();

        window.OnUpdate();
    }
    FlameUI::Renderer::CleanUp();
    glfwTerminate();
}
