#include "Panel.h"
#include "Renderer.h"
#include "Core.h"

namespace FlameUI {
    Panel::Panel(const std::string& panelName, const glm::ivec2& position_in_pixels, const glm::ivec2& dimensions_in_pixels, const glm::vec4& color, const std::string& textureFilePath)
        : m_PanelName(panelName), m_Position(position_in_pixels), m_Dimensions(dimensions_in_pixels), m_InnerPadding(15, 10), m_Color(color), m_ZIndex(0.0f)
    {
        m_Bounds.Left = m_Position.x - (m_Dimensions.x / 2);
        m_Bounds.Right = m_Position.x + (m_Dimensions.x / 2);
        m_Bounds.Bottom = m_Position.y - (m_Dimensions.y / 2);
        m_Bounds.Top = m_Position.y + (m_Dimensions.y / 2);
        Renderer::AddQuad(&m_PanelQuadId, FL_QUAD_POS_CENTER, position_in_pixels, dimensions_in_pixels, color, textureFilePath);
    }

    void Panel::AddButton(
        uint32_t* quadId,
        const std::string& title,
        const glm::ivec2& dimensions_in_pixels,
        const glm::vec4& color,
        const std::string& textureFilePath
    )
    {
        static int top_element_position = m_Position.y + ((int)m_Dimensions.y / 2);
        static int bottom_element_position = m_Position.y - ((int)m_Dimensions.y / 2);
        if (top_element_position - bottom_element_position - 2 * m_InnerPadding.y > 0)
        {
            glm::ivec2 dimensions = dimensions_in_pixels;

            uint32_t quad_id;
            bool is_too_big_X = dimensions_in_pixels.x + (2 * m_InnerPadding.x) > m_Dimensions.x;
            bool is_too_big_Y = dimensions_in_pixels.y > top_element_position - bottom_element_position - 2 * m_InnerPadding.y;

            if ((is_too_big_X) && (!is_too_big_Y))
                dimensions.x = m_Dimensions.x - 2 * m_InnerPadding.x;
            else if ((!is_too_big_X) && (is_too_big_Y))
                dimensions.y = top_element_position - bottom_element_position - 2 * m_InnerPadding.y;
            else if (is_too_big_X && is_too_big_Y)
            {
                dimensions.x = m_Dimensions.x - 2 * m_InnerPadding.x;
                dimensions.y = top_element_position - bottom_element_position - 2 * m_InnerPadding.y;
            }

            glm::ivec2 position = {
                m_Position.x - ((int)m_Dimensions.x / 2) + m_InnerPadding.x,
                top_element_position - m_InnerPadding.y - (int)dimensions.y
            };

            m_Buttons.push_back(Button::Create(&quad_id, title, FL_QUAD_POS_BOTTOM_LEFT_VERTEX, position, dimensions, color, textureFilePath));

            if (quadId)
                *quadId = quad_id;

            top_element_position = top_element_position - m_InnerPadding.y - dimensions.y;
        }
        else
        {
            FL_LOG("Panel is full!");
        }
    }

    void Panel::SetDefaultZIndex(float z)
    {
        m_DefaultZIndex = z;
        m_ZIndex = m_DefaultZIndex;
        Renderer::SetQuadZIndex(&m_PanelQuadId, m_DefaultZIndex);
    }

    void Panel::SetFocus(bool value)
    {
        m_IsFocused = value;
        if (m_IsFocused)
        {
            m_ZIndex = 1.0f;
            Renderer::SetQuadZIndex(&m_PanelQuadId, 1.0f);
        }
        else if (!m_IsFocused)
        {
            m_ZIndex = m_DefaultZIndex;
            Renderer::SetQuadZIndex(&m_PanelQuadId, m_ZIndex);
        }
    }

    void Panel::OnUpdate()
    {
        m_Bounds.Left = m_Position.x - (m_Dimensions.x / 2);
        m_Bounds.Right = m_Position.x + (m_Dimensions.x / 2);
        m_Bounds.Bottom = m_Position.y - (m_Dimensions.y / 2);
        m_Bounds.Top = m_Position.y + (m_Dimensions.y / 2);
    }

    void Panel::OnEvent()
    {
        if (m_IsFocused)
        {
            auto window = Renderer::GetUserGLFWwindow();
            glm::vec2 viewportSize = Renderer::GetViewportSize();
            double x, y;
            float cursor_pos_x, cursor_pos_y;
            glfwGetCursorPos(window, &x, &y);
            cursor_pos_x = x - viewportSize.x / 2.0f;
            cursor_pos_y = -y + viewportSize.y / 2.0f;
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
            {
                m_IsFirstTime = true;
                m_IsGrabbed = false;
            }

            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
            {
                if ((cursor_pos_x >= m_Bounds.Left) && (cursor_pos_x <= m_Bounds.Right) && (cursor_pos_y >= m_Bounds.Bottom) && (cursor_pos_y <= m_Bounds.Top))
                {
                    m_IsGrabbed = true;
                    if (m_IsFirstTime)
                    {
                        m_IsFirstTime = false;
                        m_OffsetX = cursor_pos_x - m_Position.x;
                        m_OffsetY = cursor_pos_y - m_Position.y;
                    }
                }

                if (m_IsGrabbed)
                {
                    m_Position = { cursor_pos_x - m_OffsetX , cursor_pos_y - m_OffsetY };
                    Renderer::SetQuadPosition(&m_PanelQuadId, { (int)m_Position.x, (int)m_Position.y });
                }
            }
        }
    }

    std::shared_ptr<Panel> Panel::Create(const std::string& panelName, const glm::ivec2& position_in_pixels, const glm::ivec2& dimensions_in_pixels, const glm::vec4& color, const std::string& textureFilePath)
    {
        return std::make_shared<Panel>(panelName, position_in_pixels, dimensions_in_pixels, color, textureFilePath);
    }
}
