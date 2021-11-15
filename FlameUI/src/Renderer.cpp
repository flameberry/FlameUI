#include "Renderer.h"
#include <iostream>
#include <fstream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

#include "msdfgen/msdfgen.h"
#include "msdfgen/msdfgen-ext.h"

namespace FlameUI {
    std::unordered_map<uint32_t, uint32_t[2]>     Renderer::s_QuadDictionary;
    std::unordered_map<std::string, GLint>        Renderer::m_uniformloc_cache;
    std::unordered_map<char, Renderer::Character> Renderer::s_Characters;
    std::vector<Renderer::Batch>                  Renderer::s_Batches;
    uint32_t                                      Renderer::s_UniformBufferId;
    float                                         Renderer::s_AspectRatio = (float)(1280.0f / 720.0f);
    std::string                                   Renderer::s_UserFontFilePath = "";

    /// FL_PROJECT_DIR is just a macro defined by cmake, which is the absolute file path of the source dir of the FlameUI project
    std::string                                   Renderer::s_DefaultFontFilePath = FL_PROJECT_DIR + std::string("FlameUI/resources/fonts/OpenSans-Regular.ttf");
    Renderer::UniformBufferData                   Renderer::s_UniformBufferData;
    Renderer::FontProps                           Renderer::s_FontProps = { 1.0f, 0.5f, 8.0f };

    ///
    /// Framebuffer Class Function Implementations --------------------------------------------------------------------------------
    ///

    Framebuffer::Framebuffer(float width, float height)
        : m_FramebufferId(0), m_ColorAttachmentId(0), m_DepthAttachmentId(0), m_FramebufferSize(width, height)
    {
        glGenFramebuffers(1, &m_FramebufferId);
        glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferId);

        glGenTextures(1, &m_ColorAttachmentId);
        glBindTexture(GL_TEXTURE_2D, m_ColorAttachmentId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_FramebufferSize.x, m_FramebufferSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachmentId, 0);

        glGenTextures(1, &m_DepthAttachmentId);
        glBindTexture(GL_TEXTURE_2D, m_DepthAttachmentId);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_FramebufferSize.x, m_FramebufferSize.y, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachmentId, 0);

        FL_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Framebuffer::OnUpdate()
    {
        if (m_FramebufferId)
        {
            glDeleteFramebuffers(1, &m_FramebufferId);
            glDeleteTextures(1, &m_ColorAttachmentId);
            glDeleteTextures(1, &m_DepthAttachmentId);
        }

        glGenFramebuffers(1, &m_FramebufferId);
        glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferId);

        glGenTextures(1, &m_ColorAttachmentId);
        glBindTexture(GL_TEXTURE_2D, m_ColorAttachmentId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_FramebufferSize.x, m_FramebufferSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachmentId, 0);

        glGenTextures(1, &m_DepthAttachmentId);
        glBindTexture(GL_TEXTURE_2D, m_DepthAttachmentId);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_FramebufferSize.x, m_FramebufferSize.y, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachmentId, 0);

        FL_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Framebuffer::SetFramebufferSize(float width, float height)
    {
        m_FramebufferSize = { width, height };
    }

    void Framebuffer::Bind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferId);
        glViewport(0, 0, m_FramebufferSize.x, m_FramebufferSize.y);
    }

    void Framebuffer::Unbind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    Framebuffer::~Framebuffer()
    {
        glDeleteTextures(1, &m_ColorAttachmentId);
        glDeleteTextures(1, &m_DepthAttachmentId);
        glDeleteFramebuffers(1, &m_FramebufferId);
    }

    ///
    /// Batch Struct Function Implementations --------------------------------------------------------------------------------
    ///
    Renderer::Batch::Batch()
    {
        Vertices.reserve(s_Max_Vertices_Per_Batch);
    }

    Renderer::Batch::Batch(const BatchType& batchType)
        : CurrentBatchType(batchType)
    {
        Vertices.reserve(s_Max_Vertices_Per_Batch);
    }

    void Renderer::Batch::Init()
    {
        uint32_t indices[s_Max_Indices_Per_Batch];
        size_t offset = 0;
        for (size_t i = 0; i < s_Max_Indices_Per_Batch; i += 6)
        {
            indices[0 + i] = 1 + offset;
            indices[1 + i] = 2 + offset;
            indices[2 + i] = 3 + offset;

            indices[3 + i] = 3 + offset;
            indices[4 + i] = 0 + offset;
            indices[5 + i] = 1 + offset;

            offset += 4;
        }

        GL_CHECK_ERROR(glGenVertexArrays(1, &RendererIds.v_arrayId));
        GL_CHECK_ERROR(glBindVertexArray(RendererIds.v_arrayId));

        GL_CHECK_ERROR(glGenBuffers(1, &RendererIds.v_bufferId));
        GL_CHECK_ERROR(glBindBuffer(GL_ARRAY_BUFFER, RendererIds.v_bufferId));
        GL_CHECK_ERROR(glBufferData(GL_ARRAY_BUFFER, s_Max_Vertices_Per_Batch * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW));

        GL_CHECK_ERROR(glBindVertexArray(RendererIds.v_arrayId));

        GL_CHECK_ERROR(glEnableVertexAttribArray(0));
        GL_CHECK_ERROR(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)offsetof(Vertex, position)));
        GL_CHECK_ERROR(glEnableVertexAttribArray(1));
        GL_CHECK_ERROR(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)offsetof(Vertex, color)));
        GL_CHECK_ERROR(glEnableVertexAttribArray(2));
        GL_CHECK_ERROR(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)offsetof(Vertex, texture_uv)));
        GL_CHECK_ERROR(glEnableVertexAttribArray(3));
        GL_CHECK_ERROR(glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)offsetof(Vertex, texture_index)));

        GL_CHECK_ERROR(glGenBuffers(1, &RendererIds.i_bufferId));
        GL_CHECK_ERROR(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, RendererIds.i_bufferId));
        GL_CHECK_ERROR(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));

        GL_CHECK_ERROR(glBindVertexArray(RendererIds.v_arrayId));

        std::string shaderFilePath = "";
        switch (CurrentBatchType)
        {
        case BatchType::Quad:
            shaderFilePath = FL_PROJECT_DIR + std::string("/FlameUI/resources/shaders/Quad.glsl");
            break;
        case BatchType::Text:
            shaderFilePath = FL_PROJECT_DIR + std::string("/FlameUI/resources/shaders/Font.glsl");
            break;
        default:
            break;
        }

        auto [vertexSource, fragmentSource] = ReadShaderSource(shaderFilePath);
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
        RendererIds.shaderId = glCreateProgram();

        // Attach our shaders to our program
        glAttachShader(RendererIds.shaderId, vertex_shader);
        glAttachShader(RendererIds.shaderId, fragment_shader);

        // Link our program
        glLinkProgram(RendererIds.shaderId);

        // Note the different functions here: glGetProgram* instead of glGetShader*.
        GLint isLinked = 0;
        glGetProgramiv(RendererIds.shaderId, GL_LINK_STATUS, (int*)&isLinked);
        if (isLinked == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetProgramiv(RendererIds.shaderId, GL_INFO_LOG_LENGTH, &maxLength);

            // The maxLength includes the NULL character
            std::vector<GLchar> infoLog(maxLength);
            glGetProgramInfoLog(RendererIds.shaderId, maxLength, &maxLength, &infoLog[0]);

            // We don't need the program anymore.
            glDeleteProgram(RendererIds.shaderId);
            // Don't leak shaders either.
            glDeleteShader(vertex_shader);
            glDeleteShader(fragment_shader);

            FL_ERROR("Error linking shader program:\n{0}", infoLog.data());
        }

        // Always detach shaders after a successful link.
        glDetachShader(RendererIds.shaderId, vertex_shader);
        glDetachShader(RendererIds.shaderId, fragment_shader);

        GL_CHECK_ERROR(glUseProgram(RendererIds.shaderId));
        int samplers[s_Max_Texture_Slots];
        for (uint16_t i = 0; i < s_Max_Texture_Slots; i++)
            samplers[i] = (int)i;
        GL_CHECK_ERROR(glUniform1iv(GetUniformLocation("u_TextureSamplers", RendererIds.shaderId), s_Max_Texture_Slots, samplers));

        if (CurrentBatchType == BatchType::Text)
        {
            GL_CHECK_ERROR(glUniform1f(GetUniformLocation("u_PixelRange", RendererIds.shaderId), s_FontProps.PixelRange));
            GL_CHECK_ERROR(glUniform1f(GetUniformLocation("u_Strength", RendererIds.shaderId), s_FontProps.Strength));
        }
        GL_CHECK_ERROR(glUseProgram(0));
    }

    void Renderer::Batch::OnDraw()
    {
        if (Vertices.size())
        {
            GL_CHECK_ERROR(glBindBuffer(GL_ARRAY_BUFFER, RendererIds.v_bufferId));
            GL_CHECK_ERROR(glBufferSubData(GL_ARRAY_BUFFER, 0, Vertices.size() * sizeof(Vertex), Vertices.data()));

            for (uint8_t i = 0; i < RendererIds.textureIds.size(); i++)
            {
                GL_CHECK_ERROR(glActiveTexture(GL_TEXTURE0 + i));
                GL_CHECK_ERROR(glBindTexture(GL_TEXTURE_2D, RendererIds.textureIds[i]));
            }

            GL_CHECK_ERROR(glUseProgram(RendererIds.shaderId));
            GL_CHECK_ERROR(glBindVertexArray(RendererIds.v_arrayId));
            GL_CHECK_ERROR(glDrawElements(GL_TRIANGLES, s_Max_Indices_Per_Batch, GL_UNSIGNED_INT, 0));
        }
    }

    ///
    /// Renderer Function Implementations --------------------------------------------------------------------------------
    ///
    void Renderer::OnResize(uint32_t window_width, uint32_t window_height)
    {
        s_AspectRatio = ((float)window_width / (float)window_height);
        s_UniformBufferData.ProjectionMatrix = glm::ortho(-s_AspectRatio, s_AspectRatio, -1.0f, 1.0f, -1.0f, 1.0f);

        glViewport(0, 0, window_width, window_height);
    }

    uint32_t Renderer::GenQuadId()
    {
        static uint32_t offset = 0;
        uint32_t id = 15 + offset;
        offset += 2;
        return id;
    }

    void Renderer::Init()
    {
        GL_CHECK_ERROR(glEnable(GL_BLEND));
        GL_CHECK_ERROR(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
        if (s_UserFontFilePath == "")
            s_UserFontFilePath = s_DefaultFontFilePath;
        LoadFont(s_UserFontFilePath);

        /* Create Uniform Buffer */
        GL_CHECK_ERROR(glGenBuffers(1, &s_UniformBufferId));
        GL_CHECK_ERROR(glBindBuffer(GL_UNIFORM_BUFFER, s_UniformBufferId));
        GL_CHECK_ERROR(glBufferData(GL_UNIFORM_BUFFER, sizeof(UniformBufferData), nullptr, GL_DYNAMIC_DRAW));
        GL_CHECK_ERROR(glBindBufferRange(GL_UNIFORM_BUFFER, 0, s_UniformBufferId, 0, sizeof(UniformBufferData)));
        GL_CHECK_ERROR(glBindBuffer(GL_UNIFORM_BUFFER, 0));
    }

    void Renderer::GetQuadVertices(
        std::array<Vertex, 4>* vertices,
        const QuadPosType& positionType,
        const glm::ivec2& position_in_pixels,
        const glm::ivec2& dimensions_in_pixels,
        const glm::vec4& color
    )
    {
        glm::vec2 position = ConvertPixelsToOpenGLValues(position_in_pixels);
        glm::vec2 dimensions = ConvertPixelsToOpenGLValues({ (int)dimensions_in_pixels.x, (int)dimensions_in_pixels.y });

        Vertex Vertices[4];

        switch (positionType)
        {
        case FL_QUAD_POS_BOTTOM_LEFT_VERTEX:
            (*vertices)[0].position = { position.x,                position.y,                0.0f };
            (*vertices)[1].position = { position.x,                position.y + dimensions.y, 0.0f };
            (*vertices)[2].position = { position.x + dimensions.x, position.y + dimensions.y, 0.0f };
            (*vertices)[3].position = { position.x + dimensions.x, position.y,                0.0f };
            break;
        case FL_QUAD_POS_CENTER:
            (*vertices)[0].position = { -(dimensions.x / 2.0f) + position.x, -(dimensions.y / 2.0f) + position.y, 0.0f };
            (*vertices)[1].position = { -(dimensions.x / 2.0f) + position.x, +(dimensions.y / 2.0f) + position.y, 0.0f };
            (*vertices)[2].position = { +(dimensions.x / 2.0f) + position.x, +(dimensions.y / 2.0f) + position.y, 0.0f };
            (*vertices)[3].position = { +(dimensions.x / 2.0f) + position.x, -(dimensions.y / 2.0f) + position.y, 0.0f };
            break;
        default:
            break;
        }

        for (auto& vertex : (*vertices))
        {
            vertex.color = color;
            vertex.texture_index = -1.0f;
        }

        (*vertices)[0].texture_uv = { 0.0f, 0.0f };
        (*vertices)[1].texture_uv = { 0.0f, 1.0f };
        (*vertices)[2].texture_uv = { 1.0f, 1.0f };
        (*vertices)[3].texture_uv = { 1.0f, 0.0f };
    }

    void Renderer::AddQuad(
        uint32_t* quadId,
        const QuadPosType& positionType,
        const glm::ivec2& position_in_pixels,
        const glm::ivec2& dimensions_in_pixels,
        const glm::vec4& color,
        const std::string& textureFilePath
    )
    {
        std::array<Vertex, 4> vertices;
        switch (positionType)
        {
        case FL_QUAD_POS_BOTTOM_LEFT_VERTEX:
            GetQuadVertices(&vertices, FL_QUAD_POS_BOTTOM_LEFT_VERTEX, position_in_pixels, dimensions_in_pixels, color);
            break;
        case FL_QUAD_POS_CENTER:
            GetQuadVertices(&vertices, FL_QUAD_POS_CENTER, position_in_pixels, dimensions_in_pixels, color);
            break;
        default:
            break;
        }

        static uint32_t quad_id = 0;
        if ((!quad_id) || (s_Batches[s_QuadDictionary[quad_id][0]].Vertices.size() == s_Max_Vertices_Per_Batch))
        {
            s_Batches.emplace_back(BatchType::Quad);
            s_Batches.back().Init();

            quad_id = GenQuadId();
            if (quadId)
                *quadId = quad_id;
            s_QuadDictionary[quad_id][0] = s_Batches.size() - 1;
            s_QuadDictionary[quad_id][1] = 0;

            for (auto& vertex : vertices)
                s_Batches.back().Vertices.push_back(vertex);

            LoadTexture(&quad_id, textureFilePath);
        }
        else if ((quad_id) && (s_Batches[s_QuadDictionary[quad_id][0]].Vertices.size() < s_Max_Vertices_Per_Batch))
        {
            uint32_t previous_quad_id = quad_id;

            quad_id = GenQuadId();
            if (quadId)
                *quadId = quad_id;

            s_QuadDictionary[quad_id][0] = s_QuadDictionary[previous_quad_id][0];
            s_QuadDictionary[quad_id][1] = s_QuadDictionary[previous_quad_id][1] + 4;

            for (auto& vertex : vertices)
                s_Batches[s_QuadDictionary[quad_id][0]].Vertices.push_back(vertex);

            LoadTexture(&quad_id, textureFilePath);
        }
    }

    void Renderer::AddQuadToTextBatch(
        uint32_t* quadId,
        const std::array<FlameUI::Vertex, 4>& vertices,
        uint32_t textureId
    )
    {
        static uint32_t quad_id = 0;
        if ((!quad_id) || (s_Batches[s_QuadDictionary[quad_id][0]].Vertices.size() == s_Max_Vertices_Per_Batch))
        {
            s_Batches.emplace_back(BatchType::Text);
            s_Batches.back().Init();

            quad_id = GenQuadId();
            if (quadId)
                *quadId = quad_id;
            s_QuadDictionary[quad_id][0] = s_Batches.size() - 1;
            s_QuadDictionary[quad_id][1] = 0;

            for (auto& vertex : vertices)
                s_Batches.back().Vertices.push_back(vertex);

            s_Batches.back().RendererIds.textureIds.push_back(textureId);
        }
        else if ((quad_id) && (s_Batches[s_QuadDictionary[quad_id][0]].Vertices.size() < s_Max_Vertices_Per_Batch))
        {
            uint32_t previous_quad_id = quad_id;
            quad_id = GenQuadId();
            if (quadId)
                *quadId = quad_id;

            s_QuadDictionary[quad_id][0] = s_QuadDictionary[previous_quad_id][0];
            s_QuadDictionary[quad_id][1] = s_QuadDictionary[previous_quad_id][1] + 4;

            for (auto& vertex : vertices)
                s_Batches[s_QuadDictionary[previous_quad_id][0]].Vertices.push_back(vertex);

            s_Batches[s_QuadDictionary[previous_quad_id][0]].RendererIds.textureIds.push_back(textureId);
        }
    }

    void Renderer::AddText(
        const std::string& text,
        const glm::ivec2& position_in_pixels,
        float scale,
        const glm::vec4& color
    )
    {
        static uint16_t slot = 0;
        auto position = position_in_pixels;

        for (std::string::const_iterator it = text.begin(); it != text.end(); it++)
        {
            Character character = s_Characters[*it];

            float xpos = position.x + character.Bearing.x * scale;
            float ypos = position.y - (character.Size.y - character.Bearing.y) * scale - s_FontProps.DescenderY * scale;

            float w = character.Size.x * scale;
            float h = character.Size.y * scale;

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
            if (slot == s_Max_Texture_Slots)
                slot = 0;

            for (uint8_t i = 0; i < 4; i++)
                vertices[i].color = color;

            AddQuadToTextBatch(nullptr, vertices, character.TextureId);
            position.x += (character.Advance) * scale + 1.0f;
        }
    }

    void Renderer::OnDraw()
    {
        /* Set Projection Matrix in GPU memory, for all shader programs to access it */
        GL_CHECK_ERROR(glBindBuffer(GL_UNIFORM_BUFFER, s_UniformBufferId));
        GL_CHECK_ERROR(glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(s_UniformBufferData.ProjectionMatrix)));

        for (auto& batch : s_Batches)
            batch.OnDraw();

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
        FL_LOG("Font Metrics are:\nAscendry: {0}, Descendry: {1}, Line Height: {2}, emSize: {3}", metrics.ascenderY, metrics.descenderY, metrics.lineHeight, metrics.emSize);
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

            Character ch = {
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
            textDimensions.x += s_Characters[*it].Size.x * scale;
            if (lowestPositionOfChar > -(s_Characters[*it].Size.y - s_Characters[*it].Bearing.y) * scale)
                lowestPositionOfChar = -(s_Characters[*it].Size.y - s_Characters[*it].Bearing.y) * scale;
            if (highestPositionOfChar < -(s_Characters[*it].Size.y - s_Characters[*it].Bearing.y) * scale + s_Characters[*it].Size.y * scale)
                highestPositionOfChar = -(s_Characters[*it].Size.y - s_Characters[*it].Bearing.y) * scale + s_Characters[*it].Size.y * scale;
        }
        textDimensions.y = highestPositionOfChar - lowestPositionOfChar;

        return textDimensions;
    }

    glm::vec2 Renderer::ConvertPixelsToOpenGLValues(const glm::ivec2& value_in_pixels)
    {
        glm::vec2 position_in_opengl_coords;
        position_in_opengl_coords.x = value_in_pixels.x * (float)((2.0f * s_AspectRatio) / 1280.0f);
        position_in_opengl_coords.y = value_in_pixels.y * (float)(2.0f / 720.0f);
        return position_in_opengl_coords;
    }

    void Renderer::ChangeQuadVertices(uint32_t& quadId, const std::array<Vertex, 4>& vertices)
    {
        for (uint8_t i = 0; i < 4; i++)
            s_Batches[s_QuadDictionary[quadId][0]].Vertices[s_QuadDictionary[quadId][1] + i] = vertices[i];
    }

    void Renderer::ChangeQuadVertices(
        uint32_t& quadId,
        const QuadPosType& positionType,
        const glm::ivec2& position_in_pixels,
        const glm::ivec2& dimensions_in_pixels,
        const glm::vec4& color
    )
    {
        std::array<Vertex, 4> vertices;
        GetQuadVertices(&vertices, positionType, position_in_pixels, dimensions_in_pixels, color);
        for (uint8_t i = 0; i < 4; i++)
            s_Batches[s_QuadDictionary[quadId][0]].Vertices[s_QuadDictionary[quadId][1] + i] = vertices[i];
    }

    float Renderer::ConvertXAxisPixelValueToOpenGLValue(int X)
    {
        return (float)X * (float)((2.0f * s_AspectRatio) / 1280.0f);
    }

    float Renderer::ConvertYAxisPixelValueToOpenGLValue(int Y)
    {
        return (float)Y * (float)(2.0f / 720.0f);
    }

    void Renderer::LoadTexture(uint32_t* quadId, const std::string& filePath)
    {
        static uint16_t slot = 0;
        if (filePath != "")
        {
            FL_ASSERT(s_Batches[s_QuadDictionary[*quadId][0]].RendererIds.textureIds.size() < s_Max_Texture_Slots, "Texture Slots are full!");
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
            GL_CHECK_ERROR(glActiveTexture(GL_TEXTURE0 + slot));
            GL_CHECK_ERROR(glBindTexture(GL_TEXTURE_2D, textureId));

            GL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
            GL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
            GL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
            GL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

            GL_CHECK_ERROR(glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data));
            GL_CHECK_ERROR(glGenerateMipmap(GL_TEXTURE_2D));

            stbi_image_free(data);

            s_Batches[s_QuadDictionary[*quadId][0]].RendererIds.textureIds.push_back(textureId);

            uint16_t index = s_QuadDictionary[*quadId][1];
            for (uint8_t j = 0; j < 4; j++)
                s_Batches[s_QuadDictionary[*quadId][0]].Vertices[index + j].texture_index = (float)slot;
        }
        else
        {
            s_Batches[s_QuadDictionary[*quadId][0]].RendererIds.textureIds.emplace_back(0);
        }
        slot++;
        if (slot == 16)
            slot = 0;
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
        if (m_uniformloc_cache.find(name) != m_uniformloc_cache.end())
            return m_uniformloc_cache[name];

        GLint location = glGetUniformLocation(shaderId, name.c_str());
        if (location == -1)
            FL_LOG("Uniform \"{0}\" not found!", name);
        m_uniformloc_cache[name] = location;
        return location;
    }

    void Renderer::CleanUp()
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glUseProgram(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
        for (auto batch : s_Batches)
        {
            glDeleteBuffers(1, &batch.RendererIds.v_bufferId);
            glDeleteBuffers(1, &batch.RendererIds.i_bufferId);
            glDeleteVertexArrays(1, &batch.RendererIds.v_arrayId);
            glDeleteProgram(batch.RendererIds.shaderId);
            for (auto& textureId : batch.RendererIds.textureIds)
                glDeleteTextures(1, &textureId);
        }
    }
}
