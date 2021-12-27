#pragma once
#include <glm/glm.hpp>
#include "../core/Core.h"
#include "Panel.h"

namespace FlameUI {
    struct Metrics { glm::vec2 position, dimensions; };
    class Dockspace
    {
    public:
        Dockspace();
        Dockspace(float left, float right, float bottom, float top);

        void    SetBounds(const Bounds& bounds) { m_Bounds = bounds; }
        Bounds  GetBounds()        const { return m_Bounds; }
        void    FreeLeftDock();
        void    FreeRightDock();
        void    FreeBottomDock();
        void    FreeTopDock();
        bool    IsLeftOccupied()   const { return m_IsLeftOccupied; }
        bool    IsRightOccupied()  const { return m_IsRightOccupied; }
        bool    IsBottomOccupied() const { return m_IsBottomOccupied; }
        bool    IsTopOccupied()    const { return m_IsTopOccupied; }
        Metrics GetPanelMetricsForDockingLeft(const glm::vec2& panel_position, const glm::vec2& panel_dimensions);
        Metrics GetPanelMetricsForDockingRight(const glm::vec2& panel_position, const glm::vec2& panel_dimensions);
        Metrics GetPanelMetricsForDockingBottom(const glm::vec2& panel_position, const glm::vec2& panel_dimensions);
        Metrics GetPanelMetricsForDockingTop(const glm::vec2& panel_position, const glm::vec2& panel_dimensions);
        void    SetLeftPanel(Panel* panel);
        void    SetRightPanel(Panel* panel);
        void    SetBottomPanel(Panel* panel);
        void    SetTopPanel(Panel* panel);

        Panel* GetLeftPanel() const { return m_LeftPanel; };
        Panel* GetRightPanel() const { return m_RightPanel; };
        Panel* GetBottomPanel() const { return m_BottomPanel; };
        Panel* GetTopPanel() const { return m_TopPanel; };
    private:
        enum class DockingSlot { Left = 0, Right, Bottom, Top };
    private:
        void    InvalidatePanelPositionsWhenDockingInSlot(const DockingSlot& slot, const glm::vec2& panel_dimensions);
    private:
        Panel* m_LeftPanel;
        Panel* m_RightPanel;
        Panel* m_BottomPanel;
        Panel* m_TopPanel;
        Bounds m_Bounds;
        bool m_IsLeftOccupied, m_IsRightOccupied, m_IsBottomOccupied, m_IsTopOccupied;
    };
}