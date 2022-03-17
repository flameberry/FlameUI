#include "FlameUILayer.h"
#include "SandboxApp.h"

namespace Flameberry {
    void FlameUILayer::OnAttach()
    {
        FlameUI::ThemeInfo themeInfo{};
        themeInfo.panelTitleBarColor = { 0.15f, 0.1505f, 0.151f, 1.0f };

        FlameUI::RendererInitInfo rendererInitInfo{};
        rendererInitInfo.userWindow = SandboxApp::GetApp()->GetMainWindow().GetGLFWwindow();
        rendererInitInfo.enableFontRendering = false;
        rendererInitInfo.themeInfo = &themeInfo;

        FlameUI::Renderer::Init(rendererInitInfo);

        // m_Panels.emplace_back(FlameUI::Panel::Create("Panel", glm::vec2{ 0.0f }, { 200.0f, 550.0f }, FL_PURPLE));
        // m_Panels.emplace_back(FlameUI::Panel::Create("PanelOne", glm::vec2{ 0.0f }, { 300.0f, 600.0f }, FL_WHITE));
        // m_Panels.emplace_back(FlameUI::Panel::Create("PanelTwo", glm::vec2{ 0.0f }, { 100.0f, 550.0f }, FL_BLUE));
        // m_Panels.emplace_back(FlameUI::Panel::Create("PanelTwo", glm::vec2{ 0.0f }, { 300.0f, 300.0f }, FL_DARK_BLUE));

        FlameUI::EventPipeline::SubmitPanel("Panel", glm::vec2{ 0.0f }, { 200.0f, 550.0f }, FL_PURPLE);
        FlameUI::EventPipeline::SubmitPanel("PanelOne", glm::vec2{ 0.0f }, { 300.0f, 600.0f }, FL_WHITE);
        FlameUI::EventPipeline::SubmitPanel("PanelTwo", glm::vec2{ 0.0f }, { 100.0f, 550.0f }, FL_BLUE);
        FlameUI::EventPipeline::SubmitPanel("PanelTwo", glm::vec2{ 0.0f }, { 300.0f, 300.0f }, FL_DARK_BLUE);

        FlameUI::EventPipeline::Prepare();
    }

    void FlameUILayer::OnRender()
    {
        FlameUI::Renderer::Begin();
        FlameUI::EventPipeline::Execute();
        // for (auto& panel : m_Panels)
        //     panel->OnDraw();
        FlameUI::Renderer::End();
    }
}