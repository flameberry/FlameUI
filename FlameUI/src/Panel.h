#pragma once
#include <vector>
#include "Renderer.h"
#include "Button.h"
#include <GLFW/glfw3.h>

namespace FlameUI {
    enum class ResizeState { None = 0, HoveredOnResizeArea, Resizing };
    enum class DetailedResizeState
    {
        NotResizing = 0,
        ResizingLeftBorder, ResizingRightBorder, ResizingBottomBorder, ResizingTopBorder,
        ResizingBottomLeftCorner, ResizingBottomRightCorner, ResizingTopLeftCorner, ResizingTopRightCorner
    };

    enum class DockState { None = 0, HoveredOnDockingArea, Docked };
    enum class DetailedDockState
    {
        NotDocked = 0,
        DockedLeft, DockedRight, DockedBottom, DockedTop
    };

    class Panel
    {
    public:
        Panel(const std::string& panelName, const glm::vec2& position_in_pixels, const glm::vec2& dimensions_in_pixels, const glm::vec4& color);
        ~Panel() = default;

        void              AddButton(const std::string& title, const glm::vec2& dimensions_in_pixels, const glm::vec4& color, const std::string& textureFilePath);
        void              OnEvent();
        void              OnUpdate();
        bool              IsFocused() const { return m_IsFocused; }
        bool              IsGrabbed() const { return m_IsGrabbed; }
        bool              IsResizing() const { return m_ResizeState == ResizeState::Resizing; }
        bool              IsDocked() const { return m_DockState == DockState::Docked; }
        bool              IsHoveredOnPanel();
        void              InvalidateMetrics();
        void              SetFocus(bool value);
        void              SetZIndex(float z);
        float             GetZIndex() const { return m_ZIndex; }
        std::string       GetPanelName() const { return m_PanelName; }
        uint32_t          GetPanelQuadId() const { return m_PanelQuadId; }
        Bounds            GetBounds() const { return m_Bounds; }
        static uint16_t   GetResizeAreaPadding() { return s_ResizeAreaPadding; }
        DetailedDockState GetDetailedDockState() const { return m_DetailedDockState; }
        float             GetWidth() const { return m_Dimensions.x; }
        float             GetHeight() const { return m_Dimensions.y; }
        Bounds            GetDockspaceContribution() const;
        void              ProvideDockspaceBounds(const Bounds& bounds) { m_DockspaceBounds = bounds; }

        static std::shared_ptr<Panel> Create(const std::string& panelName, const glm::vec2& position_in_pixels, const glm::vec2& dimensions_in_pixels, const glm::vec4& color);
    private:
        void LogResizeState();
        void ResizeLeft(const std::array<Vertex*, 4>& ptr_to_vertices, const float& cursor_pos_x);
        void ResizeRight(const std::array<Vertex*, 4>& ptr_to_vertices, const float& cursor_pos_x);
        void ResizeBottom(const std::array<Vertex*, 4>& ptr_to_vertices, const float& cursor_pos_y);
        void ResizeTop(const std::array<Vertex*, 4>& ptr_to_vertices, const float& cursor_pos_y);
        void LogDockState();
        void DockLeft();
        void DockRight();
        void DockBottom();
        void DockTop();
        void Undock();
        void ShowDockingPreview();
    private:
        GLFWcursor* m_CursorState;

        uint32_t                             m_PanelQuadId;
        // Important: m_Position is the position of the center of the panel
        glm::vec2                            m_Position;
        glm::vec2                            m_Dimensions;
        Bounds                               m_Bounds;
        glm::vec2                            m_InnerPadding;
        glm::vec4                            m_Color;
        float                                m_ZIndex;
        std::string                          m_PanelName;
        bool                                 m_IsFocused = false;
        bool                                 m_IsFirstTime = true;
        bool                                 m_IsGrabbed = false;
        bool                                 m_IsCursorInPanelArea = false;
        float                                m_OffsetX, m_OffsetY;
        constexpr static uint16_t            s_MinWidth = 10, s_MinHeight = 10;
        constexpr static uint16_t            s_ResizeAreaPadding = 3;
        ResizeState                          m_ResizeState;
        DetailedResizeState                  m_DetailedResizeState;
        DockState                            m_DockState;
        DetailedDockState                    m_DetailedDockState;
        constexpr static uint16_t            s_MinLengthOfDockedPanel = 100;
        Bounds                               m_DockspaceContribution;
        Bounds                               m_DockspaceBounds;
        glm::vec2                            m_DockingPosition;
        bool                                 m_IsCurrentlyDocked = false;
        std::vector<std::shared_ptr<Button>> m_Buttons;
    };
}