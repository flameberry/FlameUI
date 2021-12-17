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
    private:
        static std::vector<Panel>     s_Panels;
        static std::vector<float>     s_DepthValues;
        static std::vector<uint16_t>  s_PanelPositions;
        static std::vector<Dockspace> s_Dockspaces;
    };
}