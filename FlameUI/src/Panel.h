#pragma once
#include <vector>
#include "Button.h"

namespace FlameUI {
    class Panel
    {
    public:
        Panel(
            const glm::ivec2& position_in_pixels,
            const glm::ivec2& dimensions_in_pixels,
            const glm::vec4& color,
            const std::string& textureFilePath
        );
        ~Panel() = default;

        void AddButton(
            uint32_t* quadId,
            const std::string& title,
            const glm::ivec2& dimensions_in_pixels,
            const glm::vec4& color,
            const std::string& textureFilePath
        );
        void OnDraw();

        static std::shared_ptr<Panel> Create(
            const glm::ivec2& position_in_pixels,
            const glm::ivec2& dimensions_in_pixels,
            const glm::vec4& color,
            const std::string& textureFilePath
        );
        inline uint32_t GetPanelQuadId() const { return m_PanelQuadId; }
    private:
        uint32_t m_PanelQuadId;
        // Important: m_Position is the position of the center of the panel
        glm::ivec2 m_Position;
        glm::ivec2 m_Dimensions;
        glm::ivec2 m_InnerPadding;

        std::vector<std::shared_ptr<Button>> m_Buttons;
    };
}