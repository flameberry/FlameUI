#include "FlameUI.h"
#define FL_NOT_CLICKED -2
#define FL_CLICKED_ON_NOTHING -1

namespace FlameUI {
    std::vector<std::shared_ptr<Panel>> _FlameUI::s_Panels;
    std::vector<float>                  _FlameUI::s_DepthValues;
    std::vector<uint16_t>               _FlameUI::s_PanelPositions;
    Bounds                              _FlameUI::s_DockspaceBounds;

    void _FlameUI::Init()
    {
        glm::vec2 viewportSize = Renderer::GetViewportSize();
        s_DockspaceBounds = { -viewportSize.x / 2.0f, viewportSize.x / 2.0f, -viewportSize.y / 2.0f, viewportSize.y / 2.0f };
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
                s_Panels[i]->SetZIndex(z);
                FL_INFO("Initial Z value of Panel Index {0} is : {1}", i, z);
                s_PanelPositions[i] = i;
                FL_INFO("Panel with Index {0}, is at position {1}", i, s_PanelPositions[i]);
            }
        }

        for (uint16_t i = 0; i < s_DepthValues.size(); i++)
            FL_INFO("Depth Value at index {0} is {1}", i, s_DepthValues[i]);
    }

    void _FlameUI::OnUpdate()
    {
        FL_TIMER_SCOPE("_FlameUI_OnUpdate()");
        CheckFocus();
        OnDockspaceUpdate();
        for (auto& panel : s_Panels)
        {
            panel->ProvideDockspaceBounds(s_DockspaceBounds);
            panel->OnEvent();
            panel->OnUpdate();
        }
    }

    void _FlameUI::AddPanel(const std::shared_ptr<Panel>& panel)
    {
        s_Panels.push_back(panel);
    }

    void _FlameUI::InvalidatePanelPositions(int current_panel_index)
    {
        for (uint16_t i = 0; i < s_PanelPositions.size(); i++)
        {
            if (s_PanelPositions[i] < s_PanelPositions[current_panel_index])
                s_PanelPositions[i]++;
        }
        s_PanelPositions[current_panel_index] = 0;
        for (uint16_t i = 0; i < s_Panels.size(); i++)
            s_Panels[i]->SetZIndex(s_DepthValues[s_PanelPositions[i]]);
    }

    void _FlameUI::CheckFocus()
    {
        FL_TIMER_SCOPE("CheckFocus()");
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
                if (s_Panels[i]->IsHoveredOnPanel())
                {
                    if (z_index < s_Panels[i]->GetZIndex())
                    {
                        z_index = s_Panels[i]->GetZIndex();
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
                    s_Panels[panel_index]->SetFocus(true);

                    first_time = false;
                }

                if ((last_panel_index != FL_CLICKED_ON_NOTHING) && (panel_index != last_panel_index) && (!s_Panels[last_panel_index]->IsGrabbed()) && (!s_Panels[last_panel_index]->IsResizing()))
                {
                    InvalidatePanelPositions(panel_index);
                    s_Panels[last_panel_index]->SetFocus(false);
                    s_Panels[panel_index]->SetFocus(true);

                    last_panel_index = panel_index;
                }

                if ((last_panel_index == FL_CLICKED_ON_NOTHING) && (!is_grabbed_outside))
                {
                    InvalidatePanelPositions(panel_index);
                    s_Panels[panel_index]->SetFocus(true);

                    last_panel_index = panel_index;
                }
            }
            else
            {
                if ((last_panel_index != FL_NOT_CLICKED) && (last_panel_index != FL_CLICKED_ON_NOTHING))
                {
                    if ((!s_Panels[last_panel_index]->IsGrabbed()) && (!s_Panels[last_panel_index]->IsResizing()))
                    {
                        s_Panels[last_panel_index]->SetFocus(false);
                        last_panel_index = FL_CLICKED_ON_NOTHING;
                    }
                }
            }
        }
    }

    void _FlameUI::OnDockspaceUpdate()
    {
        glm::vec2 viewportSize = Renderer::GetViewportSize();
        s_DockspaceBounds.Left = -viewportSize.x / 2.0f;
        s_DockspaceBounds.Right = viewportSize.x / 2.0f;
        s_DockspaceBounds.Bottom = -viewportSize.y / 2.0f;
        s_DockspaceBounds.Top = viewportSize.y / 2.0f;
        for (auto& panel : s_Panels)
        {
            Bounds panel_dockspace_contribution = panel->GetDockspaceContribution();
            s_DockspaceBounds.Left += panel_dockspace_contribution.Left;
            s_DockspaceBounds.Right -= panel_dockspace_contribution.Right;
            s_DockspaceBounds.Bottom += panel_dockspace_contribution.Bottom;
            s_DockspaceBounds.Top -= panel_dockspace_contribution.Top;
        }
        FL_LOG("Dockspace Bounds are Left: {0}, Right: {1}, Bottom: {2}, Top: {3}", s_DockspaceBounds.Left, s_DockspaceBounds.Right, s_DockspaceBounds.Bottom, s_DockspaceBounds.Top);
    }
}
