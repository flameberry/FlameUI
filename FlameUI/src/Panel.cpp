#include "Panel.h"
#include "Renderer.h"
#include "Core.h"
#include "Timer.h"

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
    }

    void Panel::OnUpdate()
    {
        std::array<Vertex*, 4> ptr = Renderer::GetPtrToQuadVertices(&m_PanelQuadId);
        float size_x = ptr[3]->position.x - ptr[0]->position.x;
        float size_y = ptr[1]->position.y - ptr[0]->position.y;
        float position_x = ptr[3]->position.x - (ptr[3]->position.x - ptr[0]->position.x) / 2.0f;
        float position_y = ptr[1]->position.y - (ptr[1]->position.y - ptr[0]->position.y) / 2.0f;
        m_Position = Renderer::ConvertOpenGLValuesToPixels({ position_x, position_y });
        m_Dimensions = Renderer::ConvertOpenGLValuesToPixels({ size_x, size_y });
        m_Bounds.Left = m_Position.x - (m_Dimensions.x / 2);
        m_Bounds.Right = m_Position.x + (m_Dimensions.x / 2);
        m_Bounds.Bottom = m_Position.y - (m_Dimensions.y / 2);
        m_Bounds.Top = m_Position.y + (m_Dimensions.y / 2);
    }

    void Panel::OnEvent()
    {
        GLFWwindow* window = Renderer::GetUserGLFWwindow();
        glm::vec2 viewportSize = Renderer::GetViewportSize();
        double x, y;
        float cursor_pos_x, cursor_pos_y;
        static const uint32_t resize_area = 5;
        glfwGetCursorPos(window, &x, &y);
        cursor_pos_x = x - viewportSize.x / 2.0f;
        cursor_pos_y = -y + viewportSize.y / 2.0f;

        bool is_on_left_border = (cursor_pos_x >= m_Bounds.Left - resize_area) && (cursor_pos_x <= m_Bounds.Left + resize_area) && (cursor_pos_y <= m_Bounds.Top + resize_area) && (cursor_pos_y >= m_Bounds.Bottom - resize_area);
        bool is_on_right_border = (cursor_pos_x >= m_Bounds.Right - resize_area) && (cursor_pos_x <= m_Bounds.Right + resize_area) && (cursor_pos_y <= m_Bounds.Top + resize_area) && (cursor_pos_y >= m_Bounds.Bottom - resize_area);
        bool is_on_top_border = (cursor_pos_y >= m_Bounds.Top - resize_area) && (cursor_pos_y <= m_Bounds.Top + resize_area) && (cursor_pos_x <= m_Bounds.Right + resize_area) && (cursor_pos_x >= m_Bounds.Left - resize_area);
        bool is_on_bottom_border = (cursor_pos_y >= m_Bounds.Bottom - resize_area) && (cursor_pos_y <= m_Bounds.Bottom + resize_area) && (cursor_pos_x <= m_Bounds.Right + resize_area) && (cursor_pos_x >= m_Bounds.Left - resize_area);
        bool is_on_tl_or_br_corners = (is_on_left_border && is_on_top_border) || (is_on_bottom_border && is_on_right_border);
        bool is_on_tr_or_bl_corners = (is_on_right_border && is_on_top_border) || (is_on_bottom_border && is_on_left_border);

        int resize_cursor = NULL;
        if (is_on_left_border || is_on_right_border)
            resize_cursor = GLFW_HRESIZE_CURSOR;
        else if (is_on_top_border || is_on_bottom_border)
            resize_cursor = GLFW_VRESIZE_CURSOR;

        if (is_on_tl_or_br_corners)
            resize_cursor = GLFW_RESIZE_NWSE_CURSOR;
        else if (is_on_tr_or_bl_corners)
            resize_cursor = GLFW_RESIZE_NESW_CURSOR;

        glfwSetCursor(window, glfwCreateStandardCursor(resize_cursor));

        bool resizing = false;
        if (resize_cursor != NULL)
            resizing = true;

        static bool is_being_resized = false;

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
            is_being_resized = false;

        if (resizing)
        {
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
            {
                if (is_on_left_border)
                    is_being_resized = true;
            }
        }

        if (is_being_resized)
        {
            auto ptr = Renderer::GetPtrToQuadVertices(&m_PanelQuadId);
            ptr[0]->position.x = Renderer::ConvertPixelsToOpenGLValues({ cursor_pos_x, 0 }).x;
            ptr[1]->position.x = Renderer::ConvertPixelsToOpenGLValues({ cursor_pos_x, 0 }).x;
        }

        if (m_IsFocused && !is_being_resized)
        {
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
