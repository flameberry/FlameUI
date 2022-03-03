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
    enum class UnitType
    {
        NONE = 0, PIXEL_UNITS, OPENGL_UNITS
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
        static ThemeInfo   GetThemeInfo() { return s_ThemeInfo; }
        static glm::vec2   GetWindowContentScale() { return s_WindowContentScale; }
        static GLFWwindow* GetUserGLFWwindow();
        static glm::vec2   GetViewportSize();
        static glm::vec2   GetTextDimensions(const std::string& text, float scale);
        static GLint       GetUniformLocation(const std::string& name, uint32_t shaderId);
        static float       GetAspectRatio() { return s_AspectRatio; }
        static glm::vec2   ConvertPixelsToOpenGLValues(const glm::vec2& value_in_pixels);
        static glm::vec2   ConvertOpenGLValuesToPixels(const glm::vec2& opengl_coords);
        static float       ConvertXAxisPixelValueToOpenGLValue(int X);
        static float       ConvertYAxisPixelValueToOpenGLValue(int Y);
        static void        AddQuad(const glm::vec3& position, const glm::vec2& dimensions, const glm::vec4& color, const float elementTypeIndex, const char* textureFilePath, UnitType unitType = UnitType::PIXEL_UNITS);
        static void        AddQuad(const glm::vec3& position, const glm::vec2& dimensions, const glm::vec4& color, const float elementTypeIndex, UnitType unitType = UnitType::PIXEL_UNITS);
        static void        AddText(const std::string& text, const glm::vec2& position_in_pixels, float scale, const glm::vec4& color);
        static void        CleanUp();
        static uint32_t    CreateTexture(const std::string& filePath);

        static void Begin();
        static void End();

        static glm::vec2& GetCursorPosition();
        static std::tuple<std::string, std::string> ReadShaderSource(const std::string& filePath);
    private:
        /// Private Functions which will be used by the Renderer as Utilites
        static void     OnUpdate();
        static void     LoadFont(const std::string& filePath);
        static void     GetQuadVertices(std::array<Vertex, 4>* vertices, const QuadPosType& positionType, const glm::vec2& position_in_pixels, const glm::vec2& dimensions_in_pixels, const glm::vec4& color, float z, float elementTypeIndex);
        static uint32_t GetTextureIdIfAvailable(const char* textureFilePath);
    private:
        /// Struct that contains all the matrices needed by the shader, which will be stored in a Uniform Buffer
        struct UniformBufferData { glm::mat4 ProjectionMatrix; };
        struct TextureUniformBufferData { int Samplers[MAX_TEXTURE_SLOTS]; };
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
        static Batch                                     s_Batch;
        /// Stores all the characters and their properties, which are extracted from the font provided by the user
        static std::unordered_map<char, flame::character>s_Characters;
        /// Stores the size of the vieport that FlameUI is being drawn on
        static glm::vec2                                 s_ViewportSize;
        /// Stores the cursor position per frame on the User Window
        static glm::vec2                                 s_CursorPosition;
        /// Stores the GLFWwindow where FlameUI is being drawn
        static GLFWwindow* s_UserWindow;
        /// Stores the uniform location in a shader if the location needs to be reused
        static std::unordered_map<std::string, GLint>    s_UniformLocationCache;
        /// Stores the texture IDs of the already loaded textures to be reused
        static std::unordered_map<std::string, uint32_t> s_TextureIdCache;

        static float s_CurrentTextureSlot;

        constexpr static glm::vec4 s_TemplateVertexPositions[4] = {
             {-0.5f, -0.5f, 0.0f, 1.0f},
             {-0.5f,  0.5f, 0.0f, 1.0f},
             { 0.5f,  0.5f, 0.0f, 1.0f},
             { 0.5f, -0.5f, 0.0f, 1.0f}
        };
    };
}
