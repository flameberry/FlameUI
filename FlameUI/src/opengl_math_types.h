#pragma once
#include <glm/glm.hpp>
namespace FlameUI {
    struct Vertex
    {
        glm::vec3 position;
        glm::vec4 color;
        glm::vec2 texture_uv;
        float texture_index;

        Vertex()
            : position(0.0f), color(1.0f), texture_uv(0.0f), texture_index(-1.0f)
        {
        }

        Vertex(const glm::vec3& position, const glm::vec4& color, const glm::vec2& texture_uv = glm::vec2(0.0f), float texture_index = -1.0f)
            : position(position), color(color), texture_uv(texture_uv), texture_index(texture_index)
        {
        }
    };
}
