#pragma once
#include <array>
#include <vector>
#include <string>
#include <glad/glad.h>
#include <unordered_map>
#include "Core.h"
#include "Vertex.h"

namespace FlameUI {
    /// Class which deals with OpenGL Framebuffer
    class Framebuffer
    {
    public:
        Framebuffer(float width = 1280.0f, float height = 720.0f);
        ~Framebuffer();

        /// Recreates the Framebuffer object using the `m_FramebufferSize` variable
        void     OnUpdate();
        /// Sets the Framebuffer Size, but to take effect the Framebuffer object must be recreated using the `OnUpdate()` function
        void     SetFramebufferSize(float width, float height);
        /// Returns the opengl texture Id of texture made using the Framebuffer object
        uint32_t GetColorAttachmentId() const { return m_ColorAttachmentId; };
        /// Binds the Framebuffer object
        void     Bind() const;
        /// Unbinds the Framebuffer object
        void     Unbind() const;
    private:
        /// Renderer Ids, for the Framebuffer object, the texture of color attachment and for the depth attachment
        uint32_t  m_FramebufferId, m_ColorAttachmentId, m_DepthAttachmentId;
        /// Used by the `OnUpdate()` function to recreate the Framebuffer object
        glm::vec2 m_FramebufferSize;
    };

    class Renderer
    {
    public:
        static void        Init();
        static uint32_t    GenQuadId();
        static void        OnResize(uint32_t window_width, uint32_t window_height);
        static glm::vec2   GetTextDimensions(const std::string& text, float scale);
        inline static void SetUIFont(const std::string& filePath) { s_UserFontFilePath = filePath; }
        static void        AddText(const std::string& text, const glm::ivec2& position_in_pixels, float scale, const glm::vec4& color);
        static void        AddQuad(uint32_t* quadId, const QuadPosType& positionType, const glm::ivec2& position_in_pixels, const glm::ivec2& dimensions_in_pixels, const glm::vec4& color, const std::string& textureFilePath);
        static void        OnDraw();
        static void        ChangeQuadVertices(uint32_t& quadId, const std::array<Vertex, 4>& vertices);
        static void        ChangeQuadVertices(uint32_t& quadId, const QuadPosType& positionType, const glm::ivec2& position_in_pixels = glm::ivec2(), const glm::ivec2& dimensions_in_pixels = glm::ivec2(), const glm::vec4& color = glm::vec4());
        static void        CleanUp();
    private:
        /// Private Functions which will be used by the Renderer as Utilites
        static std::tuple<std::string, std::string> ReadShaderSource(const std::string& filePath);

        static void        LoadFont(const std::string& filePath);
        static void        GetQuadVertices(std::array<Vertex, 4>* vertices, const QuadPosType& positionType, const glm::ivec2& position_in_pixels, const glm::ivec2& dimensions_in_pixels, const glm::vec4& color);
        static void        LoadTexture(uint32_t* quadId, const std::string& filePath);
        static GLint       GetUniformLocation(const std::string& name, uint32_t shaderId);
        static glm::vec2   ConvertPixelsToOpenGLValues(const glm::ivec2& value_in_pixels);
        static float       ConvertXAxisPixelValueToOpenGLValue(int X);
        static float       ConvertYAxisPixelValueToOpenGLValue(int Y);
        static void        AddQuadToTextBatch(uint32_t* quadId, const std::array<FlameUI::Vertex, 4>& vertices, uint32_t textureId);
    private:
        /// This value has to be set to whatever number of texture slots your GPU can suppor
        static const uint16_t s_Max_Texture_Slots = 16;
        static const uint16_t s_Max_Quads_Per_Batch = 1 * s_Max_Texture_Slots;
        static const uint16_t s_Max_Indices_Per_Batch = 6 * s_Max_Quads_Per_Batch;
        static const uint16_t s_Max_Vertices_Per_Batch = 4 * s_Max_Quads_Per_Batch;
    private:
        /// Struct containing every basic RendererIds needed for a batch of quads
        struct OpenGLIds
        {
            uint32_t v_bufferId, i_bufferId, v_arrayId, shaderId;
            std::vector<uint32_t> textureIds;
        };
        /// Struct that contains all the matrices needed by the shader, which will be stored in a Uniform Buffer
        struct UniformBufferData { glm::mat4 ProjectionMatrix; };
        enum class BatchType { None = 0, Quad, Text };
        struct Batch
        {
            Batch();
            Batch(const BatchType& batchType);
            ~Batch() = default;

            void Init();
            void OnDraw();

            OpenGLIds           RendererIds;
            BatchType           CurrentBatchType;
            std::vector<Vertex> Vertices;
        };
        struct Character
        {
            uint32_t   TextureId;  // ID handle of the glyph texture
            glm::ivec2 Size;       // Size of glyph
            glm::ivec2 Bearing;    // Offset from baseline to left/top of glyph
            double     Advance;    // Offset to advance to next glyph
        };
        struct FontProps
        {
            float Scale;
            float Strength;
            float PixelRange;
            float AscenderY;
            float DescenderY;
        };
    private:
        /// AspectRatio used for converting pixel coordinates to opengl coordinates
        static float                                     s_AspectRatio;
        /// The RendererId needed for the Uniform Buffer
        static uint32_t                                  s_UniformBufferId;
        /// Contains Font properties of the main UI font
        static FontProps                                 s_FontProps;
        /// Stores all matrices needed by the shader, also stored in a Uniform Buffer
        static UniformBufferData                         s_UniformBufferData;
        /// Stores file path of the font provided by user and the default font file path
        static std::string                               s_UserFontFilePath, s_DefaultFontFilePath;
        /// The main vector of all the batches of quads to ever exist in the program
        static std::vector<Batch>                        s_Batches;
        /// Stores all the characters and their properties, which are extracted from the font provided by the user
        static std::unordered_map<char, Character>       s_Characters;

        /// Stores QuadId and array of two unsigned ints,
        /// first is the batch index in the `s_Batches` vector and second is the index at which the vertices of the quad start,
        /// in stored in the corresponding batch's `Vertices` vector
        /// The QuadId is generated using the `GenQuadId()` function, which is an unique ID assigned to every quad
        static std::unordered_map<uint32_t, uint32_t[2]> s_QuadDictionary;
        /// Stores the uniform location in a shader if the location needs to be reused
        static std::unordered_map<std::string, GLint>    m_uniformloc_cache;
    };
}
