#pragma once
#include <string>
#include "Core.h"
#include <glm/glm.hpp>

namespace FlameUI {
    struct QuadProps
    {
        glm::vec2  position_in_pixels;
        glm::vec2  dimensions_in_pixels;
        glm::vec4   color;
        std::string texture_filepath;

        QuadProps() = default;
        QuadProps(
            const glm::vec2& position_in_pixels, const glm::vec2& dimensions_in_pixels, const glm::vec4& color,
            const std::string& texture_filepath
        )
            : position_in_pixels(position_in_pixels), dimensions_in_pixels(dimensions_in_pixels), color(color), texture_filepath(texture_filepath)
        {
        }
    };

    class Button
    {
    public:
        Button(const std::string& title, const QuadPosType& quadPosType, const glm::vec2& position_in_pixels,
            const glm::vec2& dimensions_in_pixels, const glm::vec4& color, const std::string& textureFilePath);
        Button(const std::string& title, const glm::vec2& panel_position, const QuadPosType& quadPosType,
            const glm::vec2& button_pos_in_pixels, const glm::vec2& dimensions_in_pixels, const glm::vec4& color,
            const std::string& textureFilePath
        );
        ~Button() = default;

        static std::shared_ptr<Button> Create(
            const std::string& title, const QuadPosType& quadPosType, const glm::vec2& position_in_pixels,
            const glm::vec2& dimensions_in_pixels, const glm::vec4& color, const std::string& textureFilePath
        );

        static std::shared_ptr<Button> Create(
            const std::string& title, const glm::vec2& panel_position, const QuadPosType& quadPosType,
            const glm::vec2& button_pos_in_pixels, const glm::vec2& dimensions_in_pixels, const glm::vec4& color,
            const std::string& textureFilePath
        );

        void       OnUpdate(const glm::vec2& panel_position_in_pixels, float z_index);
        glm::vec2  GetPosition(const QuadPosType& quadPosType) const;
        uint32_t   GetQuadId() const { return m_ButtonQuadId; }
    private:
        glm::vec2  m_PositionCenter;
        glm::vec2  m_PositionBLVertex;
        glm::vec2  m_PositionCenterRelativeToPanel;
        glm::ivec4 m_InnerPadding;
        uint32_t   m_ButtonQuadId;
    };
}