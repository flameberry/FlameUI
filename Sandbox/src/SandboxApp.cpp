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

    // FlameUI::PanelCreateInfo panelCreateInfo{};
    // panelCreateInfo.position = { 0.0f, 0.0f };
    // panelCreateInfo.dimensions = { 200.0f, 550.0f };
    // panelCreateInfo.color = purple;

    // FlameUI::PanelCreateInfo panelCreateInfo1{};
    // panelCreateInfo1.position = { 0.0f, 0.0f };
    // panelCreateInfo1.dimensions = { 300, 600 };
    // panelCreateInfo1.color = white;

    // FlameUI::PanelCreateInfo panelCreateInfo2{};
    // panelCreateInfo2.position = { 0.0f, 0.0f };
    // panelCreateInfo2.dimensions = { 100.0f, 550.0f };
    // panelCreateInfo2.color = blue;

    // FlameUI::EventPipeline::SubmitPanel(panelCreateInfo);
    // FlameUI::EventPipeline::SubmitPanel(panelCreateInfo1);
    // FlameUI::EventPipeline::SubmitPanel(panelCreateInfo2);

    // FlameUI::EventPipeline::Prepare();

    // FlameUI::Renderer::AddText("Flameberry", { 0, 0 }, 2.0f, yellow);

    while (window.IsRunning())
    {
        glClearColor(pink.x, pink.y, pink.z, pink.w);
        glClear(GL_COLOR_BUFFER_BIT);

        FlameUI::Renderer::Begin();
        // FlameUI::Renderer::AddQuad({ 0, 0, 0 }, { 100.0f, 100.0f }, white, FL_ELEMENT_TYPE_GENERAL_INDEX);
        // FlameUI::Renderer::AddQuad({ 0, 100.0f, 0 }, { 100.0f, 100.0f }, red, FL_ELEMENT_TYPE_GENERAL_INDEX);
        // FlameUI::Renderer::AddQuad({ 100.0f, 0, 0 }, { 100.0f, 100.0f }, blue, FL_ELEMENT_TYPE_GENERAL_INDEX);
        for (uint32_t x = 0; x < 1000; x += 300)
        {
            for (uint32_t y = 0; y < 500; y += 300)
            {
                FlameUI::Renderer::AddQuad({ -590.0f + x, -310.0f + y, 0 }, { 100.0f, 100.0f }, white, FL_ELEMENT_TYPE_GENERAL_INDEX, FL_PROJECT_DIR"Sandbox/resources/textures/icon.png");
                FlameUI::Renderer::AddQuad({ -490.0f + x, -310.0f + y, 0 }, { 100.0f, 100.0f }, white, FL_ELEMENT_TYPE_GENERAL_INDEX, FL_PROJECT_DIR"Sandbox/resources/textures/pause_icon.png");
                FlameUI::Renderer::AddQuad({ -390.0f + x, -310.0f + y, 0 }, { 100.0f, 100.0f }, white, FL_ELEMENT_TYPE_GENERAL_INDEX, FL_PROJECT_DIR"Sandbox/resources/textures/Checkerboard.png");
            }
        }
        // FlameUI::Renderer::AddQuad({ 0.0f, -100.0f, 0 }, { 100.0f, 100.0f }, white, FL_ELEMENT_TYPE_GENERAL_INDEX, FL_PROJECT_DIR"Sandbox/resources/textures/icon.png");
        // FlameUI::Renderer::AddQuad({ 0.0f, 100.0f, 0 }, { 100.0f, 100.0f }, white, FL_ELEMENT_TYPE_GENERAL_INDEX, FL_PROJECT_DIR"Sandbox/resources/textures/pause_icon.png");
        // FlameUI::Renderer::AddQuad({ 100.0f, 100.0f, 0 }, { 100.0f, 100.0f }, white, FL_ELEMENT_TYPE_GENERAL_INDEX, FL_PROJECT_DIR"Sandbox/resources/textures/Checkerboard.png");
        // FlameUI::EventPipeline::Execute();
        FlameUI::Renderer::End();

        window.OnUpdate();
    }
    FlameUI::Renderer::CleanUp();
    glfwTerminate();
}
