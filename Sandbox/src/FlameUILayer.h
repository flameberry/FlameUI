#pragma once
#include "FlameUI.h"

namespace Flameberry {
    class FlameUILayer {
    public:
        void OnAttach();
        void OnRender();
    private:
        std::vector<std::shared_ptr<FlameUI::Panel>> m_Panels;
    };
}
