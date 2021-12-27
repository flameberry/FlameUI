#include "EventPipeline.h"
#include <glm/glm.hpp>
#include "../renderer/Renderer.h"

#define FL_NOT_CLICKED -2
#define FL_CLICKED_ON_NOTHING -1

namespace FlameUI {
    std::vector<Panel>     EventPipeline::s_Panels;
    std::vector<float>     EventPipeline::s_DepthValues;
    std::vector<uint16_t>  EventPipeline::s_PanelPositions;
    std::vector<Dockspace> EventPipeline::s_Dockspaces;

    void EventPipeline::Prepare(const std::vector<Panel>& panels)
    {
        if (!panels.size())
        {
            FL_WARN("No Panels provided to the Event Pipeline!");
            return;
        }
        glm::vec2 viewportSize = Renderer::GetViewportSize();
        const float left = -viewportSize.x / 2.0f / Renderer::GetWindowContentScale().x;
        const float right = -left;
        const float bottom = -viewportSize.y / 2.0f / Renderer::GetWindowContentScale().y;
        const float top = -bottom;
        s_Panels = panels;
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
        s_Dockspaces.emplace_back(left, right, bottom, top);
    }

    void EventPipeline::Execute()
    {
        // Get all variables which will be needed for all the event handling
        GLFWwindow* window = Renderer::GetUserGLFWwindow();
        glm::vec2 viewportSize = Renderer::GetViewportSize();
        const float left = -viewportSize.x / 2.0f / Renderer::GetWindowContentScale().x;
        const float right = -left;
        const float bottom = -viewportSize.y / 2.0f / Renderer::GetWindowContentScale().y;
        const float top = -bottom;
        glm::vec2 cursor_pos = Renderer::GetCursorPosition();

        for (auto& dockspace : s_Dockspaces)
            dockspace.SetBounds({ left, right, bottom, top });

        // Handle window focusing based on mouse events
        InvalidateFocus();

        for (auto& panel : s_Panels)
        {
            // Update Panel bounds, to make them usable for event handling
            panel.InvalidateBounds();

            // Get helper variables to use for event handling
            Bounds panel_bounds = panel.GetBounds();

            // Get all variables, on which the modifications will be performed according to the events
            glm::vec2 panel_position = panel.GetPosition();
            glm::vec2 panel_dimensions = panel.GetDimensions();

            // Handling Events that depend on focus state of the panel
            if (panel.IsFocused())
            {
                // Stage 1: Handle all panel outer-events like grabbing, resizing, docking, etc.
                // The following code should alter the panel position and dimensions according to events recieved

                // Setting some constants
                const float resize_border_offset = 3.0f;

                // Setting Booleans to store the cursor position w.r.t the panel
                bool is_in_panel_area_x = cursor_pos.x >= panel_bounds.Left - resize_border_offset && cursor_pos.x <= panel_bounds.Right + resize_border_offset;
                bool is_in_panel_area_y = cursor_pos.y >= panel_bounds.Bottom - resize_border_offset && cursor_pos.y <= panel_bounds.Top + resize_border_offset;
                bool is_in_panel_area = is_in_panel_area_x && is_in_panel_area_y;
                bool is_on_tl_corner = is_in_panel_area && (cursor_pos.x <= panel_bounds.Left + resize_border_offset) && (cursor_pos.y >= panel_bounds.Top - resize_border_offset);
                bool is_on_br_corner = is_in_panel_area && (cursor_pos.x >= panel_bounds.Right - resize_border_offset) && (cursor_pos.y <= panel_bounds.Bottom + resize_border_offset);
                bool is_on_tr_corner = is_in_panel_area && (cursor_pos.x >= panel_bounds.Right - resize_border_offset) && (cursor_pos.y >= panel_bounds.Top - resize_border_offset);
                bool is_on_bl_corner = is_in_panel_area && (cursor_pos.x <= panel_bounds.Left + resize_border_offset) && (cursor_pos.y <= panel_bounds.Bottom + resize_border_offset);
                bool is_on_tl_or_br_corners = is_on_tl_corner || is_on_br_corner;
                bool is_on_tr_or_bl_corners = is_on_tr_corner || is_on_bl_corner;
                bool is_on_corners = is_on_tr_or_bl_corners || is_on_tl_or_br_corners;
                bool is_on_left_border = is_in_panel_area && (!is_on_corners) && (cursor_pos.x <= panel_bounds.Left + resize_border_offset);
                bool is_on_right_border = is_in_panel_area && (!is_on_corners) && (cursor_pos.x >= panel_bounds.Right - resize_border_offset);
                bool is_on_top_border = is_in_panel_area && (!is_on_corners) && (cursor_pos.y >= panel_bounds.Top - resize_border_offset);
                bool is_on_bottom_border = is_in_panel_area && (!is_on_corners) && (cursor_pos.y <= panel_bounds.Bottom + resize_border_offset);
                bool is_on_borders = is_on_corners || is_on_left_border || is_on_right_border || is_on_top_border || is_on_bottom_border;

                bool is_cursor_on_title_bar = is_in_panel_area && cursor_pos.y >= panel_bounds.Top - TITLE_BAR_HEIGHT;

                // Set all the states of panel
                // Setting GrabState and DockState of the panel
                if (!panel.IsResizing())
                {
                    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
                    {
                        // See if cursor is inside panel area when mouse button left is pressed
                        if (is_in_panel_area && !is_on_borders)
                        {
                            if (!panel.IsGrabbed())
                            {
                                if (panel.IsDocked() && is_cursor_on_title_bar)
                                {
                                    switch (panel.GetDetailedDockState())
                                    {
                                    case DetailedDockState::DockedLeft:
                                        s_Dockspaces[0].FreeLeftDock();
                                        break;
                                    case DetailedDockState::DockedRight:
                                        s_Dockspaces[0].FreeRightDock();
                                        break;
                                    case DetailedDockState::DockedBottom:
                                        s_Dockspaces[0].FreeBottomDock();
                                        break;
                                    case DetailedDockState::DockedTop:
                                        s_Dockspaces[0].FreeTopDock();
                                        break;
                                    case DetailedDockState::NotDocked:
                                        break;
                                    }
                                    panel.SetDockstate(DockState::None);
                                    panel.SetDetailedDockstate(DetailedDockState::NotDocked);
                                }
                                panel.StoreOffsetOfCursorFromCenter({ cursor_pos.x - panel_position.x, cursor_pos.y - panel_position.y });
                            }
                            panel.SetGrabState(GrabState::Grabbed);
                        }
                    }

                    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
                    {
                        if (panel.IsGrabbed() && is_cursor_on_title_bar)
                        {
                            if (cursor_pos.x <= left + 100.0f && !s_Dockspaces[0].IsLeftOccupied())
                            {
                                panel.SetDockstate(DockState::Docked);
                                panel.SetDetailedDockstate(DetailedDockState::DockedLeft);
                            }
                            else if (cursor_pos.x >= right - 100.0f && !s_Dockspaces[0].IsRightOccupied())
                            {
                                panel.SetDockstate(DockState::Docked);
                                panel.SetDetailedDockstate(DetailedDockState::DockedRight);
                            }
                            else if (cursor_pos.y <= bottom + 50.0f && !s_Dockspaces[0].IsBottomOccupied())
                            {
                                panel.SetDockstate(DockState::Docked);
                                panel.SetDetailedDockstate(DetailedDockState::DockedBottom);
                            }
                            else if (cursor_pos.y >= top - 50.0f && !s_Dockspaces[0].IsTopOccupied())
                            {
                                panel.SetDockstate(DockState::Docked);
                                panel.SetDetailedDockstate(DetailedDockState::DockedTop);
                            }
                        }
                        // Set the panel state to not grabbed as the mouse button has been released
                        panel.SetGrabState(GrabState::NotGrabbed);
                    }
                }

                // Setting all resizing states
                // Resetting Resize State of panel based on previous state
                if (panel.GetResizeState() != ResizeState::Resizing)
                {
                    panel.SetResizeState(ResizeState::None);
                    panel.SetDetailedResizeState(DetailedResizeState::NotResizing);
                    if (is_on_borders)
                        panel.SetResizeState(ResizeState::HoveredOnResizeArea);
                }

                // Changing cursor to symbolize resizing, if hovered on the borders of a panel
                GLFWcursor* cursor = NULL;
                if (!panel.IsGrabbed() && panel.GetResizeState() == ResizeState::HoveredOnResizeArea)
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

                if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
                {
                    if (panel.GetResizeState() != ResizeState::HoveredOnResizeArea)
                        panel.SetResizeState(ResizeState::None);
                }

                if (!panel.IsGrabbed() && panel.GetResizeState() == ResizeState::HoveredOnResizeArea && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
                {
                    panel.SetResizeState(ResizeState::Resizing);
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
                if (panel.IsGrabbed() && (!panel.IsDocked() || is_cursor_on_title_bar))
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
            }
            // Handling events that are panel focus independent
            // Docking Events Handling
            // IMP: Curently broken
            // TODO: Fix Docking
            if (true)
            {
                if (panel.IsDocked())
                {
                    Metrics panelMetricsAfterDocked;
                    switch (panel.GetDetailedDockState())
                    {
                    case DetailedDockState::DockedLeft:
                    {
                        panelMetricsAfterDocked = s_Dockspaces[0].GetPanelMetricsForDockingLeft(panel_position, panel_dimensions);
                        s_Dockspaces[0].SetLeftPanel(&panel);
                        panel.SetZIndex(0.0f);

                        // When resizing while docking adjacent docked panels must also be resized accordingly
                        if (panel.GetDetailedResizeState() == DetailedResizeState::ResizingRightBorder)
                        {
                            if (s_Dockspaces[0].IsBottomOccupied())
                            {
                                Metrics bottomPanelMetrics = GetPanelMetricsForResizingLeft({ s_Dockspaces[0].GetBottomPanel()->GetPosition(), s_Dockspaces[0].GetBottomPanel()->GetDimensions() }, cursor_pos);
                                s_Dockspaces[0].GetBottomPanel()->SetPosition(bottomPanelMetrics.position);
                                s_Dockspaces[0].GetBottomPanel()->SetDimensions(bottomPanelMetrics.dimensions);
                            }
                            if (s_Dockspaces[0].IsTopOccupied())
                            {
                                Metrics topPanelMetrics = GetPanelMetricsForResizingLeft({ s_Dockspaces[0].GetTopPanel()->GetPosition(), s_Dockspaces[0].GetTopPanel()->GetDimensions() }, cursor_pos);
                                s_Dockspaces[0].GetTopPanel()->SetPosition(topPanelMetrics.position);
                                s_Dockspaces[0].GetTopPanel()->SetDimensions(topPanelMetrics.dimensions);
                            }
                        }
                        break;
                    }
                    case DetailedDockState::DockedRight:
                    {
                        panelMetricsAfterDocked = s_Dockspaces[0].GetPanelMetricsForDockingRight(panel_position, panel_dimensions);
                        s_Dockspaces[0].SetRightPanel(&panel);
                        panel.SetZIndex(0.0f);

                        // When resizing while docking adjacent docked panels must also be resized accordingly
                        if (panel.GetDetailedResizeState() == DetailedResizeState::ResizingLeftBorder)
                        {
                            if (s_Dockspaces[0].IsBottomOccupied())
                            {
                                Metrics bottomPanelMetrics = GetPanelMetricsForResizingRight({ s_Dockspaces[0].GetBottomPanel()->GetPosition(), s_Dockspaces[0].GetBottomPanel()->GetDimensions() }, cursor_pos);
                                s_Dockspaces[0].GetBottomPanel()->SetPosition(bottomPanelMetrics.position);
                                s_Dockspaces[0].GetBottomPanel()->SetDimensions(bottomPanelMetrics.dimensions);
                            }
                            if (s_Dockspaces[0].IsTopOccupied())
                            {
                                Metrics topPanelMetrics = GetPanelMetricsForResizingRight({ s_Dockspaces[0].GetTopPanel()->GetPosition(), s_Dockspaces[0].GetTopPanel()->GetDimensions() }, cursor_pos);
                                s_Dockspaces[0].GetTopPanel()->SetPosition(topPanelMetrics.position);
                                s_Dockspaces[0].GetTopPanel()->SetDimensions(topPanelMetrics.dimensions);
                            }
                        }
                        break;
                    }
                    case DetailedDockState::DockedBottom:
                    {
                        panelMetricsAfterDocked = s_Dockspaces[0].GetPanelMetricsForDockingBottom(panel_position, panel_dimensions);
                        s_Dockspaces[0].SetBottomPanel(&panel);
                        panel.SetZIndex(0.0f);

                        // When resizing while docking adjacent docked panels must also be resized accordingly
                        if (panel.GetDetailedResizeState() == DetailedResizeState::ResizingTopBorder)
                        {
                            if (s_Dockspaces[0].IsLeftOccupied())
                            {
                                Metrics leftPanelMetrics = GetPanelMetricsForResizingBottom({ s_Dockspaces[0].GetLeftPanel()->GetPosition(), s_Dockspaces[0].GetLeftPanel()->GetDimensions() }, cursor_pos);
                                s_Dockspaces[0].GetLeftPanel()->SetPosition(leftPanelMetrics.position);
                                s_Dockspaces[0].GetLeftPanel()->SetDimensions(leftPanelMetrics.dimensions);
                            }
                            if (s_Dockspaces[0].IsRightOccupied())
                            {
                                Metrics rightPanelMetrics = GetPanelMetricsForResizingBottom({ s_Dockspaces[0].GetRightPanel()->GetPosition(), s_Dockspaces[0].GetRightPanel()->GetDimensions() }, cursor_pos);
                                s_Dockspaces[0].GetRightPanel()->SetPosition(rightPanelMetrics.position);
                                s_Dockspaces[0].GetRightPanel()->SetDimensions(rightPanelMetrics.dimensions);
                            }
                        }
                        break;
                    }
                    case DetailedDockState::DockedTop:
                    {
                        panelMetricsAfterDocked = s_Dockspaces[0].GetPanelMetricsForDockingTop(panel_position, panel_dimensions);
                        s_Dockspaces[0].SetTopPanel(&panel);
                        panel.SetZIndex(0.0f);

                        // When resizing while docking adjacent docked panels must also be resized accordingly
                        if (panel.GetDetailedResizeState() == DetailedResizeState::ResizingBottomBorder)
                        {
                            if (s_Dockspaces[0].IsLeftOccupied())
                            {
                                Metrics leftPanelMetrics = GetPanelMetricsForResizingTop({ s_Dockspaces[0].GetLeftPanel()->GetPosition(), s_Dockspaces[0].GetLeftPanel()->GetDimensions() }, cursor_pos);
                                s_Dockspaces[0].GetLeftPanel()->SetPosition(leftPanelMetrics.position);
                                s_Dockspaces[0].GetLeftPanel()->SetDimensions(leftPanelMetrics.dimensions);
                            }
                            if (s_Dockspaces[0].IsRightOccupied())
                            {
                                Metrics rightPanelMetrics = GetPanelMetricsForResizingTop({ s_Dockspaces[0].GetRightPanel()->GetPosition(), s_Dockspaces[0].GetRightPanel()->GetDimensions() }, cursor_pos);
                                s_Dockspaces[0].GetRightPanel()->SetPosition(rightPanelMetrics.position);
                                s_Dockspaces[0].GetRightPanel()->SetDimensions(rightPanelMetrics.dimensions);
                            }
                        }
                        break;
                    }
                    default:
                        break;
                    }
                    panel_position = panelMetricsAfterDocked.position;
                    panel_dimensions = panelMetricsAfterDocked.dimensions;
                }
            }
            // ----------------------------------------------------------------------------------------------

            panel.SetPosition(panel_position);
            panel.SetDimensions(panel_dimensions);
            // -----------------------------------------------------------------------------


            // Stage 2: Handle all panel inner-events like button clicking, scrolling, slider events, etc.
            // -------------------------------------------------------------------------------------------

        }

        // Stage 3: Submiting the final position and dimensions of all panels to the Renderer to draw
        for (auto& panel : s_Panels)
            panel.InvalidateRenderData();
    }

    void EventPipeline::InvalidateFocus()
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

                if ((last_panel_index != FL_CLICKED_ON_NOTHING) && (panel_index != last_panel_index) && (!s_Panels[last_panel_index].IsGrabbed()) && (!s_Panels[last_panel_index].IsResizing()))
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
                    if ((!s_Panels[last_panel_index].IsGrabbed()) && (!s_Panels[last_panel_index].IsResizing()))
                    {
                        s_Panels[last_panel_index].SetFocus(false);
                        last_panel_index = FL_CLICKED_ON_NOTHING;
                    }
                }
            }
        }
    }

    void EventPipeline::InvalidatePanelPositions(int current_panel_index)
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

    Metrics EventPipeline::GetPanelMetricsForResizingLeft(const Metrics& panelMetrics, const glm::vec2& cursorPosition)
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

    Metrics EventPipeline::GetPanelMetricsForResizingRight(const Metrics& panelMetrics, const glm::vec2& cursorPosition)
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

    Metrics EventPipeline::GetPanelMetricsForResizingBottom(const Metrics& panelMetrics, const glm::vec2& cursorPosition)
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

    Metrics EventPipeline::GetPanelMetricsForResizingTop(const Metrics& panelMetrics, const glm::vec2& cursorPosition)
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
