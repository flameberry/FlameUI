#pragma once
#include "Panel.h"

namespace FlameUI {
    class DockingSystem
    {
    public:
        DockingSystem();
        ~DockingSystem();

        void DockLeft(Panel& panel);
        void DockRight(Panel& panel);
        void DockBottom(Panel& panel);
        void DockTop(Panel& panel);
    private:
        Panel* m_LeftPanel, m_RightPanel, m_BottomPanel, m_TopPanel;
    };
}