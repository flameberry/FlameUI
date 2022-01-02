#include "Panel.h"
#include "../core/Core.h"
#include "../utils/Timer.h"

namespace FlameUI {
    Panel::Panel(const PanelCreateInfo& panelCreateInfo)
        : m_PanelName(panelCreateInfo.title),
        m_Position(panelCreateInfo.position),
        m_Dimensions(panelCreateInfo.dimensions),
        m_InnerPadding(15, 10),
        m_Color(panelCreateInfo.color),
        m_ZIndex(0.0f),
        m_OffsetOfCursorWhenGrabbed(0.0f),
        m_GrabState(GrabState::NotGrabbed),
        m_ResizeState(ResizeState::None),
        m_DetailedResizeState(DetailedResizeState::NotResizing),
        m_DockState(DockState::None),
        m_DetailedDockState(DetailedDockState::NotDocked)
    {
        // Set the bounds for initialize the panel
        InvalidateBounds();
    }

    bool Panel::IsHoveredOnPanel()
    {
        glm::vec2& cursor_pos = Renderer::GetCursorPosition();
        if ((cursor_pos.x >= m_Bounds.Left - 3.0f) && (cursor_pos.x <= m_Bounds.Right + 3.0f) && (cursor_pos.y >= m_Bounds.Bottom - 3.0f) && (cursor_pos.y <= m_Bounds.Top + 3.0f))
            return true;
        return false;
    }

    void Panel::InvalidateBounds()
    {
        m_Bounds.Left = m_Position.x - m_Dimensions.x / 2.0f;
        m_Bounds.Right = m_Position.x + m_Dimensions.x / 2.0f;
        m_Bounds.Bottom = m_Position.y - m_Dimensions.y / 2.0f;
        m_Bounds.Top = m_Position.y + m_Dimensions.y / 2.0f;
    }

    void Panel::OnDraw()
    {
        Renderer::AddQuad({ m_Position, m_ZIndex }, m_Dimensions, m_Color, FL_ELEMENT_TYPE_PANEL_INDEX);
    }

    void Panel::SetZIndex(float z)
    {
        m_ZIndex = z;
    }

    void Panel::SetFocus(bool value) { m_IsFocused = value; }

    std::shared_ptr<Panel> Panel::Create(const PanelCreateInfo& panelCreateInfo)
    {
        return std::make_shared<Panel>(panelCreateInfo);
    }
}
