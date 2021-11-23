#include "FlameUI.h"
#define FL_NOT_CLICKED -2
#define FL_CLICKED_ON_NOTHING -1

namespace FlameUI {
    std::vector<std::shared_ptr<Panel>>                           _FlameUI::s_Panels;
    std::vector<float>                                            _FlameUI::s_DepthValues;
    void _FlameUI::AddPanel(const std::shared_ptr<Panel>& panel)
    {
        s_Panels.push_back(panel);
    }

    void _FlameUI::CheckFocus()
    {
        FL_TIMER_SCOPE("CheckFocus()");
        GLFWwindow* window = Renderer::GetUserGLFWwindow();
        glm::vec2 viewportSize = Renderer::GetViewportSize();
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
            double x, y;
            float cursor_pos_x, cursor_pos_y;
            glfwGetCursorPos(window, &x, &y);
            cursor_pos_x = x - viewportSize.x / 2.0f;
            cursor_pos_y = -y + viewportSize.y / 2.0f;
            for (uint16_t i = 0; i < s_Panels.size(); i++)
            {
                Bounds bounds = s_Panels[i]->GetBounds();
                if ((cursor_pos_x >= bounds.Left) && (cursor_pos_x <= bounds.Right) && (cursor_pos_y >= bounds.Bottom) && (cursor_pos_y <= bounds.Top))
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
                    s_Panels[panel_index]->SetFocus(true);
                    s_Panels[panel_index]->SetDefaultZIndex(s_DepthValues.back());
                    first_time = false;
                }

                if ((last_panel_index != FL_CLICKED_ON_NOTHING) && (panel_index != last_panel_index) && (!s_Panels[last_panel_index]->IsGrabbed()))
                {
                    s_Panels[last_panel_index]->SetFocus(false);
                    s_Panels[last_panel_index]->SetDefaultZIndex(s_DepthValues[s_DepthValues.size() - 2]);
                    s_Panels[panel_index]->SetFocus(true);
                    s_Panels[panel_index]->SetDefaultZIndex(s_DepthValues.back());
                    last_panel_index = panel_index;
                }

                if ((last_panel_index == FL_CLICKED_ON_NOTHING) && (!is_grabbed_outside))
                {
                    s_Panels[panel_index]->SetFocus(true);
                    s_Panels[panel_index]->SetDefaultZIndex(s_DepthValues.back());
                    last_panel_index = panel_index;
                }
            }
            else
            {
                if ((last_panel_index != FL_NOT_CLICKED) && (last_panel_index != FL_CLICKED_ON_NOTHING))
                {
                    if (!s_Panels[last_panel_index]->IsGrabbed())
                    {
                        s_Panels[last_panel_index]->SetFocus(false);
                        s_Panels[last_panel_index]->SetDefaultZIndex(s_DepthValues.back());
                        last_panel_index = FL_CLICKED_ON_NOTHING;
                    }
                }
            }
        }
    }

    void _FlameUI::OnUpdate()
    {
        FL_TIMER_SCOPE("_FlameUI_OnUpdate()");
        for (auto& panel : s_Panels)
            panel->OnUpdate();
        CheckFocus();
        for (auto& panel : s_Panels)
            panel->OnEvent();
    }

    void _FlameUI::ShiftDepthValues(uint32_t current_panel_index, uint32_t last_panel_index)
    {
        s_Panels[current_panel_index]->SetDefaultZIndex(s_DepthValues.back());
        s_Panels[last_panel_index]->SetDefaultZIndex(s_DepthValues[s_DepthValues.size() - 2]);
    }

    void _FlameUI::Init()
    {
        if (s_Panels.size())
        {
            float offset = 1.9f / s_Panels.size();
            float start = -0.9f;
            for (auto& panel : s_Panels)
            {
                static uint16_t i = 0;
                float z = start + offset * i;
                s_DepthValues.push_back(z);
                panel->SetDefaultZIndex(z);
                i++;

                FL_LOG("Panel \"{0}\", Focused: {1}", panel->GetPanelName(), panel->IsFocused());
            }
        }
    }
}
