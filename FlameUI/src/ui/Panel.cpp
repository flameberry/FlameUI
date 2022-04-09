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
        static uint32_t i = 0;
        m_PanelId = i;
        i++;
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
        // Render the panel
        Renderer::AddQuad(m_Position, m_Dimensions, m_Color, FL_ELEMENT_TYPE_PANEL_INDEX, UnitType::PIXEL_UNITS, m_IsFocused);

        // Render all the buttons
        for (auto& buttonInfo : m_ButtonInfos)
            Renderer::AddQuad(glm::vec3{ m_Position.x, m_Position.y, m_Position.z + 0.0000001f }, buttonInfo.dimensions, Renderer::GetThemeInfo().buttonColor, FL_ELEMENT_TYPE_BUTTON_INDEX);

        // Empty the button vector
        m_ButtonInfos.clear();
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

    void Panel::AddButton(const std::string& text, const glm::vec2& position, const glm::vec2& dimensions)
    {
        m_ButtonInfos.emplace_back(text, glm::vec3{ m_Position.x, m_Position.y, m_Position.z + 0.0000000001f }, dimensions);
    }
}
