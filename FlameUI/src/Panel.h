#pragma once
#include <vector>
#include "Renderer.h"
#include "Button.h"
#include <GLFW/glfw3.h>

namespace FlameUI {
    enum class ResizeState { None = 0, HoveredOnResizeArea, Resizing };
    enum class DetailedResizeState {
        NotResizing = 0,
        ResizingLeftBorder, ResizingRightBorder, ResizingBottomBorder, ResizingTopBorder,
        ResizingBottomLeftCorner, ResizingBottomRightCorner, ResizingTopLeftCorner, ResizingTopRightCorner
    };

    class Panel
    {
    public:
        Panel(const std::string& panelName, const glm::ivec2& position_in_pixels, const glm::ivec2& dimensions_in_pixels, const glm::vec4& color, const std::string& textureFilePath);
        ~Panel() = default;

        void            AddButton(uint32_t* quadId, const std::string& title, const glm::ivec2& dimensions_in_pixels, const glm::vec4& color, const std::string& textureFilePath);
        void            OnEvent();
        void            OnUpdate();
        bool            IsFocused() const { return m_IsFocused; }
        bool            IsGrabbed() const { return m_IsGrabbed; }
        bool            IsResizing() const { return m_ResizeState == ResizeState::Resizing; }
        bool            IsHoveredOnPanel();
        void            SetFocus(bool value);
        void            SetZIndex(float z);
        float           GetZIndex() const { return m_ZIndex; }
        std::string     GetPanelName() const { return m_PanelName; }
        uint32_t        GetPanelQuadId() const { return m_PanelQuadId; }
        Bounds          GetBounds() const { return m_Bounds; }
        static uint16_t GetResizeAreaPadding() { return s_ResizeAreaPadding; }

        static std::shared_ptr<Panel> Create(const std::string& panelName, const glm::ivec2& position_in_pixels, const glm::ivec2& dimensions_in_pixels, const glm::vec4& color, const std::string& textureFilePath);
    private:
        void ResizeLeft(const std::array<Vertex*, 4>& ptr_to_vertices, const float& cursor_pos_x);
        void ResizeRight(const std::array<Vertex*, 4>& ptr_to_vertices, const float& cursor_pos_x);
        void ResizeBottom(const std::array<Vertex*, 4>& ptr_to_vertices, const float& cursor_pos_y);
        void ResizeTop(const std::array<Vertex*, 4>& ptr_to_vertices, const float& cursor_pos_y);
    private:
        GLFWcursor* m_CursorState;

        uint32_t                             m_PanelQuadId;
        // Important: m_Position is the position of the center of the panel
        glm::ivec2                           m_Position;
        glm::ivec2                           m_Dimensions;
        Bounds                               m_Bounds;
        glm::ivec2                           m_InnerPadding;
        glm::vec4                            m_Color;
        float                                m_ZIndex;
        std::string                          m_PanelName;
        bool                                 m_IsFocused = false;
        bool                                 m_IsFirstTime = true;
        bool                                 m_IsGrabbed = false;
        bool                                 m_IsCursorInPanelArea = false;
        float                                m_OffsetX, m_OffsetY;
        constexpr static uint16_t            s_MinWidth = 10, s_MinHeight = 10;
        constexpr static uint16_t            s_ResizeAreaPadding = 2;
        ResizeState                          m_ResizeState;
        DetailedResizeState                  m_DetailedResizeState;
        std::vector<std::shared_ptr<Button>> m_Buttons;
    };
}