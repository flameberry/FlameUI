#include "Panel.h"
#include "Core.h"
#include "Timer.h"

namespace FlameUI {
    Panel::Panel(const std::string& panelName, const glm::vec2& position_in_pixels, const glm::vec2& dimensions_in_pixels, const glm::vec4& color)
        : m_PanelName(panelName), m_Position(position_in_pixels),
        m_Dimensions(dimensions_in_pixels), m_InnerPadding(15, 10),
        m_Color(color), m_ZIndex(0.0f), m_ResizeState(ResizeState::None),
        m_DetailedResizeState(DetailedResizeState::NotResizing), m_CursorState(NULL),
        m_DockState(DockState::None), m_DetailedDockState(DetailedDockState::NotDocked),
        m_DockingPosition(0.0f)
    {
        m_Bounds.Left = m_Position.x - (m_Dimensions.x / 2);
        m_Bounds.Right = m_Position.x + (m_Dimensions.x / 2);
        m_Bounds.Bottom = m_Position.y - (m_Dimensions.y / 2);
        m_Bounds.Top = m_Position.y + (m_Dimensions.y / 2);
        Renderer::AddQuad(&m_PanelQuadId, FL_QUAD_POS_CENTER, position_in_pixels, dimensions_in_pixels, color);
    }

    void Panel::AddButton(const std::string& title, const glm::vec2& dimensions_in_pixels,
        const glm::vec4& color, const std::string& textureFilePath)
    {
        static int top_element_position = m_Position.y + ((int)m_Dimensions.y / 2);
        static int bottom_element_position = m_Position.y - ((int)m_Dimensions.y / 2);
        if (top_element_position - bottom_element_position - 2 * m_InnerPadding.y > 0)
        {
            glm::vec2 dimensions = dimensions_in_pixels;

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

            glm::vec2 position = {
                m_Position.x - ((int)m_Dimensions.x / 2) + m_InnerPadding.x,
                top_element_position - m_InnerPadding.y - (int)dimensions.y
            };

            m_Buttons.push_back(Button::Create(title, m_Position, FL_QUAD_POS_BOTTOM_LEFT_VERTEX, position, dimensions, color, textureFilePath));

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

    void Panel::InvalidateMetrics()
    {
        m_Position = Renderer::GetQuadPositionInPixels(&m_PanelQuadId);
        m_Dimensions = Renderer::GetQuadDimensionsInPixels(&m_PanelQuadId);
    }

    void Panel::LogResizeState()
    {
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
    }

    void Panel::LogDockState()
    {
        std::string dock_state = "", detailed_dock_state = "";

        switch (m_DockState)
        {
        case DockState::None:
            dock_state = "NONE";
            break;
        case DockState::HoveredOnDockingArea:
            dock_state = "HOVERED_ON_DOCKING_AREA";
            break;
        case DockState::Docked:
            dock_state = "DOCKED";
            break;
        }

        switch (m_DetailedDockState)
        {
        case DetailedDockState::NotDocked:
            detailed_dock_state = "NOT_DOCKED";
            break;
        case DetailedDockState::DockedLeft:
            detailed_dock_state = "DOCKED_LEFT";
            break;
        case DetailedDockState::DockedRight:
            detailed_dock_state = "DOCKED_RIGHT";
            break;
        case DetailedDockState::DockedBottom:
            detailed_dock_state = "DOCKED_BOTTOM";
            break;
        case DetailedDockState::DockedTop:
            detailed_dock_state = "DOCKED_TOP";
            break;
        }

        FL_LOG("Panel {0}: DockState: {1}, Detailed Dock State: {2}", m_PanelName, dock_state, detailed_dock_state);
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

    void Panel::ShowDockingPreview()
    {
        static bool once = true;
        uint32_t quadid = 0;
        if (m_DockState == DockState::HoveredOnDockingArea)
        {
            if (once)
            {
                Renderer::AddQuad(&quadid, FL_QUAD_POS_BOTTOM_LEFT_VERTEX, { 0, 0 }, { m_Dimensions.x, m_Dimensions.y }, { 0.0f, 1.0f, 1.0f, 1.0f });
                once = false;
            }
        }
        else
        {
            Renderer::RemoveQuad(&quadid);
            once = true;
        }
    }

    void Panel::Undock()
    {
        m_IsCurrentlyDocked = false;
        FL_LOG("Undocked panel");
    }

    void Panel::DockLeft()
    {
        if (!m_IsCurrentlyDocked)
        {
            glm::vec2 viewportSize = Renderer::GetViewportSize();
            float half_width = (m_DockspaceBounds.Right - m_DockspaceBounds.Left) / 2.0f;

            m_DockingPosition.x = m_DockspaceBounds.Left + m_Dimensions.x / 2.0f;
            m_DockingPosition.y = m_DockspaceBounds.Bottom + (m_DockspaceBounds.Top - m_DockspaceBounds.Bottom) / 2.0f;

            if (m_Dimensions.x > half_width)
                m_Dimensions.x = half_width;
            m_Dimensions.y = m_DockspaceBounds.Top - m_DockspaceBounds.Bottom;
            m_IsCurrentlyDocked = true;
        }
        m_Position = m_DockingPosition;
        Renderer::ChangeQuadVertices(&m_PanelQuadId, FL_QUAD_POS_CENTER, m_Position, m_Dimensions, m_Color);
        SetZIndex(m_ZIndex);
    }

    void Panel::DockRight()
    {
        if (!m_IsCurrentlyDocked)
        {
            glm::vec2 viewportSize = Renderer::GetViewportSize();
            float half_width = (m_DockspaceBounds.Right - m_DockspaceBounds.Left) / 2.0f;

            m_DockingPosition.x = m_DockspaceBounds.Right - m_Dimensions.x / 2.0f;
            m_DockingPosition.y = m_DockspaceBounds.Bottom + (m_DockspaceBounds.Top - m_DockspaceBounds.Bottom) / 2.0f;

            if (m_Dimensions.x > half_width)
                m_Dimensions.x = half_width;
            m_Dimensions.y = m_DockspaceBounds.Top - m_DockspaceBounds.Bottom;
            m_IsCurrentlyDocked = true;
        }
        m_Position = m_DockingPosition;
        Renderer::ChangeQuadVertices(&m_PanelQuadId, FL_QUAD_POS_CENTER, m_Position, m_Dimensions, m_Color);
        SetZIndex(m_ZIndex);
    }

    void Panel::DockBottom()
    {
        if (!m_IsCurrentlyDocked)
        {
            glm::vec2 viewportSize = Renderer::GetViewportSize();
            float half_height = (m_DockspaceBounds.Top - m_DockspaceBounds.Bottom) / 2.0f;
            if (m_Dimensions.y > half_height)
                m_Dimensions.y = half_height;

            m_Dimensions.x = m_DockspaceBounds.Right - m_DockspaceBounds.Left;
            m_DockingPosition.y = m_DockspaceBounds.Bottom + m_Dimensions.y / 2.0f;
            m_DockingPosition.x = m_DockspaceBounds.Left + (m_DockspaceBounds.Right - m_DockspaceBounds.Left) / 2.0f;
            m_IsCurrentlyDocked = true;
        }
        m_Position = m_DockingPosition;
        Renderer::ChangeQuadVertices(&m_PanelQuadId, FL_QUAD_POS_CENTER, m_Position, m_Dimensions, m_Color);
        SetZIndex(m_ZIndex);
    }

    void Panel::DockTop()
    {
        if (!m_IsCurrentlyDocked)
        {
            glm::vec2 viewportSize = Renderer::GetViewportSize();
            float half_height = (m_DockspaceBounds.Top - m_DockspaceBounds.Bottom) / 2.0f;

            if (m_Dimensions.y > half_height)
                m_Dimensions.y = half_height;

            m_Dimensions.x = m_DockspaceBounds.Right - m_DockspaceBounds.Left;
            m_DockingPosition.y = m_DockspaceBounds.Top - m_Dimensions.y / 2.0f;
            m_DockingPosition.x = m_DockspaceBounds.Left + (m_DockspaceBounds.Right - m_DockspaceBounds.Left) / 2.0f;
            m_IsCurrentlyDocked = true;
        }
        m_Position = m_DockingPosition;
        Renderer::ChangeQuadVertices(&m_PanelQuadId, FL_QUAD_POS_CENTER, m_Position, m_Dimensions, m_Color);
        SetZIndex(m_ZIndex);
    }

    void Panel::SetZIndex(float z)
    {
        m_ZIndex = z;
        Renderer::SetQuadZIndex(&m_PanelQuadId, m_ZIndex);
    }

    void Panel::SetFocus(bool value) { m_IsFocused = value; }

    Bounds Panel::GetDockspaceContribution() const
    {
        if (m_DockState == DockState::Docked)
        {
            switch (m_DetailedDockState)
            {
            case DetailedDockState::DockedLeft:
                return { m_Dimensions.x, 0.0f, 0.0f, 0.0f };
                break;
            case DetailedDockState::DockedRight:
                return { 0.0f, m_Dimensions.x, 0.0f, 0.0f };
                break;
            case DetailedDockState::DockedBottom:
                return { 0.0f, 0.0f, m_Dimensions.y, 0.0f };
                break;
            case DetailedDockState::DockedTop:
                return { 0.0f, 0.0f, 0.0f, m_Dimensions.y };
                break;
            default:
                break;
            }
        }
        return { 0.0f, 0.0f, 0.0f, 0.0f };
    }

    void Panel::OnUpdate()
    {
        InvalidateMetrics();

        m_Bounds.Left = m_Position.x - (m_Dimensions.x / 2);
        m_Bounds.Right = m_Position.x + (m_Dimensions.x / 2);
        m_Bounds.Bottom = m_Position.y - (m_Dimensions.y / 2);
        m_Bounds.Top = m_Position.y + (m_Dimensions.y / 2);

        for (auto& button : m_Buttons)
            button->OnUpdate(m_Position, m_ZIndex);
    }

    void Panel::OnEvent()
    {
        GLFWwindow* window = Renderer::GetUserGLFWwindow();
        glm::vec2& cursor_pos = Renderer::GetCursorPosition();
        glm::vec2 viewportSize = Renderer::GetViewportSize();
        float left = -viewportSize.x / 2.0f;
        float right = viewportSize.x / 2.0f;
        float bottom = -viewportSize.y / 2.0f;
        float top = viewportSize.y / 2.0f;

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
                LogResizeState();

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
                default:
                    break;
                }
                InvalidateMetrics();
            }
        }

        if (m_IsFocused && (m_ResizeState != ResizeState::Resizing))
        {
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
            {
                if (m_IsGrabbed)
                {
                    if (cursor_pos.x <= m_DockspaceBounds.Left + 100.0f)
                    {
                        m_DockState = DockState::Docked;
                        m_DetailedDockState = DetailedDockState::DockedLeft;
                    }
                    else if (cursor_pos.x >= m_DockspaceBounds.Right - 100.0f)
                    {
                        m_DockState = DockState::Docked;
                        m_DetailedDockState = DetailedDockState::DockedRight;
                    }
                    else if (cursor_pos.y <= m_DockspaceBounds.Bottom + 100.0f)
                    {
                        m_DockState = DockState::Docked;
                        m_DetailedDockState = DetailedDockState::DockedBottom;
                    }
                    else if (cursor_pos.y >= m_DockspaceBounds.Top - 100.0f)
                    {
                        m_DockState = DockState::Docked;
                        m_DetailedDockState = DetailedDockState::DockedTop;
                    }
                }
                else if (m_DockState == DockState::HoveredOnDockingArea)
                {
                    m_DockState = DockState::None;
                }

                m_IsFirstTime = true;
                m_IsGrabbed = false;
            }

            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
            {
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
                    if (m_DockState == DockState::Docked)
                    {
                        m_DockState = DockState::None;
                        m_DetailedDockState = DetailedDockState::NotDocked;
                        Undock();
                    }

                    /// Broken if else statement
                    if (cursor_pos.x <= left + 100.0f)
                        m_DockState = DockState::HoveredOnDockingArea;
                    else
                        m_DockState = DockState::None;

                    m_Position = { cursor_pos.x - m_OffsetX , cursor_pos.y - m_OffsetY };
                    Renderer::SetQuadPosition(&m_PanelQuadId, { (int)m_Position.x, (int)m_Position.y });
                }
            }
        }

        /// Currently broken as the function it uses to remove quads is broken
        if (false)
            ShowDockingPreview();

        if (true)
        {
            if (m_DockState == DockState::Docked)
            {
                switch (m_DetailedDockState)
                {
                case DetailedDockState::DockedLeft:
                    DockLeft();
                    break;
                case DetailedDockState::DockedRight:
                    DockRight();
                    break;
                case DetailedDockState::DockedBottom:
                    DockBottom();
                    break;
                case DetailedDockState::DockedTop:
                    DockTop();
                    break;
                case DetailedDockState::NotDocked:
                    break;
                }
            }
        }

        if (false)
            LogDockState();
    }

    std::shared_ptr<Panel> Panel::Create(const std::string& panelName, const glm::vec2& position_in_pixels,
        const glm::vec2& dimensions_in_pixels, const glm::vec4& color)
    {
        return std::make_shared<Panel>(panelName, position_in_pixels, dimensions_in_pixels, color);
    }
}
