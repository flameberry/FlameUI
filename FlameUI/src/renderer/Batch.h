#pragma once
#include <vector>
#include <array>
#include "../core/datatypes.h"
#include "../core/ElementTypeIndex.h"
#include <glm/glm.hpp>

/// This Macro contains the max number of texture slots that the GPU supports, varies for each computer.
#define MAX_TEXTURE_SLOTS 16
#define MAX_QUADS 10000
#define MAX_VERTICES 4 * MAX_QUADS
#define MAX_INDICES 6 * MAX_QUADS
#define TITLE_BAR_HEIGHT 15

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
        /// Quad Dimensions which will be used by the shader to customize the quad
        glm::vec2 quad_dimensions;
        /// This will tell the shader what kind of UI Element is being rendered
        float     element_type_index;

        /// Default Constructor
        Vertex()
            : position(0.0f), color(1.0f), texture_uv(0.0f), texture_index(-1.0f), quad_dimensions(0.0f), element_type_index(FL_ELEMENT_TYPE_GENERAL_INDEX)
        {
        }
    };

    class Batch
    {
    public:
        ~Batch() = default;

        void Init();
        void Empty();
        void OnDraw();
        void AddQuad(Vertex* vertices);
    protected:
        /// Renderer IDs required for OpenGL 
        uint32_t m_VertexBufferId, m_IndexBufferId, m_VertexArrayId, m_ShaderProgramId;
        std::array<uint32_t, MAX_TEXTURE_SLOTS> m_TextureIds;
        /// All the vertices stored by a Batch.
        std::vector<Vertex> m_Vertices;
    };
}