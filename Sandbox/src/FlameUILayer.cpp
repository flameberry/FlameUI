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
        themeInfo.buttonHoveredColor = { 0.26f, 0.59f, 0.98f, 1.00f };

        FlameUI::RendererInitInfo rendererInitInfo{};
        rendererInitInfo.userWindow = SandboxApp::GetApp()->GetMainWindow().GetGLFWwindow();
        rendererInitInfo.enableFontRendering = true;
        rendererInitInfo.fontFilePath = FL_PROJECT_DIR"FlameUI/resources/fonts/OpenSans-Regular.ttf";
        rendererInitInfo.themeInfo = &themeInfo;

        FlameUI::Renderer::Init(rendererInitInfo);

        FlameUI::Pipeline::SubmitPanel("Panel", glm::vec2{ 0.0f }, { 200.0f, 550.0f }, FL_PURPLE);
        FlameUI::Pipeline::SubmitButton("whatever", { 80.0f, 30.0f });

        FlameUI::Pipeline::SubmitPanel("PanelOne", glm::vec2{ 0.0f }, { 300.0f, 600.0f }, FL_WHITE);
        FlameUI::Pipeline::SubmitPanel("PanelTwo", glm::vec2{ 0.0f }, { 100.0f, 550.0f }, FL_BLUE);
        FlameUI::Pipeline::SubmitPanel("PanelThree", glm::vec2{ 0.0f }, { 300.0f, 300.0f }, FL_DARK_BLUE);
        FlameUI::Pipeline::SubmitPanel("PanelFour", glm::vec2{ 0.0f }, { 300.0f, 200.0f }, FL_DARK_BLUE);

        FlameUI::Pipeline::Prepare();
    }

    void FlameUILayer::OnRender()
    {
        FlameUI::Pipeline::Execute();
    }
}