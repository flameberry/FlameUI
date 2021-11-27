#include "Panel.h"
#include "Core.h"
#include "Timer.h"

namespace FlameUI {
    Panel::Panel(const std::string& panelName, const glm::ivec2& position_in_pixels, const glm::ivec2& dimensions_in_pixels, const glm::vec4& color, const std::string& textureFilePath)
        : m_PanelName(panelName), m_Position(position_in_pixels),
        m_Dimensions(dimensions_in_pixels), m_InnerPadding(15, 10),
        m_Color(color), m_ZIndex(0.0f), m_ResizeState(ResizeState::None),
        m_DetailedResizeState(DetailedResizeState::NotResizing), m_CursorState(NULL)
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

    bool Panel::IsHoveredOnPanel()
    {
        glm::vec2& cursor_pos = Renderer::GetCursorPosition();
        if ((cursor_pos.x >= m_Bounds.Left - s_ResizeAreaPadding) && (cursor_pos.x <= m_Bounds.Right + s_ResizeAreaPadding) && (cursor_pos.y >= m_Bounds.Bottom - s_ResizeAreaPadding) && (cursor_pos.y <= m_Bounds.Top + s_ResizeAreaPadding))
            return true;
        return false;
    }

    void Panel::ResizeLeft(const std::array<Vertex*, 4>& ptr_to_vertices, const float& cursor_pos_x)
    {
        if (cursor_pos_x <= m_Position.x - s_MinWidth / 2)
        {
            ptr_to_vertices[0]->position.x = Renderer::ConvertXAxisPixelValueToOpenGLValue(cursor_pos_x);
            ptr_to_vertices[1]->position.x = Renderer::ConvertXAxisPixelValueToOpenGLValue(cursor_pos_x);
        }
        else
        {
            ptr_to_vertices[0]->position.x = Renderer::ConvertXAxisPixelValueToOpenGLValue(m_Position.x - s_MinWidth / 2);
            ptr_to_vertices[1]->position.x = Renderer::ConvertXAxisPixelValueToOpenGLValue(m_Position.x - s_MinWidth / 2);
        }
    }

    void Panel::ResizeRight(const std::array<Vertex*, 4>& ptr_to_vertices, const float& cursor_pos_x)
    {
        if (cursor_pos_x >= m_Position.x + s_MinWidth / 2)
        {
            ptr_to_vertices[2]->position.x = Renderer::ConvertXAxisPixelValueToOpenGLValue(cursor_pos_x);
            ptr_to_vertices[3]->position.x = Renderer::ConvertXAxisPixelValueToOpenGLValue(cursor_pos_x);
        }
        else
        {
            ptr_to_vertices[2]->position.x = Renderer::ConvertXAxisPixelValueToOpenGLValue(m_Position.x + s_MinWidth / 2);
            ptr_to_vertices[3]->position.x = Renderer::ConvertXAxisPixelValueToOpenGLValue(m_Position.x + s_MinWidth / 2);
        }
    }

    void Panel::ResizeBottom(const std::array<Vertex*, 4>& ptr_to_vertices, const float& cursor_pos_y)
    {
        if (cursor_pos_y <= m_Position.y - s_MinHeight / 2)
        {
            ptr_to_vertices[0]->position.y = Renderer::ConvertXAxisPixelValueToOpenGLValue(cursor_pos_y);
            ptr_to_vertices[3]->position.y = Renderer::ConvertXAxisPixelValueToOpenGLValue(cursor_pos_y);
        }
        else
        {
            ptr_to_vertices[0]->position.y = Renderer::ConvertXAxisPixelValueToOpenGLValue(m_Position.y - s_MinHeight / 2);
            ptr_to_vertices[3]->position.y = Renderer::ConvertXAxisPixelValueToOpenGLValue(m_Position.y - s_MinHeight / 2);
        }
    }

    void Panel::ResizeTop(const std::array<Vertex*, 4>& ptr_to_vertices, const float& cursor_pos_y)
    {
        if (cursor_pos_y >= m_Position.y + s_MinHeight / 2)
        {
            ptr_to_vertices[1]->position.y = Renderer::ConvertXAxisPixelValueToOpenGLValue(cursor_pos_y);
            ptr_to_vertices[2]->position.y = Renderer::ConvertXAxisPixelValueToOpenGLValue(cursor_pos_y);
        }
        else
        {
            ptr_to_vertices[1]->position.y = Renderer::ConvertXAxisPixelValueToOpenGLValue(m_Position.y + s_MinHeight / 2);
            ptr_to_vertices[2]->position.y = Renderer::ConvertXAxisPixelValueToOpenGLValue(m_Position.y + s_MinHeight / 2);
        }
    }

    void Panel::SetZIndex(float z)
    {
        m_ZIndex = z;
        Renderer::SetQuadZIndex(&m_PanelQuadId, m_ZIndex);
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
        glm::vec2& cursor_pos = Renderer::GetCursorPosition();

        /// All Location Booleans
        bool is_in_panel_area_x = (cursor_pos.x >= m_Bounds.Left - s_ResizeAreaPadding) && (cursor_pos.x <= m_Bounds.Right + s_ResizeAreaPadding);
        bool is_in_panel_area_y = (cursor_pos.y >= m_Bounds.Bottom - s_ResizeAreaPadding) && (cursor_pos.y <= m_Bounds.Top + s_ResizeAreaPadding);
        m_IsCursorInPanelArea = is_in_panel_area_x && is_in_panel_area_y;

        if (m_IsFocused)
        {
            bool is_on_tl_corner = m_IsCursorInPanelArea && (cursor_pos.x <= m_Bounds.Left + s_ResizeAreaPadding) && (cursor_pos.y >= m_Bounds.Top - s_ResizeAreaPadding);
            bool is_on_br_corner = m_IsCursorInPanelArea && (cursor_pos.x >= m_Bounds.Right - s_ResizeAreaPadding) && (cursor_pos.y <= m_Bounds.Bottom + s_ResizeAreaPadding);
            bool is_on_tr_corner = m_IsCursorInPanelArea && (cursor_pos.x >= m_Bounds.Right - s_ResizeAreaPadding) && (cursor_pos.y >= m_Bounds.Top - s_ResizeAreaPadding);
            bool is_on_bl_corner = m_IsCursorInPanelArea && (cursor_pos.x <= m_Bounds.Left + s_ResizeAreaPadding) && (cursor_pos.y <= m_Bounds.Bottom + s_ResizeAreaPadding);

            bool is_on_tl_or_br_corners = is_on_tl_corner || is_on_br_corner;
            bool is_on_tr_or_bl_corners = is_on_tr_corner || is_on_bl_corner;

            bool is_on_corners = is_on_tr_or_bl_corners || is_on_tl_or_br_corners;

            bool is_on_left_border = m_IsCursorInPanelArea && (!is_on_corners) && (cursor_pos.x <= m_Bounds.Left + s_ResizeAreaPadding);
            bool is_on_right_border = m_IsCursorInPanelArea && (!is_on_corners) && (cursor_pos.x >= m_Bounds.Right - s_ResizeAreaPadding);
            bool is_on_top_border = m_IsCursorInPanelArea && (!is_on_corners) && (cursor_pos.y >= m_Bounds.Top - s_ResizeAreaPadding);
            bool is_on_bottom_border = m_IsCursorInPanelArea && (!is_on_corners) && (cursor_pos.y <= m_Bounds.Bottom + s_ResizeAreaPadding);

            bool is_on_borders = is_on_corners || is_on_left_border || is_on_right_border || is_on_top_border || is_on_bottom_border;

            if (m_ResizeState != ResizeState::Resizing)
            {
                m_ResizeState = ResizeState::None;
                m_DetailedResizeState = DetailedResizeState::NotResizing;
                if (is_on_borders)
                    m_ResizeState = ResizeState::HoveredOnResizeArea;
            }

            if (!m_IsGrabbed)
            {
                if (m_ResizeState == ResizeState::HoveredOnResizeArea)
                {
                    if (is_on_left_border || is_on_right_border)
                        m_CursorState = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
                    else if (is_on_top_border || is_on_bottom_border)
                        m_CursorState = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
                    else if (is_on_tl_or_br_corners)
                        m_CursorState = glfwCreateStandardCursor(GLFW_RESIZE_NWSE_CURSOR);
                    else if (is_on_tr_or_bl_corners)
                        m_CursorState = glfwCreateStandardCursor(GLFW_RESIZE_NESW_CURSOR);
                }
                else if (m_ResizeState == ResizeState::None)
                    m_CursorState = NULL;

                glfwSetCursor(window, m_CursorState);
            }

            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
            {
                if (m_ResizeState != ResizeState::HoveredOnResizeArea)
                    m_ResizeState = ResizeState::None;
            }

            if (m_ResizeState == ResizeState::HoveredOnResizeArea)
            {
                if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
                {
                    if (!m_IsGrabbed)
                    {
                        m_ResizeState = ResizeState::Resizing;
                        if (is_on_left_border)
                        {
                            m_DetailedResizeState = DetailedResizeState::ResizingLeftBorder;
                            m_CursorState = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
                        }
                        else if (is_on_right_border)
                        {
                            m_DetailedResizeState = DetailedResizeState::ResizingRightBorder;
                            m_CursorState = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
                        }
                        else if (is_on_bottom_border)
                        {
                            m_DetailedResizeState = DetailedResizeState::ResizingBottomBorder;
                            m_CursorState = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
                        }
                        else if (is_on_top_border)
                        {
                            m_DetailedResizeState = DetailedResizeState::ResizingTopBorder;
                            m_CursorState = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
                        }
                        else if (is_on_bl_corner)
                        {
                            m_DetailedResizeState = DetailedResizeState::ResizingBottomLeftCorner;
                            m_CursorState = glfwCreateStandardCursor(GLFW_RESIZE_NESW_CURSOR);
                        }
                        else if (is_on_br_corner)
                        {
                            m_DetailedResizeState = DetailedResizeState::ResizingBottomRightCorner;
                            m_CursorState = glfwCreateStandardCursor(GLFW_RESIZE_NWSE_CURSOR);
                        }
                        else if (is_on_tl_corner)
                        {
                            m_DetailedResizeState = DetailedResizeState::ResizingTopLeftCorner;
                            m_CursorState = glfwCreateStandardCursor(GLFW_RESIZE_NWSE_CURSOR);
                        }
                        else if (is_on_tr_corner)
                        {
                            m_DetailedResizeState = DetailedResizeState::ResizingTopRightCorner;
                            m_CursorState = glfwCreateStandardCursor(GLFW_RESIZE_NESW_CURSOR);
                        }
                    }
                }
            }

            if (false)
            {
                ///
                /// Logging Resize State ----------------------------------------------------------------------------------------
                ///

                std::string resize_state = "", detailed_resize_state = "";
                switch (m_ResizeState)
                {
                case ResizeState::None:
                    resize_state = "NONE";
                    break;
                case ResizeState::HoveredOnResizeArea:
                    resize_state = "HOVERED_ON_RESIZE_AREA";
                    break;
                case ResizeState::Resizing:
                    resize_state = "RESIZING";
                    break;
                }

                switch (m_DetailedResizeState)
                {
                case DetailedResizeState::NotResizing:
                    detailed_resize_state = "NONE";
                    break;
                case DetailedResizeState::ResizingLeftBorder:
                    detailed_resize_state = "RESIZING_LEFT_BORDER";
                    break;
                case DetailedResizeState::ResizingRightBorder:
                    detailed_resize_state = "RESIZING_RIGHT_BORDER";
                    break;
                case DetailedResizeState::ResizingBottomBorder:
                    detailed_resize_state = "RESIZING_BOTTOM_BORDER";
                    break;
                case DetailedResizeState::ResizingTopBorder:
                    detailed_resize_state = "RESIZING_TOP_BORDER";
                    break;
                case DetailedResizeState::ResizingBottomLeftCorner:
                    detailed_resize_state = "RESIZING_BOTTOM_LEFT_CORNER";
                    break;
                case DetailedResizeState::ResizingBottomRightCorner:
                    detailed_resize_state = "RESIZING_BOTTOM_RIGHT_CORNER";
                    break;
                case DetailedResizeState::ResizingTopLeftCorner:
                    detailed_resize_state = "RESIZING_TOP_LEFT_CORNER";
                    break;
                case DetailedResizeState::ResizingTopRightCorner:
                    detailed_resize_state = "RESIZING_TOP_RIGHT_CORNER";
                    break;
                }

                FL_LOG("Panel '{0}': Resize State: {1}, Detailed Resize State: {2}", m_PanelName, resize_state, detailed_resize_state);

                ///
                /// --------------------------------------------------------------------------------------------------------------
                ///

            }

            if (m_ResizeState == ResizeState::Resizing)
            {
                auto ptr = Renderer::GetPtrToQuadVertices(&m_PanelQuadId);
                switch (m_DetailedResizeState)
                {
                case DetailedResizeState::ResizingLeftBorder:
                    ResizeLeft(ptr, cursor_pos.x);
                    break;
                case DetailedResizeState::ResizingRightBorder:
                    ResizeRight(ptr, cursor_pos.x);
                    break;
                case DetailedResizeState::ResizingBottomBorder:
                    ResizeBottom(ptr, cursor_pos.y);
                    break;
                case DetailedResizeState::ResizingTopBorder:
                    ResizeTop(ptr, cursor_pos.y);
                    break;
                case DetailedResizeState::ResizingBottomLeftCorner:
                    ResizeLeft(ptr, cursor_pos.x);
                    ResizeBottom(ptr, cursor_pos.y);
                    break;
                case DetailedResizeState::ResizingBottomRightCorner:
                    ResizeRight(ptr, cursor_pos.x);
                    ResizeBottom(ptr, cursor_pos.y);
                    break;
                case DetailedResizeState::ResizingTopLeftCorner:
                    ResizeLeft(ptr, cursor_pos.x);
                    ResizeTop(ptr, cursor_pos.y);
                    break;
                case DetailedResizeState::ResizingTopRightCorner:
                    ResizeRight(ptr, cursor_pos.x);
                    ResizeTop(ptr, cursor_pos.y);
                    break;
                }
            }
        }

        if (m_IsFocused && (m_ResizeState != ResizeState::Resizing))
        {
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
            {
                m_IsFirstTime = true;
                m_IsGrabbed = false;
            }
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
            {
                //
                // if (m_IsCursorInPanelArea)
                //
                if (m_IsCursorInPanelArea && (m_ResizeState != ResizeState::HoveredOnResizeArea))
                {
                    m_IsGrabbed = true;
                    if (m_IsFirstTime)
                    {
                        m_IsFirstTime = false;
                        m_OffsetX = cursor_pos.x - m_Position.x;
                        m_OffsetY = cursor_pos.y - m_Position.y;
                    }
                }
                if (m_IsGrabbed)
                {
                    m_Position = { cursor_pos.x - m_OffsetX , cursor_pos.y - m_OffsetY };
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
