#pragma once
#include <array>
#include <vector>
#include <string>
#include <glad/glad.h>
#include <unordered_map>
#include "../core/Core.h"
#include <GLFW/glfw3.h>
#include "Batch.h"
#include "../ui/Text.h"

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

    struct QuadCreateInfo
    {
        // Pointer to the variable where the unique Quad-Id should be stored
        uint32_t* quadId;
        // The type of position that is being provided, value can be 'FL_QUAD_POS_CENTER' or 'FL_QUAD_POS_BOTTOM_LEFT_VERTEX'
        QuadPosType positionType;
        // The position (in pixel units) which will be used to calculate the opengl vertices according to the Quad Position Type given
        glm::vec2* position;
        // The dimensions (in pixel units) of the Quad
        glm::vec2* dimensions;
        // The color (in rgba format) of the Quad. Note: Only valid if no 'textureFilePath' is provided
        glm::vec4* color;
        // The absolute texture file path, which will be displayed on the quad
        const char* textureFilePath;
        // The Z Index of the Quad
        float zIndex;
        // The element type to be rendered
        float elementTypeIndex;
    };

    struct ThemeInfo
    {
        // glm::vec4 panelBgColor;
        glm::vec4 panelTitleBarColor{ 50.0f / 255.0f, 50.0f / 255.0f, 50.0f / 255.0f, 1.0f };
    };

    struct RendererInitInfo
    {
        GLFWwindow* userWindow;
        bool enableFontRendering{ true };
        std::string fontFilePath{ FL_PROJECT_DIR"FlameUI/resources/fonts/OpenSans-Regular.ttf" };
        ThemeInfo* themeInfo;
    };

    class Renderer
    {
    public:
        // The Init function should be called after the GLFW window creation and before the main loop
        static void        Init(const RendererInitInfo& rendererInitInfo);
        static void        OnResize();
        static bool        DoesQuadExist(uint32_t* quadId);
        static ThemeInfo   GetThemeInfo() { return s_ThemeInfo; }
        static glm::vec2   GetWindowContentScale() { return s_WindowContentScale; }
        static GLFWwindow* GetUserGLFWwindow();
        static glm::vec2   GetViewportSize();
        static glm::vec2   GetTextDimensions(const std::string& text, float scale);
        static glm::vec2   GetQuadPositionInPixels(uint32_t* quadId);
        static glm::vec2   GetQuadDimensionsInPixels(uint32_t* quadId);
        static GLint       GetUniformLocation(const std::string& name, uint32_t shaderId);
        static glm::vec2   ConvertPixelsToOpenGLValues(const glm::vec2& value_in_pixels);
        static glm::vec2   ConvertOpenGLValuesToPixels(const glm::vec2& opengl_coords);
        static float       ConvertXAxisPixelValueToOpenGLValue(int X);
        static float       ConvertYAxisPixelValueToOpenGLValue(int Y);
        static void        SetQuadZIndex(uint32_t* quadId, float z);
        static void        SetQuadPosition(uint32_t* quadId, const glm::vec2& position_in_pixels);
        static void        SetQuadDimensions(uint32_t* quadId, const glm::vec2& dimensions_in_pixels);
        static void        SetQuadColor(uint32_t* quadId, const glm::vec4& color);
        static void        AddQuad(const QuadCreateInfo& quadCreateInfo);
        static void        AddText(const std::string& text, const glm::vec2& position_in_pixels, float scale, const glm::vec4& color);
        // Currently doesn't work
        static void        RemoveQuad(uint32_t* quadId);
        static void        OnUpdate();
        static void        OnDraw();
        static void        ChangeQuadVertices(uint32_t* quadId, const std::array<Vertex, 4>& vertices);
        static void        ChangeQuadVertices(const QuadCreateInfo& quadCreateInfo);
        static void        CleanUp();

        /// Functions For Debug Purposes
        static void        PrintQuadDictionary();
        static float       PrintTextWidth()
        {
            TextHelper textHelper("Flameberry Engine", s_Characters, 1.0f);
            return textHelper.GetTextWidth();
        }

        static glm::vec2& GetCursorPosition();
        static std::array<Vertex*, 4>               GetPtrToQuadVertices(uint32_t* quadId);
        static std::tuple<std::string, std::string> ReadShaderSource(const std::string& filePath);
    private:
        /// Private Functions which will be used by the Renderer as Utilites
        static uint32_t    GenQuadId();
        static void        LoadFont(const std::string& filePath);
        static void        GetQuadVertices(std::array<Vertex, 4>* vertices, const QuadPosType& positionType, const glm::vec2& position_in_pixels, const glm::vec2& dimensions_in_pixels, const glm::vec4& color, float z, float elementTypeIndex);
        static void        LoadTexture(uint32_t* quadId, const std::string& filePath);
        static void        AddBasicQuad(const QuadCreateInfo& quadCreateInfo);
        static void        AddTexturedQuad(const QuadCreateInfo& quadCreateInfo);
        static void        AddQuadToTextBatch(uint32_t* quadId, const std::array<FlameUI::Vertex, 4>& vertices, uint32_t textureId);
    private:
        /// Struct that contains all the matrices needed by the shader, which will be stored in a Uniform Buffer
        struct UniformBufferData { glm::mat4 ProjectionMatrix; };
        struct FontProps
        {
            float Scale;
            float Strength;
            float PixelRange;
            float AscenderY;
            float DescenderY;
        };
    public:
        static FontProps& GetFontProps() { return s_FontProps; }
    private:
        /// Stores the window content scale, useful for correct scaling on retina displays
        static glm::vec2                                 s_WindowContentScale;
        /// AspectRatio used for converting pixel coordinates to opengl coordinates
        static float                                     s_AspectRatio;
        /// The RendererId needed for the Uniform Buffer
        static uint32_t                                  s_UniformBufferId;
        /// Contains Font properties of the main UI font
        static FontProps                                 s_FontProps;
        /// Stores all matrices needed by the shader, also stored in a Uniform Buffer
        static UniformBufferData                         s_UniformBufferData;
        /// Stores all the colors of the UI
        static ThemeInfo                                 s_ThemeInfo;
        /// Stores file path of the font provided by user and the default font file path
        static std::string                               s_UserFontFilePath;
        /// The main vector of all the batches of quads to ever exist in the program
        static std::vector<std::shared_ptr<Batch>>       s_Batches;
        /// Stores all the characters and their properties, which are extracted from the font provided by the user
        static std::unordered_map<char, flame::character>       s_Characters;
        /// Stores the size of the vieport that FlameUI is being drawn on
        static glm::vec2                                 s_ViewportSize;
        /// Stores the cursor position per frame on the User Window
        static glm::vec2                                 s_CursorPosition;
        /// Stores the GLFWwindow where FlameUI is being drawn
        static GLFWwindow* s_UserWindow;

        /// Stores QuadId and array of two unsigned ints,
        /// first is the batch index in the `s_Batches` vector and second is the index at which the vertices of the quad start,
        /// in stored in the corresponding batch's `Vertices` vector
        /// The QuadId is generated using the `GenQuadId()` function, which is an unique ID assigned to every quad
        static std::unordered_map<uint32_t, uint32_t[2]> s_QuadDictionary;
        /// Stores the uniform location in a shader if the location needs to be reused
        static std::unordered_map<std::string, GLint>    m_UniformLocationCache;
    };
}
