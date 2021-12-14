#include "Dockspace.h"

namespace FlameUI {
    Dockspace::Dockspace()
        : m_Bounds(0.0f), m_IsLeftOccupied(false), m_IsRightOccupied(false), m_IsBottomOccupied(false), m_IsTopOccupied(false)
    {
    }

    Dockspace::Dockspace(float left, float right, float bottom, float top)
        : m_Bounds(left, right, bottom, top), m_IsLeftOccupied(false), m_IsRightOccupied(false), m_IsBottomOccupied(false), m_IsTopOccupied(false)
    {
    }
}