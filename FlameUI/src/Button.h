#pragma once
#include <string>
#include "Core.h"
#include <glm/glm.hpp>

namespace FlameUI {
    struct QuadProps
    {
        glm::ivec2 position_in_pixels;
        glm::ivec2 dimensions_in_pixels;
        glm::vec4 color;
        std::string texture_filepath;

        QuadProps() = default;
        QuadProps(
            const glm::ivec2& position_in_pixels,
            const glm::ivec2& dimensions_in_pixels,
            const glm::vec4& color,
            const std::string& texture_filepath
        )
            : position_in_pixels(position_in_pixels), dimensions_in_pixels(dimensions_in_pixels), color(color), texture_filepath(texture_filepath)
        {
        }
    };

    class Button
    {
    public:
        Button(
            uint32_t* quadId,
            const std::string& title,
            const QuadPosType& quadPosType,
            const glm::ivec2& position_in_pixels,
            const glm::ivec2& dimensions_in_pixels,
            const glm::vec4& color,
            const std::string& textureFilePath
        );
        ~Button() = default;

        static std::shared_ptr<Button> Create(
            uint32_t* quadId,
            const std::string& title,
            const QuadPosType& quadPosType,
            const glm::ivec2& position_in_pixels,
            const glm::ivec2& dimensions_in_pixels,
            const glm::vec4& color,
            const std::string& textureFilePath
        );

        inline glm::ivec2 GetPosition() const { return m_Position; }
        inline uint32_t GetQuadId() const { return m_QuadId; }
    private:
        glm::ivec2 m_Position;
        glm::ivec4 m_InnerPadding;
        uint32_t m_QuadId;
    };
}