#include "DockingSystem.h"
#include "Pipeline.h"

namespace FlameUI {
    DockingSystem::DockingSystem()
    {
    }

    DockingSystem::~DockingSystem()
    {
    }

    void DockingSystem::DockLeft(Panel& panel)
    {
        for (auto& _panel : Pipeline::GetPanels())
        {
            if (_panel.IsDocked())
            {
                Rect2D panelRect2D = _panel.GetPanelRect2D();
            }
        }
    }

    void DockingSystem::DockRight(Panel& panel)
    {
    }
    void DockingSystem::DockBottom(Panel& panel)
    {
    }
    void DockingSystem::DockTop(Panel& panel)
    {
    }
}