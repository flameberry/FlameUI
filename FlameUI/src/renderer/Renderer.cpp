#include "Renderer.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "utils/Timer.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

#include "msdfgen/msdfgen.h"
#include "msdfgen/msdfgen-ext.h"

namespace FlameUI {
    std::unordered_map<std::string, GLint>     Renderer::s_UniformLocationCache;
    std::unordered_map<std::string, uint32_t>  Renderer::s_TextureIdCache;
    std::unordered_map<char, flame::character> Renderer::s_Characters;
    Renderer::Batch                            Renderer::s_Batch;
    uint32_t                                   Renderer::s_UniformBufferId;
    glm::vec2                                  Renderer::s_WindowContentScale;
    float                                      Renderer::s_AspectRatio = (float)(1280.0f / 720.0f);
    std::string                                Renderer::s_UserFontFilePath = "";
    Renderer::UniformBufferData                Renderer::s_UniformBufferData;
    ThemeInfo                                  Renderer::s_ThemeInfo{};
    Renderer::FontProps                        Renderer::s_FontProps = { .Scale = 1.0f, .Strength = 0.5f, .PixelRange = 8.0f };
    glm::vec2                                  Renderer::s_ViewportSize = { 1280.0f, 720.0f };
    glm::vec2                                  Renderer::s_CursorPosition = { 0.0f, 0.0f };
    float                                      Renderer::s_CurrentTextureSlot = 0;
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

        InitBatch();
        FL_INFO("Initialized Renderer!");
    }

    void Renderer::InitBatch()
    {
        s_Batch.TextureIds.reserve(MAX_TEXTURE_SLOTS);

        uint32_t indices[MAX_INDICES];
        size_t offset = 0;
        for (size_t i = 0; i < MAX_INDICES; i += 6)
        {
            indices[0 + i] = 1 + offset;
            indices[1 + i] = 2 + offset;
            indices[2 + i] = 3 + offset;

            indices[3 + i] = 3 + offset;
            indices[4 + i] = 0 + offset;
            indices[5 + i] = 1 + offset;

            offset += 4;
        }

        glGenVertexArrays(1, &s_Batch.VertexArrayId);
        glBindVertexArray(s_Batch.VertexArrayId);

        glGenBuffers(1, &s_Batch.VertexBufferId);
        glBindBuffer(GL_ARRAY_BUFFER, s_Batch.VertexBufferId);
        glBufferData(GL_ARRAY_BUFFER, MAX_VERTICES * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

        glBindVertexArray(s_Batch.VertexArrayId);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)offsetof(Vertex, position));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)offsetof(Vertex, color));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)offsetof(Vertex, texture_uv));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)offsetof(Vertex, texture_index));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)offsetof(Vertex, quad_dimensions));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)offsetof(Vertex, element_type_index));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 1, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)offsetof(Vertex, is_panel_active));

        glGenBuffers(1, &s_Batch.IndexBufferId);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_Batch.IndexBufferId);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glBindVertexArray(s_Batch.VertexArrayId);

        auto [vertexSource, fragmentSource] = Renderer::ReadShaderSource(FL_PROJECT_DIR + std::string("FlameUI/resources/shaders/Quad.glsl"));
        // Create an empty vertex shader handle
        GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);

        // Send the vertex shader source code to GL
        // Note that std::string's .c_str is NULL character terminated.
        const GLchar* source = (const GLchar*)vertexSource.c_str();
        glShaderSource(vertex_shader, 1, &source, 0);

        // Compile the vertex shader
        glCompileShader(vertex_shader);

        GLint isCompiled = 0;
        glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &isCompiled);
        if (isCompiled == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &maxLength);

            // The maxLength includes the NULL character
            std::vector<GLchar> infoLog(maxLength);
            glGetShaderInfoLog(vertex_shader, maxLength, &maxLength, &infoLog[0]);

            // We don't need the shader anymore.
            glDeleteShader(vertex_shader);

            FL_ERROR("Error compiling VERTEX shader:\n{0}", infoLog.data());
        }

        // Create an empty fragment shader handle
        GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

        // Send the fragment shader source code to GL
        // Note that std::string's .c_str is NULL character terminated.
        source = (const GLchar*)fragmentSource.c_str();
        glShaderSource(fragment_shader, 1, &source, 0);

        // Compile the fragment shader
        glCompileShader(fragment_shader);

        glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &isCompiled);
        if (isCompiled == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &maxLength);

            // The maxLength includes the NULL character
            std::vector<GLchar> infoLog(maxLength);
            glGetShaderInfoLog(fragment_shader, maxLength, &maxLength, &infoLog[0]);

            // We don't need the shader anymore.
            glDeleteShader(fragment_shader);
            // Either of them. Don't leak shaders.
            glDeleteShader(vertex_shader);

            FL_ERROR("Error compiling FRAGMENT shader:\n{0}", infoLog.data());
        }

        // Vertex and fragment shaders are successfully compiled.
        // Now time to link them together into a program.
        // Get a program object.
        s_Batch.ShaderProgramId = glCreateProgram();

        // Attach our shaders to our program
        glAttachShader(s_Batch.ShaderProgramId, vertex_shader);
        glAttachShader(s_Batch.ShaderProgramId, fragment_shader);

        // Link our program
        glLinkProgram(s_Batch.ShaderProgramId);

        // Note the different functions here: glGetProgram* instead of glGetShader*.
        GLint isLinked = 0;
        glGetProgramiv(s_Batch.ShaderProgramId, GL_LINK_STATUS, (int*)&isLinked);
        if (isLinked == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetProgramiv(s_Batch.ShaderProgramId, GL_INFO_LOG_LENGTH, &maxLength);

            // The maxLength includes the NULL character
            std::vector<GLchar> infoLog(maxLength);
            glGetProgramInfoLog(s_Batch.ShaderProgramId, maxLength, &maxLength, &infoLog[0]);

            // We don't need the program anymore.
            glDeleteProgram(s_Batch.ShaderProgramId);
            // Don't leak shaders either.
            glDeleteShader(vertex_shader);
            glDeleteShader(fragment_shader);

            FL_ERROR("Error linking shader program:\n{0}", infoLog.data());
        }

        // Always detach shaders after a successful link.
        glDetachShader(s_Batch.ShaderProgramId, vertex_shader);
        glDetachShader(s_Batch.ShaderProgramId, fragment_shader);

        glUseProgram(s_Batch.ShaderProgramId);

        int samplers[MAX_TEXTURE_SLOTS];
        for (uint32_t i = 0; i < MAX_TEXTURE_SLOTS; i++)
            samplers[i] = i;
        glUniform1iv(Renderer::GetUniformLocation("u_TextureSamplers", s_Batch.ShaderProgramId), MAX_TEXTURE_SLOTS, samplers);
        glUseProgram(0);
    }

    void Renderer::FlushBatch()
    {
        if (!s_Batch.Vertices.size())
            return;

        glBindBuffer(GL_ARRAY_BUFFER, s_Batch.VertexBufferId);
        glBufferSubData(GL_ARRAY_BUFFER, 0, s_Batch.Vertices.size() * sizeof(Vertex), s_Batch.Vertices.data());

        for (uint8_t i = 0; i < s_Batch.TextureIds.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, s_Batch.TextureIds[i]);
        }

        glUseProgram(s_Batch.ShaderProgramId);
        glUniform1f(Renderer::GetUniformLocation("u_TitleBarHeight", s_Batch.ShaderProgramId), Renderer::ConvertYAxisPixelValueToOpenGLValue(TITLE_BAR_HEIGHT));

        glUniform4f(
            Renderer::GetUniformLocation("u_PanelTitleBarActiveColor", s_Batch.ShaderProgramId),
            s_ThemeInfo.panelTitleBarActiveColor.x,
            s_ThemeInfo.panelTitleBarActiveColor.y,
            s_ThemeInfo.panelTitleBarActiveColor.z,
            s_ThemeInfo.panelTitleBarActiveColor.w
        );
        glUniform4f(
            Renderer::GetUniformLocation("u_PanelTitleBarInactiveColor", s_Batch.ShaderProgramId),
            s_ThemeInfo.panelTitleBarInactiveColor.x,
            s_ThemeInfo.panelTitleBarInactiveColor.y,
            s_ThemeInfo.panelTitleBarInactiveColor.z,
            s_ThemeInfo.panelTitleBarInactiveColor.w
        );
        glUniform4f(
            Renderer::GetUniformLocation("u_PanelBgColor", s_Batch.ShaderProgramId),
            s_ThemeInfo.panelBgColor.x,
            s_ThemeInfo.panelBgColor.y,
            s_ThemeInfo.panelBgColor.z,
            s_ThemeInfo.panelBgColor.w
        );
        glUniform4f(
            Renderer::GetUniformLocation("u_BorderColor", s_Batch.ShaderProgramId),
            s_ThemeInfo.borderColor.x,
            s_ThemeInfo.borderColor.y,
            s_ThemeInfo.borderColor.z,
            s_ThemeInfo.borderColor.w
        );

        glBindVertexArray(s_Batch.VertexArrayId);
        glDrawElements(GL_TRIANGLES, (s_Batch.Vertices.size() / 4) * 6, GL_UNSIGNED_INT, 0);

        s_Batch.Vertices.clear();
        s_Batch.TextureIds.clear();
    }

    void Renderer::AddQuad(const glm::vec3& position, const glm::vec2& dimensions, const glm::vec4& color, const float elementTypeIndex, UnitType unitType, bool isPanelActive)
    {
        Vertex vertices[4];

        vertices[0].texture_uv = { 0.0f, 0.0f };
        vertices[1].texture_uv = { 0.0f, 1.0f };
        vertices[2].texture_uv = { 1.0f, 1.0f };
        vertices[3].texture_uv = { 1.0f, 0.0f };

        glm::mat4 transformation{ 1.0f };

        switch (unitType)
        {
            case UnitType::PIXEL_UNITS:
                transformation = glm::translate(glm::mat4(1.0f), { ConvertPixelsToOpenGLValues({ position.x, position.y }), position.z });
                transformation = glm::scale(transformation, { ConvertPixelsToOpenGLValues(dimensions), 0.0f });
                break;
            case UnitType::OPENGL_UNITS:
                transformation = glm::translate(glm::mat4(1.0f), position);
                transformation = glm::scale(transformation, { dimensions, 0.0f });
                break;
            default:
                break;
        }

        for (uint8_t i = 0; i < 4; i++)
        {
            vertices[i].position = transformation * s_TemplateVertexPositions[i];
            vertices[i].element_type_index = elementTypeIndex;
            vertices[i].color = color;
            vertices[i].quad_dimensions = ConvertPixelsToOpenGLValues(dimensions);
            vertices[i].is_panel_active = isPanelActive ? 1.0f : 0.0f;
        }

        for (uint8_t i = 0; i < 4; i++)
            s_Batch.Vertices.push_back(vertices[i]);
    }

    void Renderer::AddQuad(const glm::vec3& position, const glm::vec2& dimensions, const glm::vec4& color, const float elementTypeIndex, const char* textureFilePath, UnitType unitType, bool isPanelActive)
    {
        Vertex vertices[4];
        vertices[0].texture_uv = { 0.0f, 0.0f };
        vertices[1].texture_uv = { 0.0f, 1.0f };
        vertices[2].texture_uv = { 1.0f, 1.0f };
        vertices[3].texture_uv = { 1.0f, 0.0f };

        glm::mat4 transformation{ 1.0f };

        switch (unitType)
        {
            case UnitType::PIXEL_UNITS:
                transformation = glm::translate(glm::mat4(1.0f), { ConvertPixelsToOpenGLValues({ position.x, position.y }), position.z });
                transformation = glm::scale(transformation, { ConvertPixelsToOpenGLValues(dimensions), 0.0f });
                break;
            case UnitType::OPENGL_UNITS:
                transformation = glm::translate(glm::mat4(1.0f), position);
                transformation = glm::scale(transformation, { dimensions, 0.0f });
                break;
            default:
                break;
        }

        for (uint8_t i = 0; i < 4; i++)
        {
            vertices[i].position = transformation * s_TemplateVertexPositions[i];
            vertices[i].element_type_index = elementTypeIndex;
            vertices[i].color = color;
            vertices[i].quad_dimensions = ConvertPixelsToOpenGLValues(dimensions);
            vertices[i].texture_index = s_CurrentTextureSlot;
            vertices[i].is_panel_active = isPanelActive ? 1.0f : 0.0f;
        }

        for (uint8_t i = 0; i < 4; i++)
            s_Batch.Vertices.push_back(vertices[i]);

        uint32_t textureId = GetTextureIdIfAvailable(textureFilePath);
        if (!textureId)
        {
            textureId = CreateTexture(textureFilePath);
            s_TextureIdCache[textureFilePath] = textureId;
        }
        s_Batch.TextureIds.push_back(textureId);

        // Increment the texture slot every time a textured quad is added
        s_CurrentTextureSlot++;
        if (s_CurrentTextureSlot == MAX_TEXTURE_SLOTS)
        {
            FlushBatch();
            s_CurrentTextureSlot = 0;
        }
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

            // AddQuadToTextBatch(nullptr, vertices, character.textureId);
            position.x += (character.advance) * scale + 1.0f;
        }
    }

    void Renderer::Begin()
    {
        OnUpdate();

        /* Set Projection Matrix in GPU memory, for all shader programs to access it */
        glBindBuffer(GL_UNIFORM_BUFFER, s_UniformBufferId);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(s_UniformBufferData.ProjectionMatrix));
    }

    void Renderer::End()
    {
        FlushBatch();
        s_CurrentTextureSlot = 0;
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
            glGenTextures(1, &textureId);
            glBindTexture(GL_TEXTURE_2D, textureId);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, msdf.width(), msdf.height(), 0, GL_RGB, GL_FLOAT, msdf.operator float* ());

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
        position_in_opengl_coords.x = value_in_pixels.x * ((2.0f * s_AspectRatio) / s_ViewportSize.x) * s_WindowContentScale.x;
        position_in_opengl_coords.y = value_in_pixels.y * (2.0f / s_ViewportSize.y) * s_WindowContentScale.y;
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
        return static_cast<float>(X) * ((2.0f * s_AspectRatio) / s_ViewportSize.x) * s_WindowContentScale.x;
    }

    float Renderer::ConvertYAxisPixelValueToOpenGLValue(int Y)
    {
        return static_cast<float>(Y) * (2.0f / s_ViewportSize.y) * s_WindowContentScale.y;
    }

    uint32_t Renderer::CreateTexture(const std::string& filePath)
    {
        // stbi_set_flip_vertically_on_load(true);

        // int width, height, channels;
        // unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &channels, 0);

        // FL_ASSERT(data, "Failed to load texture from \"{0}\"", filePath);

        // GLenum internalFormat = 0, dataFormat = 0;
        // if (channels == 4)
        // {
        //     internalFormat = GL_RGBA8;
        //     dataFormat = GL_RGBA;
        // }
        // else if (channels == 3)
        // {
        //     internalFormat = GL_RGB8;
        //     dataFormat = GL_RGB;
        // }

        // uint32_t textureId;
        // glGenTextures(1, &textureId);
        // glBindTexture(GL_TEXTURE_2D, textureId);

        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        // glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
        // glGenerateMipmap(GL_TEXTURE_2D);

        // stbi_image_free(data);

        uint32_t* data = new uint32_t[100 * 100];
        for (uint32_t i = 0; i < 100 * 100; i++)
            data[i] = 0xffff00ff;

        uint32_t textureId;
        GL_CHECK_ERROR(glGenTextures(1, &textureId));
        glActiveTexture(GL_TEXTURE0);
        GL_CHECK_ERROR(glBindTexture(GL_TEXTURE_2D, textureId));

        // Set parameters to determine how the texture is resized
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Set parameters to determine how the texture wraps at edges
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        GL_CHECK_ERROR(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 100, 100, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, data));
        glGenerateMipmap(GL_TEXTURE_2D);
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
        if (s_UniformLocationCache.find(name) != s_UniformLocationCache.end())
            return s_UniformLocationCache[name];

        GLint location = glGetUniformLocation(shaderId, name.c_str());
        if (location == -1)
            FL_WARN("Uniform \"{0}\" not found!", name);
        s_UniformLocationCache[name] = location;
        return location;
    }

    uint32_t Renderer::GetTextureIdIfAvailable(const char* textureFilePath)
    {
        if (s_TextureIdCache.find(textureFilePath) != s_TextureIdCache.end())
            return s_TextureIdCache[textureFilePath];
        else
            return 0;
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
