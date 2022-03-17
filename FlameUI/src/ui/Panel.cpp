#include "Panel.h"
#include "../core/Core.h"
#include "../utils/Timer.h"

namespace FlameUI {
    Panel::Panel(const std::string& title, const glm::vec2& position, const glm::vec2& dimensions, const glm::vec4& color)
        : m_PanelName(title),
        m_Position({ position.x, position.y, 0.0f }),
        m_Dimensions(dimensions),
        m_InnerPadding(15, 10),
        m_Color(color),
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
        if ((cursor_pos.x >= m_PanelRect2D.l - 3.0f) && (cursor_pos.x <= m_PanelRect2D.r + 3.0f) && (cursor_pos.y >= m_PanelRect2D.b - 3.0f) && (cursor_pos.y <= m_PanelRect2D.t + 3.0f))
            return true;
        return false;
    }

    void Panel::InvalidateBounds()
    {
        m_PanelRect2D.l = m_Position.x - m_Dimensions.x / 2.0f;
        m_PanelRect2D.r = m_Position.x + m_Dimensions.x / 2.0f;
        m_PanelRect2D.b = m_Position.y - m_Dimensions.y / 2.0f;
        m_PanelRect2D.t = m_Position.y + m_Dimensions.y / 2.0f;
    }

    void Panel::OnDraw()
    {
        Renderer::AddQuad(m_Position, m_Dimensions, m_Color, FL_ELEMENT_TYPE_PANEL_INDEX);
    }

    void Panel::SetZIndex(float z)
    {
        m_Position.z = z;
    }

    void Panel::SetFocus(bool value) { m_IsFocused = value; }

    std::shared_ptr<Panel> Panel::Create(const std::string& title, const glm::vec2& position, const glm::vec2& dimensions, const glm::vec4& color)
    {
        return std::make_shared<Panel>(title, position, dimensions, color);
    }
}
