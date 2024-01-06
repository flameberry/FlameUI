#include "Panel.h"
#include "core/Core.h"
#include "utils/Timer.h"

#define FL_VERY_SMALL_NUMBER 0.000001f

namespace FlameUI {
    Panel::Panel(const std::string& title, const glm::vec2& position, const glm::vec2& dimensions, const glm::vec4& color)
        : m_PanelName(title),
        m_Position({ position.x, position.y, 0.0f }),
        m_Dimensions(dimensions),
        m_InnerPadding(15, 10),
        m_Color(color),
        m_OffsetOfCursorWhenGrabbed(0.0f),
        m_DetailedResizeState(DetailedResizeState::NotResizing),
        m_DockState(DockState::None),
        m_DetailedDockState(DetailedDockState::NotDocked),
        m_MainState(MainState::None)
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

        // Updating Button Positions
        InvalidateButtonPos();

        // Render all the buttons
        for (auto& button : m_Buttons)
            button.OnDraw();
    }

    void Panel::InvalidateButtonPos()
    {
        for (auto& button : m_Buttons)
        {
            // Write here the code to place buttons as per the space available in the panel
            glm::vec3 buttonPosition = button.GetButtonInfo().position;
            glm::vec2 buttonDimensions = button.GetButtonInfo().dimensions;
            Rect2D buttonRect2D{ buttonPosition.x - buttonDimensions.x / 2.0f, buttonPosition.x + buttonDimensions.x / 2.0f, buttonPosition.y - buttonDimensions.y / 2.0f, buttonPosition.y + buttonDimensions.y / 2.0f };

            buttonPosition.x = m_PanelRect2D.l + m_InnerPadding.x + buttonDimensions.x / 2.0f;
            buttonPosition.y = m_PanelRect2D.t - TITLE_BAR_HEIGHT - m_InnerPadding.y - buttonDimensions.y / 2.0f;
            buttonPosition.z = m_Position.z + FL_VERY_SMALL_NUMBER;
            button.UpdateMetrics(buttonPosition, buttonDimensions);
        }
    }

    void Panel::UpdateMetrics(const glm::vec2& position, const glm::vec2& dimensions)
    {
        m_Position = { position, m_Position.z };
        m_Dimensions = dimensions;
        InvalidateBounds();
    }

    void Panel::AddButton(const std::string& text, const glm::vec2& dimensions)
    {
        m_Buttons.emplace_back(ButtonInfo{ text, glm::vec3{ m_Position.x, m_Position.y, m_Position.z + 0.0000000001f }, dimensions });
    }

    void Panel::SetZIndex(float z) { m_Position.z = z; }
    void Panel::SetFocus(bool value) { m_IsFocused = value; }

    std::shared_ptr<Panel> Panel::Create(const std::string& title, const glm::vec2& position, const glm::vec2& dimensions, const glm::vec4& color)
    {
        return std::make_shared<Panel>(title, position, dimensions, color);
    }
}
