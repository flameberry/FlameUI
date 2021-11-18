#pragma once
#include <vector>
#include "Button.h"
#include <GLFW/glfw3.h>

namespace FlameUI {
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
        void            SetFocus(bool value);
        void            SetDefaultZIndex(float z);
        float           GetZIndex() const { return m_ZIndex; }
        std::string     GetPanelName() const { return m_PanelName; }
        uint32_t        GetPanelQuadId() const { return m_PanelQuadId; }
        Bounds          GetBounds() const { return m_Bounds; }

        static std::shared_ptr<Panel> Create(const std::string& panelName, const glm::ivec2& position_in_pixels, const glm::ivec2& dimensions_in_pixels, const glm::vec4& color, const std::string& textureFilePath);
    private:
        uint32_t                             m_PanelQuadId;
        // Important: m_Position is the position of the center of the panel
        glm::ivec2                           m_Position;
        glm::ivec2                           m_Dimensions;
        Bounds                               m_Bounds;
        glm::ivec2                           m_InnerPadding;
        glm::vec4                            m_Color;
        float                                m_ZIndex;
        float                                m_DefaultZIndex;
        std::string                          m_PanelName;
        bool                                 m_IsFocused = false;
        bool                                 m_IsFirstTime = true;
        bool                                 m_IsGrabbed = false;
        float                                m_OffsetX, m_OffsetY;
        std::vector<std::shared_ptr<Button>> m_Buttons;
    };
}