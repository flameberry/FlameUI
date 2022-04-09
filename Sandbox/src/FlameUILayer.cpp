#include "FlameUILayer.h"
#include "SandboxApp.h"

namespace Flameberry {
    void FlameUILayer::OnAttach()
    {
        FlameUI::ThemeInfo themeInfo{};
        themeInfo.panelBgColor = { 0.06f, 0.06f, 0.06f, 1.0f };
        themeInfo.borderColor = { 0.43f, 0.43f, 0.50f, 0.50f };

        themeInfo.panelTitleBarInactiveColor = { 0.04f, 0.04f, 0.04f, 1.00f };
        themeInfo.panelTitleBarActiveColor = { 0.16f, 0.29f, 0.48f, 1.00f };

        themeInfo.buttonColor = { 0.26f, 0.59f, 0.98f, 0.40f };

        FlameUI::RendererInitInfo rendererInitInfo{};
        rendererInitInfo.userWindow = SandboxApp::GetApp()->GetMainWindow().GetGLFWwindow();
        rendererInitInfo.enableFontRendering = false;
        rendererInitInfo.themeInfo = &themeInfo;

        FlameUI::Renderer::Init(rendererInitInfo);

        FlameUI::EventPipeline::SubmitPanel("Panel", glm::vec2{ 0.0f }, { 200.0f, 550.0f }, FL_PURPLE);
        FlameUI::EventPipeline::SubmitPanel("PanelOne", glm::vec2{ 0.0f }, { 300.0f, 600.0f }, FL_WHITE);
        FlameUI::EventPipeline::SubmitPanel("PanelTwo", glm::vec2{ 0.0f }, { 100.0f, 550.0f }, FL_BLUE);
        FlameUI::EventPipeline::SubmitPanel("PanelThree", glm::vec2{ 0.0f }, { 300.0f, 300.0f }, FL_DARK_BLUE);
        FlameUI::EventPipeline::SubmitPanel("PanelFour", glm::vec2{ 0.0f }, { 300.0f, 200.0f }, FL_DARK_BLUE);

        FlameUI::EventPipeline::Prepare();
    }

    void FlameUILayer::OnRender()
    {
        FlameUI::Renderer::Begin();

        FlameUI::EventPipeline::SubmitButton("whatever", { 100.0f, 100.0f }, { 80.0f, 30.0f });
        FlameUI::EventPipeline::Execute();

        FlameUI::Renderer::End();
    }
}