#include "Renderer.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../utils/Timer.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

#include "msdfgen/msdfgen.h"
#include "msdfgen/msdfgen-ext.h"

namespace FlameUI {
    std::unordered_map<std::string, GLint>     Renderer::m_UniformLocationCache;
    std::unordered_map<char, flame::character> Renderer::s_Characters;
    Batch                                      Renderer::s_Batch;
    uint32_t                                   Renderer::s_UniformBufferId;
    glm::vec2                                  Renderer::s_WindowContentScale;
    float                                      Renderer::s_AspectRatio = (float)(1280.0f / 720.0f);
    std::string                                Renderer::s_UserFontFilePath = "";
    Renderer::UniformBufferData                Renderer::s_UniformBufferData;
    ThemeInfo                                  Renderer::s_ThemeInfo{};
    Renderer::FontProps                        Renderer::s_FontProps = { .Scale = 1.0f, .Strength = 0.5f, .PixelRange = 8.0f };
    glm::vec2                                  Renderer::s_ViewportSize = { 1280.0f, 720.0f };
    glm::vec2                                  Renderer::s_CursorPosition = { 0.0f, 0.0f };
    GLFWwindow* Renderer::s_UserWindow;

    void Renderer::OnResize()
    {
        int width, height;
        glfwGetFramebufferSize(s_UserWindow, &width, &height);
        s_ViewportSize = { width, height };
        s_AspectRatio = s_ViewportSize.x / s_ViewportSize.y;
        s_UniformBufferData.ProjectionMatrix = glm::ortho(-s_AspectRatio, s_AspectRatio, -1.0f, 1.0f, -1.0f, 1.0f);

        glViewport(0, 0, s_ViewportSize.x, s_ViewportSize.y);
    }

    glm::vec2  Renderer::GetViewportSize() { return s_ViewportSize; }
    glm::vec2& Renderer::GetCursorPosition() { return s_CursorPosition; }
    GLFWwindow* Renderer::GetUserGLFWwindow() { return s_UserWindow; }

    void Renderer::OnUpdate()
    {
        glClear(GL_DEPTH_BUFFER_BIT);

        glm::vec2 scale;
        glfwGetWindowContentScale(s_UserWindow, &scale.x, &scale.y);
        s_WindowContentScale = scale;

        double x, y;
        glfwGetCursorPos(s_UserWindow, &x, &y);
        s_CursorPosition.x = x - s_ViewportSize.x / s_WindowContentScale.x / 2.0f;
        s_CursorPosition.y = -y + s_ViewportSize.y / s_WindowContentScale.y / 2.0f;
        FL_LOG("Cursor position is ({0}, {1})", s_CursorPosition.x, s_CursorPosition.y);
        OnResize();
    }

    void Renderer::Init(const RendererInitInfo& rendererInitInfo)
    {
        FL_LOGGER_INIT();
        FL_INFO("Initialized Logger!");

        s_UserWindow = rendererInitInfo.userWindow;
        if (rendererInitInfo.themeInfo)
            s_ThemeInfo = *rendererInitInfo.themeInfo;

        GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
        const char* monitorName = glfwGetMonitorName(primaryMonitor);
        FL_INFO("Primary Monitor: {0}", monitorName);

        glm::vec2 scale;
        glfwGetWindowContentScale(s_UserWindow, &scale.x, &scale.y);
        s_WindowContentScale = scale;

        int width, height;
        glfwGetFramebufferSize(s_UserWindow, &width, &height);
        s_ViewportSize = { (float)width, (float)height };

        if (rendererInitInfo.enableFontRendering)
        {
            s_UserFontFilePath = rendererInitInfo.fontFilePath;
            LoadFont(s_UserFontFilePath);
            FL_INFO("Loaded Font from path \"{0}\"", s_UserFontFilePath);
        }

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_DEPTH_TEST);

        /* Create Uniform Buffer */
        glGenBuffers(1, &s_UniformBufferId);
        glBindBuffer(GL_UNIFORM_BUFFER, s_UniformBufferId);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(UniformBufferData), nullptr, GL_DYNAMIC_DRAW);
        glBindBufferRange(GL_UNIFORM_BUFFER, 0, s_UniformBufferId, 0, sizeof(UniformBufferData));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        s_Batch.Init();

        FL_INFO("Initialized Renderer!");
    }

    void Renderer::GetQuadVertices(
        std::array<Vertex, 4>* vertices,
        const QuadPosType& positionType,
        const glm::vec2& position_in_pixels,
        const glm::vec2& dimensions_in_pixels,
        const glm::vec4& color,
        float z,
        float elementTypeIndex
    )
    {
        glm::vec2 position = ConvertPixelsToOpenGLValues(position_in_pixels);
        glm::vec2 dimensions = ConvertPixelsToOpenGLValues(dimensions_in_pixels);

        Vertex Vertices[4];

        switch (positionType)
        {
        case FL_QUAD_POS_BOTTOM_LEFT_VERTEX:
            (*vertices)[0].position = { position.x,                position.y,                z };
            (*vertices)[1].position = { position.x,                position.y + dimensions.y, z };
            (*vertices)[2].position = { position.x + dimensions.x, position.y + dimensions.y, z };
            (*vertices)[3].position = { position.x + dimensions.x, position.y,                z };
            break;
        case FL_QUAD_POS_CENTER:
            (*vertices)[0].position = { -(dimensions.x / 2.0f) + position.x, -(dimensions.y / 2.0f) + position.y, z };
            (*vertices)[1].position = { -(dimensions.x / 2.0f) + position.x, +(dimensions.y / 2.0f) + position.y, z };
            (*vertices)[2].position = { +(dimensions.x / 2.0f) + position.x, +(dimensions.y / 2.0f) + position.y, z };
            (*vertices)[3].position = { +(dimensions.x / 2.0f) + position.x, -(dimensions.y / 2.0f) + position.y, z };
            break;
        default:
            break;
        }

        for (auto& vertex : (*vertices))
        {
            vertex.position.x *= s_WindowContentScale.x;
            vertex.position.y *= s_WindowContentScale.y;
            vertex.quad_dimensions = dimensions;
            vertex.color = color;
            vertex.texture_index = -1.0f;
            vertex.element_type_index = elementTypeIndex;
        }

        (*vertices)[0].texture_uv = { 0.0f, 0.0f };
        (*vertices)[1].texture_uv = { 0.0f, 1.0f };
        (*vertices)[2].texture_uv = { 1.0f, 1.0f };
        (*vertices)[3].texture_uv = { 1.0f, 0.0f };
    }

    void Renderer::AddQuad(const glm::vec3& position, const glm::vec2& dimensions, const glm::vec4& color, const float elementTypeIndex)
    {
        Vertex vertices[4];

        vertices[0].texture_uv = { 0.0f, 0.0f };
        vertices[1].texture_uv = { 0.0f, 1.0f };
        vertices[2].texture_uv = { 1.0f, 1.0f };
        vertices[3].texture_uv = { 1.0f, 0.0f };

        for (uint8_t i = 0; i < 4; i++)
        {
            vertices[i].position = glm::translate(glm::scale(glm::mat4(1.0f), { ConvertPixelsToOpenGLValues(dimensions), 0.0f }), { ConvertXAxisPixelValueToOpenGLValue(position.x), ConvertYAxisPixelValueToOpenGLValue(position.y), position.z }) * s_TemplateVertexPositions[i];
            vertices[i].position.x *= s_WindowContentScale.x;
            vertices[i].position.y *= s_WindowContentScale.y;
            vertices[i].element_type_index = elementTypeIndex;
            vertices[i].color = color;
            vertices[i].quad_dimensions = ConvertPixelsToOpenGLValues(dimensions);
        }

        s_Batch.AddQuad(vertices);
    }

    void Renderer::AddText(const std::string& text, const glm::vec2& position_in_pixels, float scale, const glm::vec4& color)
    {
        static uint16_t slot = 0;
        auto position = position_in_pixels;

        for (std::string::const_iterator it = text.begin(); it != text.end(); it++)
        {
            flame::character character = s_Characters[*it];

            float xpos = position.x + character.bearing.x * scale;
            float ypos = position.y - (character.size.y - character.bearing.y) * scale - s_FontProps.DescenderY * scale;

            float w = character.size.x * scale;
            float h = character.size.y * scale;

            std::array<FlameUI::Vertex, 4> vertices;
            vertices[0].position = { xpos,     ypos,     0.0f };
            vertices[1].position = { xpos,     ypos + h, 0.0f };
            vertices[2].position = { xpos + w, ypos + h, 0.0f };
            vertices[3].position = { xpos + w, ypos,     0.0f };

            for (auto& vertex : vertices)
            {
                vertex.position.x = ConvertXAxisPixelValueToOpenGLValue(vertex.position.x);
                vertex.position.y = ConvertYAxisPixelValueToOpenGLValue(vertex.position.y);
            }

            vertices[0].texture_uv = { 0.0f, 0.0f };
            vertices[1].texture_uv = { 0.0f, 1.0f };
            vertices[2].texture_uv = { 1.0f, 1.0f };
            vertices[3].texture_uv = { 1.0f, 0.0f };

            for (auto& vertex : vertices)
                vertex.texture_index = (float)slot;

            slot++;
            if (slot == MAX_TEXTURE_SLOTS)
                slot = 0;

            for (uint8_t i = 0; i < 4; i++)
                vertices[i].color = color;

            position.x += (character.advance) * scale + 1.0f;
        }
    }

    void Renderer::Begin()
    {
        OnUpdate();

        /* Set Projection Matrix in GPU memory, for all shader programs to access it */
        GL_CHECK_ERROR(glBindBuffer(GL_UNIFORM_BUFFER, s_UniformBufferId));
        GL_CHECK_ERROR(glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(s_UniformBufferData.ProjectionMatrix)));
    }

    void Renderer::End()
    {
        s_Batch.OnDraw();
        s_Batch.Empty();
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    void Renderer::LoadFont(const std::string& filePath)
    {
        msdfgen::FreetypeHandle* ft = msdfgen::initializeFreetype();
        FL_ASSERT(ft, "Failed to initialize Font Handle!");

        msdfgen::FontHandle* msdffontHandle = loadFont(ft, filePath.c_str());
        FL_ASSERT(msdffontHandle, "Failed to load font from the file path: {0}", filePath);

        double border_width = 2.0;

        msdfgen::FontMetrics metrics;
        msdfgen::getFontMetrics(metrics, msdffontHandle);
        s_FontProps.AscenderY = metrics.ascenderY;
        s_FontProps.DescenderY = metrics.descenderY;

        for (uint8_t character = 32; character < 128; character++)
        {
            double advance = 0.0;
            msdfgen::Shape shape;
            FL_ASSERT(msdfgen::loadGlyph(shape, msdffontHandle, character, &advance), "Failed to load Glyph!");

            shape.normalize();

            auto bounds = shape.getBounds(border_width);
            uint32_t glyphWidth = ceil(bounds.r - bounds.l);
            uint32_t glyphHeight = ceil(bounds.t - bounds.b);

            msdfgen::edgeColoringSimple(shape, 3.0);
            msdfgen::Bitmap<float, 3> msdf(glyphWidth, glyphHeight);

            msdfgen::generateMSDF(msdf, shape, s_FontProps.PixelRange, 1.0, msdfgen::Vector2(-bounds.l, -bounds.b));

            uint32_t textureId;
            GL_CHECK_ERROR(glGenTextures(1, &textureId));
            GL_CHECK_ERROR(glBindTexture(GL_TEXTURE_2D, textureId));

            GL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
            GL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
            GL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
            GL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

            GL_CHECK_ERROR(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, msdf.width(), msdf.height(), 0, GL_RGB, GL_FLOAT, msdf.operator float* ()));

            flame::character ch = {
                textureId,
                { msdf.width(), msdf.height() },
                { shape.getBounds().l, shape.getBounds().t },
                advance,
            };

            s_Characters[character] = ch;
        }
        msdfgen::destroyFont(msdffontHandle);
        msdfgen::deinitializeFreetype(ft);
    }

    glm::vec2 Renderer::GetTextDimensions(const std::string& text, float scale)
    {
        glm::vec2 textDimensions = { 0, 0 };
        float lowestPositionOfChar = 0;
        float highestPositionOfChar = 0;
        for (std::string::const_iterator it = text.begin(); it != text.end(); it++)
        {
            textDimensions.x += s_Characters[*it].size.x * scale;
            if (lowestPositionOfChar > -(s_Characters[*it].size.y - s_Characters[*it].bearing.y) * scale)
                lowestPositionOfChar = -(s_Characters[*it].size.y - s_Characters[*it].bearing.y) * scale;
            if (highestPositionOfChar < -(s_Characters[*it].size.y - s_Characters[*it].bearing.y) * scale + s_Characters[*it].size.y * scale)
                highestPositionOfChar = -(s_Characters[*it].size.y - s_Characters[*it].bearing.y) * scale + s_Characters[*it].size.y * scale;
        }
        textDimensions.y = highestPositionOfChar - lowestPositionOfChar;

        return textDimensions;
    }

    glm::vec2 Renderer::ConvertPixelsToOpenGLValues(const glm::vec2& value_in_pixels)
    {
        glm::vec2 position_in_opengl_coords;
        position_in_opengl_coords.x = value_in_pixels.x * ((2.0f * s_AspectRatio) / s_ViewportSize.x);
        position_in_opengl_coords.y = value_in_pixels.y * (2.0f / s_ViewportSize.y);
        return position_in_opengl_coords;
    }

    glm::vec2 Renderer::ConvertOpenGLValuesToPixels(const glm::vec2& opengl_coords)
    {
        glm::vec2 value_in_pixels;
        int width, height;
        glfwGetFramebufferSize(s_UserWindow, &width, &height);
        value_in_pixels.x = (int)((opengl_coords.x / (2.0f * s_AspectRatio)) * width);
        value_in_pixels.y = (int)((opengl_coords.y / 2.0f) * height);
        return value_in_pixels;
    }

    float Renderer::ConvertXAxisPixelValueToOpenGLValue(int X)
    {
        return static_cast<float>(X) * ((2.0f * s_AspectRatio) / s_ViewportSize.x);
    }

    float Renderer::ConvertYAxisPixelValueToOpenGLValue(int Y)
    {
        return static_cast<float>(Y) * (2.0f / s_ViewportSize.y);
    }

    uint32_t Renderer::CreateTexture(const std::string& filePath)
    {
        stbi_set_flip_vertically_on_load(true);

        int width, height, channels;
        unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &channels, 0);

        FL_ASSERT(data, "Failed to load texture from \"{0}\"", filePath);

        GLenum internalFormat = 0, dataFormat = 0;
        if (channels == 4)
        {
            internalFormat = GL_RGBA8;
            dataFormat = GL_RGBA;
        }
        else if (channels == 3)
        {
            internalFormat = GL_RGB8;
            dataFormat = GL_RGB;
        }

        uint32_t textureId;
        GL_CHECK_ERROR(glGenTextures(1, &textureId));
        GL_CHECK_ERROR(glBindTexture(GL_TEXTURE_2D, textureId));

        GL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        GL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
        GL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

        GL_CHECK_ERROR(glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data));
        GL_CHECK_ERROR(glGenerateMipmap(GL_TEXTURE_2D));

        stbi_image_free(data);
        return textureId;
    }

    std::tuple<std::string, std::string> Renderer::ReadShaderSource(const std::string& filePath)
    {
        std::ifstream stream(filePath);

        FL_ASSERT(stream.is_open(), "The given shader file {0} cannot be opened", filePath);

        std::stringstream ss[2];
        std::string line;

        uint32_t shader_type = 2;

        while (getline(stream, line))
        {
            if (line.find("#shader") != std::string::npos)
            {
                if (line.find("vertex") != std::string::npos)
                    shader_type = 0;
                else if (line.find("fragment") != std::string::npos)
                    shader_type = 1;
            }
            else
            {
                ss[shader_type] << line << "\n";
            }
        }
        stream.close();
        return std::make_tuple(ss[0].str(), ss[1].str());
    }

    GLint Renderer::GetUniformLocation(const std::string& name, uint32_t shaderId)
    {
        if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
            return m_UniformLocationCache[name];

        GLint location = glGetUniformLocation(shaderId, name.c_str());
        if (location == -1)
            FL_WARN("Uniform \"{0}\" not found!", name);
        m_UniformLocationCache[name] = location;
        return location;
    }

    void Renderer::CleanUp()
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glUseProgram(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
    }
}
