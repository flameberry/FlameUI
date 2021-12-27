#pragma once
#include "Panel.h"
#include "Dockspace.h"

namespace FlameUI {
    class EventPipeline
    {
    public:
        static void Prepare(const std::vector<Panel>& panels);
        static void Execute();
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
        static std::vector<Dockspace> s_Dockspaces;
    };
}