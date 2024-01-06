#include "Pipeline.h"
#include <glm/glm.hpp>
#include "renderer/Renderer.h"
#include "core/Input.h"

#define FL_NOT_CLICKED -2
#define FL_CLICKED_ON_NOTHING -1

#define FL_MAX_PANELS 100

namespace FlameUI {
    std::vector<Panel>    Pipeline::s_Panels;
    std::vector<float>    Pipeline::s_DepthValues;
    std::vector<uint16_t> Pipeline::s_PanelPositions;

    void Pipeline::SubmitPanel(const std::string& title, const glm::vec2& position, const glm::vec2& dimensions, const glm::vec4& color)
    {
        s_Panels.emplace_back(title, position, dimensions, color);
    }

    void Pipeline::SubmitButton(const std::string& text, const glm::vec2& dimensions)
    {
        s_Panels.back().AddButton(text, dimensions);
    }

    void Pipeline::Prepare()
    {
        if (!s_Panels.size())
        {
            FL_WARN("No Panels provided to the Event Pipeline!");
            return;
        }
        glm::vec2 viewportSize = Renderer::GetViewportSize();
        const float left = -viewportSize.x / Renderer::GetWindowContentScale().x / 2.0f;
        const float right = -left;
        const float bottom = -viewportSize.y / Renderer::GetWindowContentScale().y / 2.0f;
        const float top = -bottom;
        s_DepthValues.resize(s_Panels.size());
        s_PanelPositions.resize(s_Panels.size());
        if (s_Panels.size())
        {
            float offset = 0.1f / s_Panels.size();
            float start = 0.1f;
            for (int16_t i = s_Panels.size() - 1; i >= 0; i--)
            {
                float z = start - offset * i;
                s_DepthValues[i] = z;
                s_Panels[i].SetZIndex(z);
                s_PanelPositions[i] = i;
            }
        }
    }

    void Pipeline::Execute()
    {
        // Get all variables which will be needed for all the event handling
        GLFWwindow* window = Renderer::GetUserGLFWwindow();
        glm::vec2 viewportSize = Renderer::GetViewportSize();
        const float left = -viewportSize.x / Renderer::GetWindowContentScale().x / 2.0f;
        const float right = -left;
        const float bottom = -viewportSize.y / Renderer::GetWindowContentScale().y / 2.0f;
        const float top = -bottom;
        glm::vec2 cursor_pos = Renderer::GetCursorPosition();

        // Handle window focusing based on mouse events
        InvalidateFocus();

        for (auto& panel : s_Panels)
        {
            // Update Panel bounds, to make them usable for event handling
            panel.InvalidateBounds();

            // Get helper variables to use for event handling
            Rect2D panel_rect_2D = panel.GetPanelRect2D();

            // Get all variables, on which the modifications will be performed according to the events
            glm::vec2 panel_position = panel.GetPosition();
            glm::vec2 panel_dimensions = panel.GetDimensions();

            // Check all button states
            if (panel.GetMainState() == MainState::None)
            {
                if (panel.GetMainState() == MainState::InPanelActivity)
                    panel.SetMainState(MainState::None);

                for (auto& button : panel.GetPanelButtons())
                {
                    if (button.GetPressState() == PressState::Hovered)
                        button.SetPressState(PressState::NotPressed);

                    if (Input::IsCursorInRect2D(button.GetButtonInfo().buttonRect2D))
                    {
                        if (Input::IsMouseButton(GLFW_MOUSE_BUTTON_LEFT, GLFW_PRESS))
                            button.SetPressState(PressState::Pressed);
                        else
                            button.SetPressState(PressState::Hovered);
                    }

                    if (Input::IsMouseButton(GLFW_MOUSE_BUTTON_LEFT, GLFW_RELEASE) && button.GetPressState() != PressState::Hovered)
                        button.SetPressState(PressState::NotPressed);

                    if (button.GetPressState() == PressState::Pressed)
                        panel.SetMainState(MainState::InPanelActivity);
                }
            }

            for (auto& button : panel.GetPanelButtons())
            {
                std::string press_state = "";
                switch (button.GetPressState())
                {
                case PressState::NotPressed: press_state = "Not_Pressed"; break;
                case PressState::Hovered: press_state = "Hovered"; break;
                case PressState::Pressed: press_state = "Pressed"; break;
                }
                FL_LOG("Button: {0}", press_state);
            }
            // -----------------------


            // Handling Events that depend on focus state of the panel
            if (panel.IsFocused())
            {
                // Stage 1: Handle all panel outer-events like grabbing, resizing, docking, etc.
                // The following code should alter the panel position and dimensions according to events recieved

                // Setting some constants
                const float resize_border_offset = 3.0f;

                // Setting Booleans to store the cursor position w.r.t the panel
                bool is_in_panel_area_x = cursor_pos.x >= panel_rect_2D.l - resize_border_offset && cursor_pos.x <= panel_rect_2D.r + resize_border_offset;
                bool is_in_panel_area_y = cursor_pos.y >= panel_rect_2D.b - resize_border_offset && cursor_pos.y <= panel_rect_2D.t + resize_border_offset;
                bool is_in_panel_area = is_in_panel_area_x && is_in_panel_area_y;
                bool is_on_tl_corner = is_in_panel_area && (cursor_pos.x <= panel_rect_2D.l + resize_border_offset) && (cursor_pos.y >= panel_rect_2D.t - resize_border_offset);
                bool is_on_br_corner = is_in_panel_area && (cursor_pos.x >= panel_rect_2D.r - resize_border_offset) && (cursor_pos.y <= panel_rect_2D.b + resize_border_offset);
                bool is_on_tr_corner = is_in_panel_area && (cursor_pos.x >= panel_rect_2D.r - resize_border_offset) && (cursor_pos.y >= panel_rect_2D.t - resize_border_offset);
                bool is_on_bl_corner = is_in_panel_area && (cursor_pos.x <= panel_rect_2D.l + resize_border_offset) && (cursor_pos.y <= panel_rect_2D.b + resize_border_offset);
                bool is_on_tl_or_br_corners = is_on_tl_corner || is_on_br_corner;
                bool is_on_tr_or_bl_corners = is_on_tr_corner || is_on_bl_corner;
                bool is_on_corners = is_on_tr_or_bl_corners || is_on_tl_or_br_corners;
                bool is_on_left_border = is_in_panel_area && (!is_on_corners) && (cursor_pos.x <= panel_rect_2D.l + resize_border_offset);
                bool is_on_right_border = is_in_panel_area && (!is_on_corners) && (cursor_pos.x >= panel_rect_2D.r - resize_border_offset);
                bool is_on_top_border = is_in_panel_area && (!is_on_corners) && (cursor_pos.y >= panel_rect_2D.t - resize_border_offset);
                bool is_on_bottom_border = is_in_panel_area && (!is_on_corners) && (cursor_pos.y <= panel_rect_2D.b + resize_border_offset);
                bool is_on_borders = is_on_corners || is_on_left_border || is_on_right_border || is_on_top_border || is_on_bottom_border;

                bool is_cursor_on_title_bar = is_in_panel_area && cursor_pos.y >= panel_rect_2D.t - TITLE_BAR_HEIGHT;

                bool is_hovered_on_resize_area = false;

                // Set all the states of panel
                // Setting MainState of the panel to Grabbed
                if (!panel.IsResizing())
                {
                    if (Input::IsMouseButton(GLFW_MOUSE_BUTTON_LEFT, GLFW_PRESS))
                    {
                        // See if cursor is inside panel area when mouse button left is pressed
                        if (is_in_panel_area && !is_on_borders && !panel.IsInPanelActivity())
                        {
                            if (!panel.IsGrabbed())
                                panel.StoreOffsetOfCursorFromCenter({ cursor_pos.x - panel_position.x, cursor_pos.y - panel_position.y });
                            panel.SetMainState(MainState::Grabbed);
                        }
                    }

                    if (Input::IsMouseButton(GLFW_MOUSE_BUTTON_LEFT, GLFW_RELEASE))
                    {
                        if (panel.IsGrabbed() && is_cursor_on_title_bar)
                        {
                            if (cursor_pos.x <= left + 100.0f)
                            {
                                panel.SetDockstate(DockState::Docked);
                                panel.SetDetailedDockstate(DetailedDockState::DockedLeft);
                            }
                            else if (cursor_pos.x >= right - 100.0f)
                            {
                                panel.SetDockstate(DockState::Docked);
                                panel.SetDetailedDockstate(DetailedDockState::DockedRight);
                            }
                            else if (cursor_pos.y <= bottom + 50.0f)
                            {
                                panel.SetDockstate(DockState::Docked);
                                panel.SetDetailedDockstate(DetailedDockState::DockedBottom);
                            }
                            else if (cursor_pos.y >= top - 50.0f)
                            {
                                panel.SetDockstate(DockState::Docked);
                                panel.SetDetailedDockstate(DetailedDockState::DockedTop);
                            }
                        }
                        // Set the panel state to not grabbed as the mouse button has been released
                        panel.SetMainState(MainState::None);
                    }
                }

                // Setting all resizing states
                // Resetting Resize State of panel based on previous state
                if (!panel.IsResizing())
                {
                    panel.SetDetailedResizeState(DetailedResizeState::NotResizing);
                    if (is_on_borders)
                        is_hovered_on_resize_area = true;
                }

                // Changing cursor to symbolize resizing, if hovered on the borders of a panel
                GLFWcursor* cursor = NULL;
                if (!panel.IsGrabbed() && is_hovered_on_resize_area)
                {
                    if (is_on_left_border || is_on_right_border)
                        cursor = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
                    else if (is_on_top_border || is_on_bottom_border)
                        cursor = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
                    else if (is_on_tl_or_br_corners)
                        cursor = glfwCreateStandardCursor(GLFW_RESIZE_NWSE_CURSOR);
                    else if (is_on_tr_or_bl_corners)
                        cursor = glfwCreateStandardCursor(GLFW_RESIZE_NESW_CURSOR);
                }

                if (Input::IsMouseButton(GLFW_MOUSE_BUTTON_LEFT, GLFW_RELEASE))
                {
                    if (panel.GetMainState() == MainState::Resizing)
                        panel.SetMainState(MainState::None);
                }

                if (!panel.IsGrabbed() && is_hovered_on_resize_area && Input::IsMouseButton(GLFW_MOUSE_BUTTON_LEFT, GLFW_PRESS) && !panel.IsInPanelActivity())
                {
                    panel.SetMainState(MainState::Resizing);
                    if (is_on_left_border)
                        panel.SetDetailedResizeState(DetailedResizeState::ResizingLeftBorder);
                    else if (is_on_right_border)
                        panel.SetDetailedResizeState(DetailedResizeState::ResizingRightBorder);
                    else if (is_on_bottom_border)
                        panel.SetDetailedResizeState(DetailedResizeState::ResizingBottomBorder);
                    else if (is_on_top_border)
                        panel.SetDetailedResizeState(DetailedResizeState::ResizingTopBorder);
                    else if (is_on_bl_corner)
                        panel.SetDetailedResizeState(DetailedResizeState::ResizingBottomLeftCorner);
                    else if (is_on_br_corner)
                        panel.SetDetailedResizeState(DetailedResizeState::ResizingBottomRightCorner);
                    else if (is_on_tl_corner)
                        panel.SetDetailedResizeState(DetailedResizeState::ResizingTopLeftCorner);
                    else if (is_on_tr_corner)
                        panel.SetDetailedResizeState(DetailedResizeState::ResizingTopRightCorner);
                }

                // Grabbing Event Handling
                if (panel.IsGrabbed())
                    panel_position = { cursor_pos.x - panel.GetOffsetOfCursorFromCenter().x , cursor_pos.y - panel.GetOffsetOfCursorFromCenter().y };
                // -----------------------

                // Resizing Event Handling
                if (panel.IsResizing())
                {
                    Metrics panelMetricsAfterResizing;
                    switch (panel.GetDetailedResizeState())
                    {
                    case DetailedResizeState::ResizingLeftBorder:
                    {
                        panelMetricsAfterResizing = GetPanelMetricsForResizingLeft({ panel_position, panel_dimensions }, cursor_pos);
                        cursor = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
                        break;
                    }
                    case DetailedResizeState::ResizingRightBorder:
                    {
                        panelMetricsAfterResizing = GetPanelMetricsForResizingRight({ panel_position, panel_dimensions }, cursor_pos);
                        cursor = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
                        break;
                    }
                    case DetailedResizeState::ResizingBottomBorder:
                    {
                        panelMetricsAfterResizing = GetPanelMetricsForResizingBottom({ panel_position, panel_dimensions }, cursor_pos);
                        cursor = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
                        break;
                    }
                    case DetailedResizeState::ResizingTopBorder:
                    {
                        panelMetricsAfterResizing = GetPanelMetricsForResizingTop({ panel_position, panel_dimensions }, cursor_pos);
                        cursor = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
                        break;
                    }
                    case DetailedResizeState::ResizingBottomLeftCorner:
                    {
                        panelMetricsAfterResizing = GetPanelMetricsForResizingBottom(GetPanelMetricsForResizingLeft({ panel_position, panel_dimensions }, cursor_pos), cursor_pos);
                        cursor = glfwCreateStandardCursor(GLFW_RESIZE_NESW_CURSOR);
                        break;
                    }
                    case DetailedResizeState::ResizingBottomRightCorner:
                    {
                        panelMetricsAfterResizing = GetPanelMetricsForResizingRight(GetPanelMetricsForResizingBottom({ panel_position, panel_dimensions }, cursor_pos), cursor_pos);
                        cursor = glfwCreateStandardCursor(GLFW_RESIZE_NWSE_CURSOR);
                        break;
                    }
                    case DetailedResizeState::ResizingTopLeftCorner:
                    {
                        panelMetricsAfterResizing = GetPanelMetricsForResizingLeft(GetPanelMetricsForResizingTop({ panel_position, panel_dimensions }, cursor_pos), cursor_pos);
                        cursor = glfwCreateStandardCursor(GLFW_RESIZE_NWSE_CURSOR);
                        break;
                    }
                    case DetailedResizeState::ResizingTopRightCorner:
                    {
                        panelMetricsAfterResizing = GetPanelMetricsForResizingRight(GetPanelMetricsForResizingTop({ panel_position, panel_dimensions }, cursor_pos), cursor_pos);
                        cursor = glfwCreateStandardCursor(GLFW_RESIZE_NESW_CURSOR);
                        break;
                    }
                    case DetailedResizeState::NotResizing:
                        break;
                    }
                    panel_position = panelMetricsAfterResizing.position;
                    panel_dimensions = panelMetricsAfterResizing.dimensions;
                }

                // Finally set the cursor depending upon the resize state
                glfwSetCursor(window, cursor);

                // -----------------------

                // Debug
                // if (panel.IsGrabbed() && is_cursor_on_title_bar && cursor_pos.x <= left + 100.0f)
                // {
                //     Renderer::AddQuad({ left + panel_dimensions.x / 2.0f, 0.0f, 0.0f }, { panel_dimensions.x, top - bottom }, { 0.0f, 1.0f, 1.0f, 0.4f }, FL_ELEMENT_TYPE_GENERAL_INDEX);
                // }
                // ------
                // -------------------------------------------------------------------------------------------
            }

            panel.UpdateMetrics(panel_position, panel_dimensions);
        }

        // Stage 3: Submiting the final position and dimensions of all panels to the Renderer to draw
        for (auto& panel : s_Panels)
            panel.OnDraw();
    }

    void Pipeline::InvalidateFocus()
    {
        GLFWwindow* window = Renderer::GetUserGLFWwindow();
        float z_index = -0.95f;
        int panel_index = FL_NOT_CLICKED;
        static int last_panel_index = FL_NOT_CLICKED;
        static bool is_grabbed_outside = false;

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
        {
            is_grabbed_outside = false;
        }

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        {
            panel_index = FL_CLICKED_ON_NOTHING;
            for (uint16_t i = 0; i < s_Panels.size(); i++)
            {
                if (s_Panels[i].IsHoveredOnPanel())
                {
                    if (z_index < s_Panels[i].GetZIndex())
                    {
                        z_index = s_Panels[i].GetZIndex();
                        panel_index = i;
                    }
                }
            }
            if ((panel_index == FL_CLICKED_ON_NOTHING) && (last_panel_index == FL_CLICKED_ON_NOTHING))
            {
                is_grabbed_outside = true;
            }
        }

        if (panel_index != FL_NOT_CLICKED)
        {
            if (panel_index != FL_CLICKED_ON_NOTHING)
            {
                static bool first_time = true;
                if (first_time)
                {
                    last_panel_index = panel_index;

                    InvalidatePanelPositions(panel_index);
                    s_Panels[panel_index].SetFocus(true);

                    first_time = false;
                }

                if ((last_panel_index != FL_CLICKED_ON_NOTHING) && (panel_index != last_panel_index) && s_Panels[last_panel_index].GetMainState() == MainState::None)
                {
                    InvalidatePanelPositions(panel_index);
                    s_Panels[last_panel_index].SetFocus(false);
                    s_Panels[panel_index].SetFocus(true);

                    last_panel_index = panel_index;
                }

                if ((last_panel_index == FL_CLICKED_ON_NOTHING) && (!is_grabbed_outside))
                {
                    InvalidatePanelPositions(panel_index);
                    s_Panels[panel_index].SetFocus(true);

                    last_panel_index = panel_index;
                }
            }
            else
            {
                if ((last_panel_index != FL_NOT_CLICKED) && (last_panel_index != FL_CLICKED_ON_NOTHING))
                {
                    // if ((!s_Panels[last_panel_index].IsGrabbed()) && (!s_Panels[last_panel_index].IsResizing()))
                    if (s_Panels[last_panel_index].GetMainState() == MainState::None)
                    {
                        s_Panels[last_panel_index].SetFocus(false);
                        last_panel_index = FL_CLICKED_ON_NOTHING;
                    }
                }
            }
        }
    }

    void Pipeline::InvalidatePanelPositions(int current_panel_index)
    {
        for (uint16_t i = 0; i < s_PanelPositions.size(); i++)
        {
            if (s_PanelPositions[i] < s_PanelPositions[current_panel_index])
                s_PanelPositions[i]++;
        }
        s_PanelPositions[current_panel_index] = 0;
        for (uint16_t i = 0; i < s_Panels.size(); i++)
            s_Panels[i].SetZIndex(s_DepthValues[s_PanelPositions[i]]);
    }

    Metrics Pipeline::GetPanelMetricsForResizingLeft(const Metrics& panelMetrics, const glm::vec2& cursorPosition)
    {
        Metrics newPanelMetrics = panelMetrics;

        float right_border_x_coord = newPanelMetrics.position.x + newPanelMetrics.dimensions.x / 2.0f;
        if (right_border_x_coord - cursorPosition.x >= MIN_PANEL_WIDTH)
            newPanelMetrics.dimensions.x = right_border_x_coord - cursorPosition.x;
        else
            newPanelMetrics.dimensions.x = MIN_PANEL_WIDTH;
        newPanelMetrics.position.x = right_border_x_coord - newPanelMetrics.dimensions.x / 2.0f;
        return newPanelMetrics;
    }

    Metrics Pipeline::GetPanelMetricsForResizingRight(const Metrics& panelMetrics, const glm::vec2& cursorPosition)
    {
        Metrics newPanelMetrics = panelMetrics;
        float left_border_x_coord = newPanelMetrics.position.x - newPanelMetrics.dimensions.x / 2.0f;
        if (cursorPosition.x - left_border_x_coord >= MIN_PANEL_WIDTH)
            newPanelMetrics.dimensions.x = cursorPosition.x - left_border_x_coord;
        else
            newPanelMetrics.dimensions.x = MIN_PANEL_WIDTH;
        newPanelMetrics.position.x = left_border_x_coord + newPanelMetrics.dimensions.x / 2.0f;
        return newPanelMetrics;
    }

    Metrics Pipeline::GetPanelMetricsForResizingBottom(const Metrics& panelMetrics, const glm::vec2& cursorPosition)
    {
        Metrics newPanelMetrics = panelMetrics;
        float top_border_y_coord = newPanelMetrics.position.y + newPanelMetrics.dimensions.y / 2.0f;
        if (top_border_y_coord - cursorPosition.y >= MIN_PANEL_HEIGHT)
            newPanelMetrics.dimensions.y = top_border_y_coord - cursorPosition.y;
        else
            newPanelMetrics.dimensions.y = MIN_PANEL_HEIGHT;
        newPanelMetrics.position.y = top_border_y_coord - newPanelMetrics.dimensions.y / 2.0f;
        return newPanelMetrics;
    }

    Metrics Pipeline::GetPanelMetricsForResizingTop(const Metrics& panelMetrics, const glm::vec2& cursorPosition)
    {
        Metrics newPanelMetrics = panelMetrics;
        float bottom_border_y_coord = newPanelMetrics.position.y - newPanelMetrics.dimensions.y / 2.0f;
        if (cursorPosition.y - bottom_border_y_coord >= MIN_PANEL_HEIGHT)
            newPanelMetrics.dimensions.y = cursorPosition.y - bottom_border_y_coord;
        else
            newPanelMetrics.dimensions.y = MIN_PANEL_HEIGHT;
        newPanelMetrics.position.y = bottom_border_y_coord + newPanelMetrics.dimensions.y / 2.0f;
        return newPanelMetrics;
    }
}
