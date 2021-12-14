#pragma once
#include <glm/glm.hpp>
#include "Core.h"

namespace FlameUI {
    class Dockspace
    {
    public:
        Dockspace();
        Dockspace(float left, float right, float bottom, float top);
        void   SetBounds(const Bounds& bounds) { m_Bounds = bounds; }
        Bounds GetBounds()        const { return m_Bounds; }
        bool   IsLeftOccupied()   const { return m_IsLeftOccupied; }
        bool   IsRightOccupied()  const { return m_IsRightOccupied; }
        bool   IsBottomOccupied() const { return m_IsBottomOccupied; }
        bool   IsTopOccupied()    const { return m_IsTopOccupied; }
    private:
        Bounds m_Bounds;
        bool m_IsLeftOccupied, m_IsRightOccupied, m_IsBottomOccupied, m_IsTopOccupied;
    };
}