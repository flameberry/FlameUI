#pragma once
#include <glm/glm.hpp>

namespace FlameUI {
    // The [Vertex] struct represents an OpenGL Vertex.
    struct Vertex
    {
        /// Position from -1.0f to 1.0f on both x-axis and y-axis
        glm::vec3 position;
        /// Color in rgba format, each channel ranging from 0.0f to 1.0f
        glm::vec4 color;
        /// Texture coordinates ranging from 0.0f to 1.0f
        glm::vec2 texture_uv;
        /// Texture index which will be used as opengl texture slot to which the texture will be bound
        float     texture_index;

        /// Default Constructor
        Vertex()
            : position(0.0f), color(1.0f), texture_uv(0.0f), texture_index(-1.0f)
        {
        }
        /// Default value of texture_index is -1.0f, so in the shader it can be recognized as a hint to not use any texture
        Vertex(const glm::vec3& position, const glm::vec4& color, const glm::vec2& texture_uv = glm::vec2(0.0f), float texture_index = -1.0f)
            : position(position), color(color), texture_uv(texture_uv), texture_index(texture_index)
        {
        }
    };
}
