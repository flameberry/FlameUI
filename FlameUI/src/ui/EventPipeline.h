#pragma once
#include "Panel.h"

namespace FlameUI {
    struct Metrics { glm::vec2 position, dimensions; };

    class EventPipeline
    {
    public:
        static void SubmitPanel(const std::string& title, const glm::vec2& position, const glm::vec2& dimensions, const glm::vec4& color);
        static void Prepare();
        static void Execute();

        static std::vector<Panel>& GetPanels() { return s_Panels; }
    private:
        static void InvalidateFocus();
        static void InvalidatePanelPositions(int current_panel_index);
        static Metrics GetPanelMetricsForResizingLeft(const Metrics& panelMetrics, const glm::vec2& cursorPosition);
        static Metrics GetPanelMetricsForResizingRight(const Metrics& panelMetrics, const glm::vec2& cursorPosition);
        static Metrics GetPanelMetricsForResizingBottom(const Metrics& panelMetrics, const glm::vec2& cursorPosition);
        static Metrics GetPanelMetricsForResizingTop(const Metrics& panelMetrics, const glm::vec2& cursorPosition);
    private:
        constexpr static float MIN_PANEL_WIDTH = 20.0f, MIN_PANEL_HEIGHT = TITLE_BAR_HEIGHT + 10.0f;
    private:
        static std::vector<Panel>     s_Panels;
        static std::vector<float>     s_DepthValues;
        static std::vector<uint16_t>  s_PanelPositions;
    };
}