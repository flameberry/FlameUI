#pragma once
#include <string>
#include <glm/glm.hpp>

namespace FlameUI {
    struct ButtonInfo
    {
        std::string text;
        glm::vec3 position;
        glm::vec2 dimensions;

        ButtonInfo(const std::string& text, const glm::vec3& position, const glm::vec2& dimensions)
            : text(text), position(position), dimensions(dimensions)
        {
        }
        ButtonInfo() = default;
        ~ButtonInfo() = default;
    };
}