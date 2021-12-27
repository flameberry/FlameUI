#include "Dockspace.h"

namespace FlameUI {
    Dockspace::Dockspace()
        : m_LeftPanel(nullptr), m_RightPanel(nullptr), m_BottomPanel(nullptr), m_TopPanel(nullptr), m_Bounds(0.0f), m_IsLeftOccupied(false), m_IsRightOccupied(false), m_IsBottomOccupied(false), m_IsTopOccupied(false)
    {
    }

    Dockspace::Dockspace(float left, float right, float bottom, float top)
        : m_LeftPanel(nullptr), m_RightPanel(nullptr), m_BottomPanel(nullptr), m_TopPanel(nullptr), m_Bounds(left, right, bottom, top), m_IsLeftOccupied(false), m_IsRightOccupied(false), m_IsBottomOccupied(false), m_IsTopOccupied(false)
    {
    }

    void Dockspace::FreeLeftDock()
    {
        m_IsLeftOccupied = false;
        m_LeftPanel = nullptr;
    }

    void Dockspace::FreeRightDock()
    {
        m_IsRightOccupied = false;
        m_RightPanel = nullptr;
    }

    void Dockspace::FreeBottomDock()
    {
        m_IsBottomOccupied = false;
        m_BottomPanel = nullptr;
    }

    void Dockspace::FreeTopDock()
    {
        m_IsTopOccupied = false;
        m_TopPanel = nullptr;
    }

    void Dockspace::InvalidatePanelPositionsWhenDockingInSlot(const DockingSlot& slot, const glm::vec2& panel_dimensions)
    {
        switch (slot)
        {
        case DockingSlot::Left:
        {
            if (m_IsBottomOccupied)
            {
                glm::vec2 bottom_panel_dimensions = m_BottomPanel->GetDimensions();
                glm::vec2 bottom_panel_position = m_BottomPanel->GetPosition();
                float bottom_panel_right_border_x_coord = bottom_panel_position.x + bottom_panel_dimensions.x / 2.0f;
                bottom_panel_dimensions.x = bottom_panel_right_border_x_coord - m_Bounds.Left - panel_dimensions.x;
                bottom_panel_position.x = bottom_panel_right_border_x_coord - bottom_panel_dimensions.x / 2.0f;
                m_BottomPanel->SetPosition(bottom_panel_position);
                m_BottomPanel->SetDimensions(bottom_panel_dimensions);
            }
            if (m_IsTopOccupied)
            {
                glm::vec2 top_panel_dimensions = m_TopPanel->GetDimensions();
                glm::vec2 top_panel_position = m_TopPanel->GetPosition();
                float top_panel_right_border_x_coord = top_panel_position.x + top_panel_dimensions.x / 2.0f;
                top_panel_dimensions.x = top_panel_right_border_x_coord - m_Bounds.Left - panel_dimensions.x;
                top_panel_position.x = top_panel_right_border_x_coord - top_panel_dimensions.x / 2.0f;
                m_TopPanel->SetPosition(top_panel_position);
                m_TopPanel->SetDimensions(top_panel_dimensions);
            }
            break;
        }
        case DockingSlot::Right:
        {
            if (m_IsBottomOccupied)
            {
                glm::vec2 bottom_panel_dimensions = m_BottomPanel->GetDimensions();
                glm::vec2 bottom_panel_position = m_BottomPanel->GetPosition();
                float bottom_panel_left_border_x_coord = bottom_panel_position.x - bottom_panel_dimensions.x / 2.0f;
                bottom_panel_dimensions.x = m_Bounds.Right - bottom_panel_left_border_x_coord - panel_dimensions.x;
                bottom_panel_position.x = bottom_panel_left_border_x_coord + bottom_panel_dimensions.x / 2.0f;
                m_BottomPanel->SetPosition(bottom_panel_position);
                m_BottomPanel->SetDimensions(bottom_panel_dimensions);
            }
            if (m_IsTopOccupied)
            {
                glm::vec2 top_panel_dimensions = m_TopPanel->GetDimensions();
                glm::vec2 top_panel_position = m_TopPanel->GetPosition();
                float top_panel_left_border_x_coord = top_panel_position.x - top_panel_dimensions.x / 2.0f;
                top_panel_dimensions.x = m_Bounds.Right - top_panel_left_border_x_coord - panel_dimensions.x;
                top_panel_position.x = top_panel_left_border_x_coord + top_panel_dimensions.x / 2.0f;
                m_TopPanel->SetPosition(top_panel_position);
                m_TopPanel->SetDimensions(top_panel_dimensions);
            }
            break;
        }
        case DockingSlot::Bottom:
        {
            if (m_IsLeftOccupied)
            {
                glm::vec2 left_panel_dimensions = m_LeftPanel->GetDimensions();
                glm::vec2 left_panel_position = m_LeftPanel->GetPosition();
                float left_panel_top_border_y_coord = left_panel_position.y + left_panel_dimensions.y / 2.0f;
                left_panel_dimensions.y = left_panel_top_border_y_coord - m_Bounds.Bottom - panel_dimensions.y;
                left_panel_position.y = left_panel_top_border_y_coord - left_panel_dimensions.y / 2.0f;
                m_LeftPanel->SetPosition(left_panel_position);
                m_LeftPanel->SetDimensions(left_panel_dimensions);
            }
            if (m_IsRightOccupied)
            {
                glm::vec2 right_panel_dimensions = m_RightPanel->GetDimensions();
                glm::vec2 right_panel_position = m_RightPanel->GetPosition();
                float right_panel_top_border_y_coord = right_panel_position.y + right_panel_dimensions.y / 2.0f;
                right_panel_dimensions.y = right_panel_top_border_y_coord - m_Bounds.Bottom - panel_dimensions.y;
                right_panel_position.y = right_panel_top_border_y_coord - right_panel_dimensions.y / 2.0f;
                m_RightPanel->SetPosition(right_panel_position);
                m_RightPanel->SetDimensions(right_panel_dimensions);
            }
            break;
        }
        case DockingSlot::Top:
        {
            if (m_IsLeftOccupied)
            {
                glm::vec2 left_panel_dimensions = m_LeftPanel->GetDimensions();
                glm::vec2 left_panel_position = m_LeftPanel->GetPosition();
                float left_panel_bottom_border_y_coord = left_panel_position.y - left_panel_dimensions.y / 2.0f;
                left_panel_dimensions.y = m_Bounds.Top - left_panel_bottom_border_y_coord - panel_dimensions.y;
                left_panel_position.y = left_panel_bottom_border_y_coord + left_panel_dimensions.y / 2.0f;
                m_LeftPanel->SetPosition(left_panel_position);
                m_LeftPanel->SetDimensions(left_panel_dimensions);
            }
            if (m_IsRightOccupied)
            {
                glm::vec2 right_panel_dimensions = m_RightPanel->GetDimensions();
                glm::vec2 right_panel_position = m_RightPanel->GetPosition();
                float right_panel_bottom_border_y_coord = right_panel_position.y - right_panel_dimensions.y / 2.0f;
                right_panel_dimensions.y = m_Bounds.Top - right_panel_bottom_border_y_coord - panel_dimensions.y;
                right_panel_position.y = right_panel_bottom_border_y_coord + right_panel_dimensions.y / 2.0f;
                m_RightPanel->SetPosition(right_panel_position);
                m_RightPanel->SetDimensions(right_panel_dimensions);
            }
            break;
        }
        }
    }

    Metrics Dockspace::GetPanelMetricsForDockingLeft(const glm::vec2& panel_position, const glm::vec2& panel_dimensions)
    {
        if (!m_IsLeftOccupied)
        {
            glm::vec new_panel_dimensions = panel_dimensions;
            new_panel_dimensions.y = m_Bounds.Top - m_Bounds.Bottom;
            if (new_panel_dimensions.x > (m_Bounds.Right - m_Bounds.Left) / 2.0f)
                new_panel_dimensions.x = (m_Bounds.Right - m_Bounds.Left) / 2.0f;
            glm::vec2 new_panel_position = { m_Bounds.Left + new_panel_dimensions.x / 2.0f, m_Bounds.Bottom + new_panel_dimensions.y / 2.0f };

            // Invalidate panel positions to adjust when the given panel is docked left
            InvalidatePanelPositionsWhenDockingInSlot(DockingSlot::Left, new_panel_dimensions);

            return { new_panel_position, new_panel_dimensions };
        }

        glm::vec2 new_panel_dimensions = panel_dimensions;
        glm::vec2 new_panel_position = panel_position;

        new_panel_position.x = m_Bounds.Left + new_panel_dimensions.x / 2.0f;

        if (m_IsTopOccupied)
        {
            // glm::vec2 top_panel_position = m_TopPanel->GetPosition();
            // glm::vec2 top_panel_dimensions = m_TopPanel->GetDimensions();
            // if (top_panel_position.x - top_panel_dimensions.x / 2.0f == m_Bounds.Left)
            // {
            //     float panel_bottom_border_y_coord = new_panel_position.y - new_panel_dimensions.y / 2.0f;
            //     new_panel_dimensions.y = m_Bounds.Top - panel_bottom_border_y_coord - top_panel_dimensions.y;
            //     new_panel_position.y = panel_bottom_border_y_coord + new_panel_dimensions.y / 2.0f;
            // }
        }
        else
        {
            float panel_bottom_border_y_coord = new_panel_position.y - new_panel_dimensions.y / 2.0f;
            new_panel_dimensions.y = m_Bounds.Top - panel_bottom_border_y_coord;
            new_panel_position.y = panel_bottom_border_y_coord + new_panel_dimensions.y / 2.0f;
        }

        if (m_IsBottomOccupied)
        {
            // glm::vec2 bottom_panel_position = m_BottomPanel->GetPosition();
            // glm::vec2 bottom_panel_dimensions = m_BottomPanel->GetDimensions();
            // if (bottom_panel_position.x - bottom_panel_dimensions.x / 2.0f == m_Bounds.Left)
            // {
            //     float panel_top_border_y_coord = new_panel_position.y + new_panel_dimensions.y / 2.0f;
            //     new_panel_dimensions.y = panel_top_border_y_coord - m_Bounds.Bottom - bottom_panel_dimensions.y;
            //     new_panel_position.y = panel_top_border_y_coord - new_panel_dimensions.y / 2.0f;
            // }
        }
        else
        {
            float panel_top_border_y_coord = new_panel_position.y + new_panel_dimensions.y / 2.0f;
            new_panel_dimensions.y = panel_top_border_y_coord - m_Bounds.Bottom;
            new_panel_position.y = panel_top_border_y_coord - new_panel_dimensions.y / 2.0f;
        }
        return { new_panel_position, new_panel_dimensions };
    }

    Metrics Dockspace::GetPanelMetricsForDockingRight(const glm::vec2& panel_position, const glm::vec2& panel_dimensions)
    {
        if (!m_IsRightOccupied)
        {
            glm::vec2 new_panel_dimensions = panel_dimensions;
            new_panel_dimensions.y = m_Bounds.Top - m_Bounds.Bottom;
            if (new_panel_dimensions.x > (m_Bounds.Right - m_Bounds.Left) / 2.0f)
                new_panel_dimensions.x = (m_Bounds.Right - m_Bounds.Left) / 2.0f;
            glm::vec2 new_panel_position = { m_Bounds.Right - new_panel_dimensions.x / 2.0f, m_Bounds.Bottom + new_panel_dimensions.y / 2.0f };

            // Invalidate panel positions to adjust when the given panel is docked right
            InvalidatePanelPositionsWhenDockingInSlot(DockingSlot::Right, new_panel_dimensions);

            return { new_panel_position, new_panel_dimensions };
        }

        glm::vec2 new_panel_dimensions = panel_dimensions;
        glm::vec2 new_panel_position = panel_position;

        new_panel_position.x = m_Bounds.Right - new_panel_dimensions.x / 2.0f;

        if (m_IsTopOccupied)
        {
            // glm::vec2 top_panel_position = m_TopPanel->GetPosition();
            // glm::vec2 top_panel_dimensions = m_TopPanel->GetDimensions();
            // if (top_panel_position.x + top_panel_dimensions.x / 2.0f == m_Bounds.Right)
            // {
            //     float panel_bottom_border_y_coord = new_panel_position.y - new_panel_dimensions.y / 2.0f;
            //     new_panel_dimensions.y = m_Bounds.Top - panel_bottom_border_y_coord - top_panel_dimensions.y;
            //     new_panel_position.y = panel_bottom_border_y_coord + new_panel_dimensions.y / 2.0f;
            // }
        }
        else
        {
            float panel_bottom_border_y_coord = new_panel_position.y - new_panel_dimensions.y / 2.0f;
            new_panel_dimensions.y = m_Bounds.Top - panel_bottom_border_y_coord;
            new_panel_position.y = panel_bottom_border_y_coord + new_panel_dimensions.y / 2.0f;
        }

        if (m_IsBottomOccupied)
        {
            glm::vec2 bottom_panel_position = m_BottomPanel->GetPosition();
            glm::vec2 bottom_panel_dimensions = m_BottomPanel->GetDimensions();
            // if (bottom_panel_position.x + bottom_panel_dimensions.x / 2.0f == m_Bounds.Right)
            // {
            //     float panel_top_border_y_coord = new_panel_position.y + new_panel_dimensions.y / 2.0f;
            //     new_panel_dimensions.y = panel_top_border_y_coord - m_Bounds.Bottom - bottom_panel_dimensions.y;
            //     new_panel_position.y = panel_top_border_y_coord - new_panel_dimensions.y / 2.0f;
            // }
        }
        else
        {
            float panel_top_border_y_coord = new_panel_position.y + new_panel_dimensions.y / 2.0f;
            new_panel_dimensions.y = panel_top_border_y_coord - m_Bounds.Bottom;
            new_panel_position.y = panel_top_border_y_coord - new_panel_dimensions.y / 2.0f;
        }
        return { new_panel_position, new_panel_dimensions };
    }

    Metrics Dockspace::GetPanelMetricsForDockingBottom(const glm::vec2& panel_position, const glm::vec2& panel_dimensions)
    {
        if (!m_IsBottomOccupied)
        {
            glm::vec new_panel_dimensions = panel_dimensions;
            new_panel_dimensions.x = m_Bounds.Right - m_Bounds.Left;
            if (new_panel_dimensions.y > (m_Bounds.Top - m_Bounds.Bottom) / 2.0f)
                new_panel_dimensions.y = (m_Bounds.Top - m_Bounds.Bottom) / 2.0f;
            glm::vec2 new_panel_position = { m_Bounds.Left + new_panel_dimensions.x / 2.0f, m_Bounds.Bottom + new_panel_dimensions.y / 2.0f };

            // Invalidate panel positions to adjust when the given panel is docked bottom
            InvalidatePanelPositionsWhenDockingInSlot(DockingSlot::Bottom, new_panel_dimensions);

            return { new_panel_position, new_panel_dimensions };
        }

        glm::vec2 new_panel_position = panel_position;
        glm::vec2 new_panel_dimensions = panel_dimensions;

        new_panel_position.y = m_Bounds.Bottom + new_panel_dimensions.y / 2.0f;

        if (m_IsLeftOccupied)
        {
            // glm::vec2 left_panel_position = m_LeftPanel->GetPosition();
            // glm::vec2 left_panel_dimensions = m_LeftPanel->GetDimensions();
            // if (left_panel_position.y - left_panel_dimensions.y / 2.0f == m_Bounds.Bottom)
            // {
            //     float panel_right_border_x_coord = new_panel_position.x + new_panel_dimensions.x / 2.0f;
            //     new_panel_dimensions.x = panel_right_border_x_coord - m_Bounds.Left - left_panel_dimensions.x;
            //     new_panel_position.x = panel_right_border_x_coord - new_panel_dimensions.x / 2.0f;
            // }
        }
        else
        {
            float bottom_panel_right_border_x_coord = new_panel_position.x + new_panel_dimensions.x / 2.0f;
            new_panel_dimensions.x = bottom_panel_right_border_x_coord - m_Bounds.Left;
            new_panel_position.x = bottom_panel_right_border_x_coord - new_panel_dimensions.x / 2.0f;
        }
        if (m_IsRightOccupied)
        {
            // glm::vec2 right_panel_position = m_RightPanel->GetPosition();
            // glm::vec2 right_panel_dimensions = m_RightPanel->GetDimensions();
            // if (right_panel_position.y - right_panel_dimensions.y / 2.0f == m_Bounds.Bottom)
            // {
            //     float panel_left_border_x_coord = new_panel_position.x - new_panel_dimensions.x / 2.0f;
            //     new_panel_dimensions.x = m_Bounds.Right - panel_left_border_x_coord - right_panel_dimensions.x;
            //     new_panel_position.x = panel_left_border_x_coord + new_panel_dimensions.x / 2.0f;
            // }
        }
        else
        {
            float bottom_panel_left_border_x_coord = new_panel_position.x - new_panel_dimensions.x / 2.0f;
            new_panel_dimensions.x = m_Bounds.Right - bottom_panel_left_border_x_coord;
            new_panel_position.x = bottom_panel_left_border_x_coord + new_panel_dimensions.x / 2.0f;
        }
        return { new_panel_position, new_panel_dimensions };
    }

    Metrics Dockspace::GetPanelMetricsForDockingTop(const glm::vec2& panel_position, const glm::vec2& panel_dimensions)
    {
        if (!m_IsTopOccupied)
        {
            glm::vec new_panel_dimensions = panel_dimensions;
            new_panel_dimensions.x = m_Bounds.Right - m_Bounds.Left;
            if (new_panel_dimensions.y > (m_Bounds.Top - m_Bounds.Bottom) / 2.0f)
                new_panel_dimensions.y = (m_Bounds.Top - m_Bounds.Bottom) / 2.0f;
            glm::vec2 new_panel_position = { m_Bounds.Left + new_panel_dimensions.x / 2.0f, m_Bounds.Top - new_panel_dimensions.y / 2.0f };

            // Invalidate panel positions to adjust when the given panel is docked top
            InvalidatePanelPositionsWhenDockingInSlot(DockingSlot::Top, new_panel_dimensions);

            return { new_panel_position, new_panel_dimensions };
        }

        glm::vec2 new_panel_position = panel_position;
        glm::vec2 new_panel_dimensions = panel_dimensions;

        new_panel_position.y = m_Bounds.Top - new_panel_dimensions.y / 2.0f;

        if (m_IsLeftOccupied)
        {
            // glm::vec2 left_panel_position = m_LeftPanel->GetPosition();
            // glm::vec2 left_panel_dimensions = m_LeftPanel->GetDimensions();
            // if (left_panel_position.y + left_panel_dimensions.y / 2.0f == m_Bounds.Top)
            // {
            //     float panel_right_border_x_coord = new_panel_position.x + new_panel_dimensions.x / 2.0f;
            //     new_panel_dimensions.x = panel_right_border_x_coord - m_Bounds.Left - left_panel_dimensions.x;
            //     new_panel_position.x = panel_right_border_x_coord - new_panel_dimensions.x / 2.0f;
            // }
        }
        else
        {
            float top_panel_right_border_x_coord = new_panel_position.x + new_panel_dimensions.x / 2.0f;
            new_panel_dimensions.x = top_panel_right_border_x_coord - m_Bounds.Left;
            new_panel_position.x = top_panel_right_border_x_coord - new_panel_dimensions.x / 2.0f;
        }
        if (m_IsRightOccupied)
        {
            // glm::vec2 right_panel_position = m_RightPanel->GetPosition();
            // glm::vec2 right_panel_dimensions = m_RightPanel->GetDimensions();
            // if (right_panel_position.y + right_panel_dimensions.y / 2.0f == m_Bounds.Top)
            // {
            //     float panel_left_border_x_coord = new_panel_position.x - new_panel_dimensions.x / 2.0f;
            //     new_panel_dimensions.x = m_Bounds.Right - panel_left_border_x_coord - right_panel_dimensions.x;
            //     new_panel_position.x = panel_left_border_x_coord + new_panel_dimensions.x / 2.0f;
            // }
        }
        else
        {
            float top_panel_left_border_x_coord = new_panel_position.x - new_panel_dimensions.x / 2.0f;
            new_panel_dimensions.x = m_Bounds.Right - top_panel_left_border_x_coord;
            new_panel_position.x = top_panel_left_border_x_coord + new_panel_dimensions.x / 2.0f;
        }
        return { new_panel_position, new_panel_dimensions };
    }

    void Dockspace::SetLeftPanel(Panel* panel)
    {
        m_LeftPanel = panel;
        m_IsLeftOccupied = true;
    }

    void Dockspace::SetRightPanel(Panel* panel)
    {
        m_RightPanel = panel;
        m_IsRightOccupied = true;
    }

    void Dockspace::SetBottomPanel(Panel* panel)
    {
        m_BottomPanel = panel;
        m_IsBottomOccupied = true;
    }

    void Dockspace::SetTopPanel(Panel* panel)
    {
        m_TopPanel = panel;
        m_IsTopOccupied = true;
    }
}
