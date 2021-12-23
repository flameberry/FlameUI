#pragma once
#include <vector>
#include <array>
#include "Vertex.h"
#include "datatypes.h"

/// This Macro contains the max number of texture slots that the GPU supports, varies for each computer.
#define MAX_TEXTURE_SLOTS 16

namespace FlameUI {
    /// The type of quads that will be stored in a batch
    enum class BatchType { BasicQuad = 0, TexturedQuad, Text };

    /// An Interface for the types of Batches indicated by the `BatchType` enum class
    class Batch
    {
    public:
        virtual ~Batch() = default;

        virtual void Init() = 0;
        virtual void OnDraw() = 0;
        virtual void AddQuad(const std::array<Vertex, 4>& vertices, uint32_t* location) = 0;
        virtual void AddTextureId(uint32_t textureId) = 0;
        virtual void SetQuadVertices(uint32_t location, const std::array<Vertex, 4>& vertices) = 0;
        virtual void SetQuadZIndex(uint32_t location, float z) = 0;
        virtual void RemoveQuadVertices(uint32_t location, flame::optional<uint32_t>& prev_loc_of_new_vertices) = 0;
        virtual bool DoQuadVerticesExist(uint32_t location) = 0;
        virtual bool IsFull() = 0;

        virtual BatchType              GetBatchType() const = 0;
        virtual glm::vec2              GetQuadPosition(uint32_t location) = 0;
        virtual glm::vec2              GetQuadDimensions(uint32_t location) = 0;
        virtual std::array<Vertex*, 4> GetPtrToQuadVertices(uint32_t location) = 0;
    protected:
        /// Renderer IDs required for OpenGL 
        uint32_t m_VertexBufferId, m_IndexBufferId, m_VertexArrayId, m_ShaderProgramId;
        /// All the vertices stored by a Batch.
        std::vector<Vertex> m_Vertices;
    };

    /// Basic Quad Batch contains all non-textured quads, just plain solid colors.
    class BasicQuadBatch : public Batch
    {
    public:
        BasicQuadBatch() = default;
        virtual ~BasicQuadBatch();

        void Init() override;
        void OnDraw() override;
        void AddQuad(const std::array<Vertex, 4>& vertices, uint32_t* location) override;
        void AddTextureId(uint32_t textureId) override;
        void SetQuadVertices(uint32_t location, const std::array<Vertex, 4>& vertices) override;
        void SetQuadZIndex(uint32_t location, float z) override;
        void RemoveQuadVertices(uint32_t location, flame::optional<uint32_t>& prev_loc_of_new_vertices) override;
        bool DoQuadVerticesExist(uint32_t location) override;
        bool IsFull() override;

        BatchType              GetBatchType() const override;
        glm::vec2              GetQuadDimensions(uint32_t location) override;
        glm::vec2              GetQuadPosition(uint32_t location) override;
        std::array<Vertex*, 4> GetPtrToQuadVertices(uint32_t location) override;
    private:
        /// Max Quads that the Renderer can draw, not because of any limitations, but because OpenGL needs a
        /// buffer size for the vertices.
        static const uint16_t s_Max_Quads = 10000;
        static const uint16_t s_Max_Vertices = 4 * s_Max_Quads;
        static const uint16_t s_Max_Indices = 6 * s_Max_Quads;
    };

    /// As the name suggests, this class contains only textured quads
    class TexturedQuadBatch : public Batch
    {
    public:
        TexturedQuadBatch() = default;
        virtual ~TexturedQuadBatch();

        void Init() override;
        void OnDraw() override;
        void AddQuad(const std::array<Vertex, 4>& vertices, uint32_t* location) override;
        void AddTextureId(uint32_t textureId) override;
        void SetQuadVertices(uint32_t location, const std::array<Vertex, 4>& vertices) override;
        void SetQuadZIndex(uint32_t location, float z) override;
        void RemoveQuadVertices(uint32_t location, flame::optional<uint32_t>& prev_loc_of_new_vertices) override;
        bool DoQuadVerticesExist(uint32_t location) override;
        bool IsFull() override;

        BatchType              GetBatchType() const override;
        glm::vec2              GetQuadPosition(uint32_t location) override;
        glm::vec2              GetQuadDimensions(uint32_t location) override;
        std::array<Vertex*, 4> GetPtrToQuadVertices(uint32_t location) override;
    private:
        /// Max number of quads are equal to max number of texture slots, as each quad needs one texture slot to be bound to.
        static const uint16_t s_Max_Quads = MAX_TEXTURE_SLOTS;
        static const uint16_t s_Max_Vertices = 4 * s_Max_Quads;
        static const uint16_t s_Max_Indices = 6 * s_Max_Quads;
    private:
        /// As this batch contains all textured quads, Renderer IDs for textures are needed which are stored in this variable.
        std::vector<uint32_t> m_TextureIds;
    };

    /// As the name suggests, this batch contains all vertices of text quads.
    class TextBatch : public Batch
    {
    public:
        TextBatch() = default;
        virtual ~TextBatch();

        void Init() override;
        void OnDraw() override;
        void AddQuad(const std::array<Vertex, 4>& vertices, uint32_t* location) override;
        void AddTextureId(uint32_t textureId) override;
        void SetQuadVertices(uint32_t location, const std::array<Vertex, 4>& vertices) override;
        void SetQuadZIndex(uint32_t location, float z) override;
        void RemoveQuadVertices(uint32_t location, flame::optional<uint32_t>& prev_loc_of_new_vertices) override;
        bool DoQuadVerticesExist(uint32_t location) override;
        bool IsFull() override;

        BatchType              GetBatchType() const override;
        glm::vec2              GetQuadPosition(uint32_t location) override;
        glm::vec2              GetQuadDimensions(uint32_t location) override;
        std::array<Vertex*, 4> GetPtrToQuadVertices(uint32_t location) override;
    private:
        /// Max number of quads are equal to max number of texture slots, as each quad needs one texture slot to be bound to.
        static const uint16_t s_Max_Quads = MAX_TEXTURE_SLOTS;
        static const uint16_t s_Max_Vertices = 4 * s_Max_Quads;
        static const uint16_t s_Max_Indices = 6 * s_Max_Quads;
    private:
        /// As this batch contains text which are textured quads,
        /// Renderer IDs for textures are needed which are stored in this variable.
        std::vector<uint32_t> m_TextureIds;
    };
}