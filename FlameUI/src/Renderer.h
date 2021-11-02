#pragma once
#include <array>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <unordered_map>
#include "opengl_math_types.h"
#include "Font.h"

namespace FlameUI {
    class Renderer
    {
    public:
        static void Init();
        static void CleanUp();

        static void AddQuad(
            uint32_t* quadId,
            const fuiVec2<int>& position_in_pixels,
            const fuiVec2<uint32_t>& dimensions_in_pixels,
            const fuiVec4<float>& color,
            const std::string& textureFilePath
        );
        static void AddText(
            const std::string& text,
            const fuiVec2<int>& position_in_pixels,
            float scale,
            const fuiVec4<float>& color
        );
        static void OnDraw();

        inline static void SetUIFont(const Font& font) { s_FlameUIFont = font; }
        static void OnResize(uint32_t window_width, uint32_t window_height);
    private:
        static void LoadFont(Font* font, const std::string& filePath);
        static uint32_t GenQuadId();
        static void GetQuadVertices(
            std::array<Vertex, 4>* vertices,
            const fuiVec2<int>& position_in_pixels,
            const fuiVec2<uint32_t>& dimensions_in_pixels,
            const fuiVec4<float>& color
        );
        static void LoadTexture(uint32_t* quadId, const std::string& filePath);
        static GLint GetUniformLocation(const std::string& name, uint32_t shaderId);
        static fuiVec2<float> ConvertPixelsToOpenGLValues(const fuiVec2<int>& value_in_pixels);
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
    private:
        static glm::mat4 s_Proj_Matrix;
        static float s_AspectRatio;

        static std::vector<Batch> s_Batches;

        static Font s_FlameUIFont;
        static std::string s_DefaultFontFilePath;

        static std::unordered_map<uint32_t, uint32_t[2]> s_QuadDictionary;
        static std::unordered_map<std::string, GLint> m_uniformloc_cache;
    };
}
