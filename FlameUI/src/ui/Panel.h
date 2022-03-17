#pragma once
#include <vector>
#include "../renderer/Renderer.h"
#include "Button.h"
#include <GLFW/glfw3.h>

namespace FlameUI {
    enum class GrabState { NotGrabbed = 0, Grabbed };
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
        Panel(const std::string& title = "Untitled Panel", const glm::vec2& position = glm::vec2{ 0.0f }, const glm::vec2& dimensions = glm::vec2{ 100.0f }, const glm::vec4& color = FL_WHITE);
        ~Panel() = default;

        void                OnDraw();
        bool                IsFocused() const { return m_IsFocused; }
        void                SetGrabState(const GrabState& grabState) { m_GrabState = grabState; }
        void                SetResizeState(const ResizeState& resizeState) { m_ResizeState = resizeState; }
        ResizeState         GetResizeState() const { return m_ResizeState; }
        void                SetDetailedResizeState(const DetailedResizeState& detailedResizeState) { m_DetailedResizeState = detailedResizeState; }
        DetailedResizeState GetDetailedResizeState() const { return m_DetailedResizeState; }
        DockState           GetDockState() const { return m_DockState; }
        void                SetDockstate(const DockState& dockState) { m_DockState = dockState; }
        DetailedDockState   GetDetailedDockState() const { return m_DetailedDockState; }
        void                SetDetailedDockstate(const DetailedDockState& detailedDockState) { m_DetailedDockState = detailedDockState; }
        bool                IsGrabbed() const { return m_GrabState == GrabState::Grabbed; }
        bool                IsResizing() const { return m_ResizeState == ResizeState::Resizing; }
        bool                IsDocked() const { return m_DockState == DockState::Docked; }
        bool                IsHoveredOnPanel();
        void                SetFocus(bool value);
        void                SetZIndex(float z);
        float               GetZIndex() const { return m_Position.z; }
        std::string         GetPanelName() const { return m_PanelName; }
        uint32_t            GetPanelQuadId() const { return m_PanelQuadId; }
        Rect2D              GetPanelRect2D() const { return m_PanelRect2D; }
        float               GetWidth() const { return m_Dimensions.x; }
        float               GetHeight() const { return m_Dimensions.y; }
        glm::vec2           GetPosition() const { return m_Position; }
        void                SetPosition(const glm::vec2& position) { m_Position.x = position.x; m_Position.y = position.y; }
        glm::vec2           GetDimensions() const { return m_Dimensions; }
        void                SetDimensions(const glm::vec2& dimensions) { m_Dimensions = dimensions; }
        // Recalculates the bounds of the panel using the 'm_Position' and 'm_Dimensions' variables
        void                InvalidateBounds();
        // Stores the offset of the cursor from the center of the panel when the panel is grabbed to bring continuity in grabbing
        void                StoreOffsetOfCursorFromCenter(const glm::vec2& offset) { m_OffsetOfCursorWhenGrabbed = offset; }
        // Gets the offset stored by the 'StoreOffsetOfCursorFromCenter' function
        glm::vec2           GetOffsetOfCursorFromCenter() const { return m_OffsetOfCursorWhenGrabbed; }

        static std::shared_ptr<Panel> Create(const std::string& title = "Untitled Panel", const glm::vec2& position = glm::vec2{ 0.0f }, const glm::vec2& dimensions = glm::vec2{ 100.0f }, const glm::vec4& color = FL_WHITE);
    private:
        // Stores the unique QuadId provided by the Renderer to every Quad
        uint32_t                             m_PanelQuadId;
        // Important: m_Position is the position of the center of the panel
        glm::vec3                            m_Position;
        // Stores the dimensions of the panel in pixel units
        glm::vec2                            m_Dimensions;
        // Stores the coordinates of the boundaries of the panel
        Rect2D                               m_PanelRect2D;
        // Stores the Inner Padding of the panel, which is used to calculate button positions
        glm::vec2                            m_InnerPadding;
        // Stores the background color of the panel
        glm::vec4                            m_Color;
        // Stores the title which will be displayed in the title bar of the panel
        std::string                          m_PanelName;
        bool                                 m_IsFocused = false;
        GrabState                            m_GrabState;
        // Stores the offset of the cursor from the center of the panel, when the panel is grabbed
        glm::vec2                            m_OffsetOfCursorWhenGrabbed;
        ResizeState                          m_ResizeState;
        DetailedResizeState                  m_DetailedResizeState;
        DockState                            m_DockState;
        DetailedDockState                    m_DetailedDockState;
        // std::vector<std::shared_ptr<Button>> m_Buttons;
    };
}