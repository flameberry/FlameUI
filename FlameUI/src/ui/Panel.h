#pragma once
#include <vector>
#include "renderer/Renderer.h"
#include "Button.h"
#include <GLFW/glfw3.h>

namespace FlameUI {
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
    enum class MainState { None = 0, Grabbed, Resizing, InPanelActivity };

    class Panel
    {
    public:
        Panel(const std::string& title = "Untitled Panel", const glm::vec2& position = glm::vec2{ 0.0f }, const glm::vec2& dimensions = glm::vec2{ 100.0f }, const glm::vec4& color = FL_WHITE);
        ~Panel() = default;

        void                OnDraw();
        void                AddButton(const std::string& text, const glm::vec2& dimensions);
        bool                IsFocused() const { return m_IsFocused; }
        void                SetDetailedResizeState(const DetailedResizeState& detailedResizeState) { m_DetailedResizeState = detailedResizeState; }
        DetailedResizeState GetDetailedResizeState() const { return m_DetailedResizeState; }
        DockState           GetDockState() const { return m_DockState; }
        void                SetDockstate(const DockState& dockState) { m_DockState = dockState; }
        DetailedDockState   GetDetailedDockState() const { return m_DetailedDockState; }
        void                SetDetailedDockstate(const DetailedDockState& detailedDockState) { m_DetailedDockState = detailedDockState; }
        MainState           GetMainState() const { return m_MainState; }
        void                SetMainState(const MainState& mainState) { m_MainState = mainState; }
        bool                IsGrabbed() const { return m_MainState == MainState::Grabbed; }
        bool                IsResizing() const { return m_MainState == MainState::Resizing; }
        bool                IsDocked() const { return m_DockState == DockState::Docked; }
        bool                IsInPanelActivity() const { return m_MainState == MainState::InPanelActivity; }
        bool                IsHoveredOnPanel();
        void                SetFocus(bool value);
        void                SetZIndex(float z);
        float               GetZIndex() const { return m_Position.z; }
        std::string         GetPanelName() const { return m_PanelName; }
        uint32_t            GetPanelId() const { return m_PanelId; }
        Rect2D              GetPanelRect2D() const { return m_PanelRect2D; }
        float               GetWidth() const { return m_Dimensions.x; }
        float               GetHeight() const { return m_Dimensions.y; }
        void                UpdateMetrics(const glm::vec2& position, const glm::vec2& dimensions);
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

        void                InvalidateButtonPos();
        std::vector<Button>& GetPanelButtons() { return m_Buttons; }

        static std::shared_ptr<Panel> Create(const std::string& title = "Untitled Panel", const glm::vec2& position = glm::vec2{ 0.0f }, const glm::vec2& dimensions = glm::vec2{ 100.0f }, const glm::vec4& color = FL_WHITE);
    private:
        uint32_t                             m_PanelId;
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
        // Stores the offset of the cursor from the center of the panel, when the panel is grabbed
        glm::vec2                            m_OffsetOfCursorWhenGrabbed;
        DetailedResizeState                  m_DetailedResizeState;
        DockState                            m_DockState;
        DetailedDockState                    m_DetailedDockState;
        MainState                            m_MainState;
        std::vector<Button>                  m_Buttons;
    };
}