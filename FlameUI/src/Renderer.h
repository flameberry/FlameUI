#pragma once
#include <array>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <unordered_map>
#include "opengl_math_types.h"
#include "core_macros.h"

namespace FlameUI {
    class Renderer
    {
    public:
        static void Init();
        static void CleanUp();

        static void AddQuad(
            uint32_t* quadId,
            const QuadPosType& positionType,
            const glm::ivec2& position_in_pixels,
            const glm::ivec2& dimensions_in_pixels,
            const glm::vec4& color,
            const std::string& textureFilePath
        );
        static void AddText(
            const std::string& text,
            const glm::ivec2& position_in_pixels,
            float scale,
            const glm::vec4& color,
            glm::ivec2* textDimensions = nullptr
        );
        static void OnDraw();
        static void ChangeQuadVertices(uint32_t& quadId, const std::array<Vertex, 4>& vertices);
        static void ChangeQuadVertices(
            uint32_t& quadId,
            const QuadPosType& positionType,
            const glm::ivec2& position_in_pixels = glm::ivec2(),
            const glm::ivec2& dimensions_in_pixels = glm::ivec2(),
            const glm::vec4& color = glm::vec4()
        );

        inline static void SetUIFont(const std::string& filePath) { s_UserFontFilePath = filePath; }
        static void OnResize(uint32_t window_width, uint32_t window_height);
    private:
        static void LoadFont(const std::string& filePath);
        static uint32_t GenQuadId();
        static void GetQuadVertices(
            std::array<Vertex, 4>* vertices,
            const QuadPosType& positionType,
            const glm::ivec2& position_in_pixels,
            const glm::ivec2& dimensions_in_pixels,
            const glm::vec4& color
        );
        static void LoadTexture(uint32_t* quadId, const std::string& filePath);
        static GLint GetUniformLocation(const std::string& name, uint32_t shaderId);
        static glm::vec2 ConvertPixelsToOpenGLValues(const glm::ivec2& value_in_pixels);
        static float ConvertXAxisPixelValueToOpenGLValue(int X);
        static float ConvertYAxisPixelValueToOpenGLValue(int Y);
        static void AddQuadToTextBatch(uint32_t* quadId, const std::array<FlameUI::Vertex, 4>& vertices, uint32_t textureId);
    private:
        static const uint16_t s_Max_Texture_Slots = 16;
        static const uint16_t s_Max_Quads_Per_Batch = 16;
        static const uint16_t s_Max_Indices_Per_Batch = 6 * s_Max_Quads_Per_Batch;
        static const uint16_t s_Max_Vertices_Per_Batch = 4 * s_Max_Quads_Per_Batch;
    private:
        struct OpenGLIds
        {
            uint32_t v_bufferId, i_bufferId, v_arrayId, shaderId;
            std::vector<uint32_t> textureIds;
        };
        struct UniformBufferData
        {
            glm::mat4 ProjectionMatrix;
        };
        enum class BatchType
        {
            None = 0, Quad, Text
        };
        struct Batch
        {
            Batch()
            {
                Vertices.reserve(s_Max_Vertices_Per_Batch);
            }
            Batch(const BatchType& batchType)
                : CurrentBatchType(batchType)
            {
                Vertices.reserve(s_Max_Vertices_Per_Batch);
            }
            ~Batch() = default;

            void Init();
            void OnDraw();

            std::vector<Vertex> Vertices;
            OpenGLIds RendererIds;
            BatchType CurrentBatchType = BatchType::None;
        };
        struct Character
        {
            uint32_t TextureId;  // ID handle of the glyph texture
            glm::ivec2 Size;       // Size of glyph
            glm::ivec2 Bearing;    // Offset from baseline to left/top of glyph
            double Advance;    // Offset to advance to next glyph
        };
        struct FontProps
        {
            float Scale;
            float Strength;
            float PixelRange;
        };
    private:
        static float s_AspectRatio;
        static uint32_t s_UniformBufferId;
        static UniformBufferData s_UniformBufferData;

        static std::vector<Batch> s_Batches;

        static std::unordered_map<char, Character> s_Characters;
        static std::string s_UserFontFilePath;
        static std::string s_DefaultFontFilePath;

        static FontProps s_FontProps;

        static std::unordered_map<uint32_t, uint32_t[2]> s_QuadDictionary;
        static std::unordered_map<std::string, GLint> m_uniformloc_cache;
    };
}
