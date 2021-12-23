#include "Panel.h"
#include "Core.h"
#include "Timer.h"

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

        // Create the main panel
        QuadCreateInfo quad_create_info{};
        quad_create_info.quadId = &m_PanelQuadId;
        quad_create_info.positionType = FL_QUAD_POS_CENTER;
        quad_create_info.position = &m_Position;
        quad_create_info.dimensions = &m_Dimensions;
        quad_create_info.color = &m_Color;

        Renderer::AddQuad(quad_create_info);
    }

    // void Panel::AddButton(const std::string& title, const glm::vec2& dimensions_in_pixels,
    //     const glm::vec4& color, const std::string& textureFilePath)
    // {
    //     static int top_element_position = m_Position.y + ((int)m_Dimensions.y / 2);
    //     static int bottom_element_position = m_Position.y - ((int)m_Dimensions.y / 2);
    //     if (top_element_position - bottom_element_position - 2 * m_InnerPadding.y > 0)
    //     {
    //         glm::vec2 dimensions = dimensions_in_pixels;

    //         uint32_t quad_id;
    //         bool is_too_big_X = dimensions_in_pixels.x + (2 * m_InnerPadding.x) > m_Dimensions.x;
    //         bool is_too_big_Y = dimensions_in_pixels.y > top_element_position - bottom_element_position - 2 * m_InnerPadding.y;

    //         if ((is_too_big_X) && (!is_too_big_Y))
    //             dimensions.x = m_Dimensions.x - 2 * m_InnerPadding.x;
    //         else if ((!is_too_big_X) && (is_too_big_Y))
    //             dimensions.y = top_element_position - bottom_element_position - 2 * m_InnerPadding.y;
    //         else if (is_too_big_X && is_too_big_Y)
    //         {
    //             dimensions.x = m_Dimensions.x - 2 * m_InnerPadding.x;
    //             dimensions.y = top_element_position - bottom_element_position - 2 * m_InnerPadding.y;
    //         }

    //         glm::vec2 position = {
    //             m_Position.x - ((int)m_Dimensions.x / 2) + m_InnerPadding.x,
    //             top_element_position - m_InnerPadding.y - (int)dimensions.y
    //         };

    //         m_Buttons.push_back(Button::Create(title, m_Position, FL_QUAD_POS_BOTTOM_LEFT_VERTEX, position, dimensions, color, textureFilePath));

    //         top_element_position = top_element_position - m_InnerPadding.y - dimensions.y;
    //     }
    //     else
    //     {
    //         FL_LOG("Panel is full!");
    //     }
    // }

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

    void Panel::InvalidateRenderData()
    {
        QuadCreateInfo quad_create_info{};
        quad_create_info.quadId = &m_PanelQuadId;
        quad_create_info.positionType = FL_QUAD_POS_CENTER;
        quad_create_info.position = &m_Position;
        quad_create_info.dimensions = &m_Dimensions;
        quad_create_info.color = &m_Color;
        quad_create_info.zIndex = m_ZIndex;
        Renderer::ChangeQuadVertices(quad_create_info);
    }

    void Panel::SetZIndex(float z)
    {
        m_ZIndex = z;
        Renderer::SetQuadZIndex(&m_PanelQuadId, m_ZIndex);
    }

    void Panel::SetFocus(bool value) { m_IsFocused = value; }

    std::shared_ptr<Panel> Panel::Create(const PanelCreateInfo& panelCreateInfo)
    {
        return std::make_shared<Panel>(panelCreateInfo);
    }
}
