#pragma once
#include <vector>
#include <array>
#include "Vertex.h"

namespace FlameUI {
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
        virtual bool IsFull() = 0;

        virtual std::array<Vertex*, 4> GetPtrToQuadVertices(uint32_t location) = 0;
    protected:
        uint32_t m_VertexBufferId, m_IndexBufferId, m_VertexArrayId, m_ShaderProgramId;
        std::vector<Vertex> m_Vertices;
    };

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
        bool IsFull() override;

        std::array<Vertex*, 4> GetPtrToQuadVertices(uint32_t location) override;
    private:
        static const uint16_t s_Max_Quads = 10000;
        static const uint16_t s_Max_Vertices = 4 * s_Max_Quads;
        static const uint16_t s_Max_Indices = 6 * s_Max_Quads;
    };

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
        bool IsFull() override;

        std::array<Vertex*, 4> GetPtrToQuadVertices(uint32_t location) override;
    private:
        static const uint16_t s_Max_Texture_Slots = 16;
        static const uint16_t s_Max_Quads = s_Max_Texture_Slots;
        static const uint16_t s_Max_Vertices = 4 * s_Max_Quads;
        static const uint16_t s_Max_Indices = 6 * s_Max_Quads;
    private:
        std::vector<uint32_t> m_TextureIds;
    };

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
        bool IsFull() override;

        std::array<Vertex*, 4> GetPtrToQuadVertices(uint32_t location) override;
    private:
        static const uint16_t s_Max_Texture_Slots = 16;
        static const uint16_t s_Max_Quads = s_Max_Texture_Slots;
        static const uint16_t s_Max_Vertices = 4 * s_Max_Quads;
        static const uint16_t s_Max_Indices = 6 * s_Max_Quads;
    private:
        std::vector<uint32_t> m_TextureIds;
    };
}