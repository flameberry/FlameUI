#include "Renderer.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Timer.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

#include "msdfgen/msdfgen.h"
#include "msdfgen/msdfgen-ext.h"

namespace FlameUI {
    std::unordered_map<uint32_t, uint32_t[2]>     Renderer::s_QuadDictionary;
    std::unordered_map<std::string, GLint>        Renderer::m_UniformLocationCache;
    std::unordered_map<char, Renderer::Character> Renderer::s_Characters;
    std::vector<std::shared_ptr<Batch>>           Renderer::s_Batches;
    uint32_t                                      Renderer::s_UniformBufferId;
    float                                         Renderer::s_AspectRatio = (float)(1280.0f / 720.0f);
    std::string                                   Renderer::s_UserFontFilePath = "";

    /// FL_PROJECT_DIR is just a macro defined by cmake, which is the absolute file path of the source dir of the FlameUI project
    std::string                                   Renderer::s_DefaultFontFilePath = FL_PROJECT_DIR + std::string("FlameUI/resources/fonts/OpenSans-Regular.ttf");
    Renderer::UniformBufferData                   Renderer::s_UniformBufferData;
    Renderer::FontProps                           Renderer::s_FontProps = { .Scale = 1.0f, .Strength = 0.5f, .PixelRange = 8.0f };
    glm::vec2                                     Renderer::s_ViewportSize = { 1280.0f, 720.0f };
    glm::vec2                                     Renderer::s_CursorPosition = { 0.0f, 0.0f };
    GLFWwindow* Renderer::s_UserWindow;

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
    /// Renderer Function Implementations --------------------------------------------------------------------------------
    ///

    void Renderer::OnResize()
    {
        int width, height;
        glfwGetFramebufferSize(s_UserWindow, &width, &height);
        s_ViewportSize = { width, height };
        s_AspectRatio = s_ViewportSize.x / s_ViewportSize.y;
        s_UniformBufferData.ProjectionMatrix = glm::ortho(-s_AspectRatio, s_AspectRatio, -1.0f, 1.0f, -1.0f, 1.0f);

        glViewport(0, 0, s_ViewportSize.x, s_ViewportSize.y);
    }

    uint32_t Renderer::GenQuadId()
    {
        static uint32_t offset = 0;
        uint32_t id = 15 + offset;
        offset += 2;
        return id;
    }

    glm::vec2   Renderer::GetViewportSize() { return s_ViewportSize; }
    GLFWwindow* Renderer::GetUserGLFWwindow() { return s_UserWindow; }
    glm::vec2& Renderer::GetCursorPosition() { return s_CursorPosition; }
    glm::vec2 Renderer::GetQuadPositionInPixels(uint32_t* quadId)
    {
        return ConvertOpenGLValuesToPixels(s_Batches[s_QuadDictionary[*quadId][0]]->GetQuadPosition(s_QuadDictionary[*quadId][1]));
    }
    glm::vec2  Renderer::GetQuadDimensionsInPixels(uint32_t* quadId)
    {
        return ConvertOpenGLValuesToPixels(s_Batches[s_QuadDictionary[*quadId][0]]->GetQuadDimensions(s_QuadDictionary[*quadId][1]));
    }

    void Renderer::OnUpdate()
    {
        glClear(GL_DEPTH_BUFFER_BIT);
        double x, y;
        glfwGetCursorPos(s_UserWindow, &x, &y);
        s_CursorPosition.x = x - s_ViewportSize.x / 2.0f;
        s_CursorPosition.y = -y + s_ViewportSize.y / 2.0f;
        OnResize();
    }

    void Renderer::Init(GLFWwindow* window)
    {
        FL_LOGGER_INIT();
        FL_INFO("Initialized Logger!");

        s_UserWindow = window;

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_DEPTH_TEST);

        int width, height;
        glfwGetFramebufferSize(s_UserWindow, &width, &height);
        s_ViewportSize = { (float)width, (float)height };

        if (s_UserFontFilePath == "")
            s_UserFontFilePath = s_DefaultFontFilePath;
        LoadFont(s_UserFontFilePath);
        FL_INFO("Loaded Font from path \"{0}\"", s_UserFontFilePath);

        /* Create Uniform Buffer */
        glGenBuffers(1, &s_UniformBufferId);
        glBindBuffer(GL_UNIFORM_BUFFER, s_UniformBufferId);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(UniformBufferData), nullptr, GL_DYNAMIC_DRAW);
        glBindBufferRange(GL_UNIFORM_BUFFER, 0, s_UniformBufferId, 0, sizeof(UniformBufferData));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        FL_INFO("Initialized Renderer!");
    }

    void Renderer::GetQuadVertices(
        std::array<Vertex, 4>* vertices,
        const QuadPosType& positionType,
        const glm::vec2& position_in_pixels,
        const glm::vec2& dimensions_in_pixels,
        const glm::vec4& color
    )
    {
        glm::vec2 position = ConvertPixelsToOpenGLValues(position_in_pixels);
        glm::vec2 dimensions = ConvertPixelsToOpenGLValues({ dimensions_in_pixels.x, dimensions_in_pixels.y });

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

    void Renderer::AddQuad(uint32_t* quadId, const QuadPosType& positionType, const glm::vec2& position_in_pixels,
        const glm::vec2& dimensions_in_pixels, const glm::vec4& color)
    {
        std::array<Vertex, 4> vertices;
        GetQuadVertices(&vertices, positionType, position_in_pixels, dimensions_in_pixels, color);

        static bool first_time = true;
        static uint32_t batch_index = 0;
        if (first_time)
        {
            s_Batches.push_back(std::make_shared<BasicQuadBatch>());
            s_Batches.back()->Init();
            batch_index = s_Batches.size() - 1;
            first_time = false;
        }

        uint32_t quad_id = GenQuadId();
        if (quadId)
            *quadId = quad_id;

        s_QuadDictionary[quad_id][0] = batch_index;
        s_Batches[batch_index]->AddQuad(vertices, &s_QuadDictionary[quad_id][1]);
    }

    void Renderer::AddQuad(uint32_t* quadId, const QuadPosType& positionType, const glm::vec2& position_in_pixels,
        const glm::vec2& dimensions_in_pixels, const glm::vec4& color, const std::string& textureFilePath)
    {
        std::array<Vertex, 4> vertices;
        GetQuadVertices(&vertices, positionType, position_in_pixels, dimensions_in_pixels, color);

        static uint16_t slot = 0;
        for (uint8_t i = 0; i < 4; i++)
            vertices[i].texture_index = slot;
        slot++;
        if (slot == s_Max_Texture_Slots)
            slot = 0;

        static uint32_t quad_id = 0;
        if ((!quad_id) || (s_Batches[s_QuadDictionary[quad_id][0]]->IsFull()))
        {
            s_Batches.push_back(std::make_shared<TexturedQuadBatch>());
            s_Batches.back()->Init();

            quad_id = GenQuadId();
            if (quadId)
                *quadId = quad_id;
            s_QuadDictionary[quad_id][0] = s_Batches.size() - 1;
            s_QuadDictionary[quad_id][1] = 0;

            s_Batches.back()->AddQuad(vertices, nullptr);
            LoadTexture(&quad_id, textureFilePath);
        }
        else if ((quad_id) && (!s_Batches[s_QuadDictionary[quad_id][0]]->IsFull()))
        {
            uint32_t previous_quad_id = quad_id;

            quad_id = GenQuadId();
            if (quadId)
                *quadId = quad_id;

            s_QuadDictionary[quad_id][0] = s_QuadDictionary[previous_quad_id][0];
            s_Batches[s_QuadDictionary[quad_id][0]]->AddQuad(vertices, &s_QuadDictionary[quad_id][1]);

            LoadTexture(&quad_id, textureFilePath);
        }
    }

    bool Renderer::DoesQuadExist(uint32_t* quadId)
    {
        if (s_QuadDictionary.find(*quadId) == s_QuadDictionary.end())
            return false;
        return s_Batches[s_QuadDictionary[*quadId][0]]->DoQuadVerticesExist(s_QuadDictionary[*quadId][1]);
    }

    // Broken Function
    // TODO: Fix this function
    void Renderer::RemoveQuad(uint32_t* quadId)
    {
        if (!DoesQuadExist(quadId))
        {
            FL_WARN("Attempted to remove quad that doesn't exist!");
            return;
        }

        flame::optional<uint32_t> prev_loc;
        switch (s_Batches[s_QuadDictionary[*quadId][0]]->GetBatchType())
        {
        case BatchType::BasicQuad:
            s_Batches[s_QuadDictionary[*quadId][0]]->RemoveQuadVertices(s_QuadDictionary[*quadId][1], prev_loc);

            if (prev_loc.has_value())
            {
                for (std::unordered_map<uint32_t, uint32_t[2]>::iterator it = s_QuadDictionary.begin(); it != s_QuadDictionary.end(); it++)
                {
                    if ((*it).second[1] == prev_loc)
                    {
                        (*it).second[1] = s_QuadDictionary[*quadId][1];
                        break;
                    }
                }
            }
            s_QuadDictionary.erase(*quadId);
            break;
        case BatchType::TexturedQuad:
            break;
        case BatchType::Text:
            break;
        }
    }

    void Renderer::AddQuadToTextBatch(
        uint32_t* quadId,
        const std::array<FlameUI::Vertex, 4>& vertices,
        uint32_t textureId
    )
    {
        static uint32_t quad_id = 0;
        if ((!quad_id) || (s_Batches[s_QuadDictionary[quad_id][0]]->IsFull()))
        {
            s_Batches.push_back(std::make_shared<TextBatch>());
            s_Batches.back()->Init();

            quad_id = GenQuadId();
            if (quadId)
                *quadId = quad_id;
            s_QuadDictionary[quad_id][0] = s_Batches.size() - 1;
            s_QuadDictionary[quad_id][1] = 0;

            s_Batches.back()->AddQuad(vertices, nullptr);
            s_Batches.back()->AddTextureId(textureId);
        }
        else if ((quad_id) && (!s_Batches[s_QuadDictionary[quad_id][0]]->IsFull()))
        {
            uint32_t previous_quad_id = quad_id;
            quad_id = GenQuadId();
            if (quadId)
                *quadId = quad_id;

            s_QuadDictionary[quad_id][0] = s_QuadDictionary[previous_quad_id][0];

            s_Batches[s_QuadDictionary[quad_id][0]]->AddQuad(vertices, &s_QuadDictionary[quad_id][1]);
            s_Batches[s_QuadDictionary[quad_id][0]]->AddTextureId(textureId);
        }
    }

    void Renderer::AddText(const std::string& text, const glm::vec2& position_in_pixels, float scale, const glm::vec4& color)
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
        FL_TIMER_SCOPE("Renderer_OnDraw()");
        /* Set Projection Matrix in GPU memory, for all shader programs to access it */
        GL_CHECK_ERROR(glBindBuffer(GL_UNIFORM_BUFFER, s_UniformBufferId));
        GL_CHECK_ERROR(glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(s_UniformBufferData.ProjectionMatrix)));

        for (auto& batch : s_Batches)
            batch->OnDraw();

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

    glm::vec2 Renderer::ConvertPixelsToOpenGLValues(const glm::vec2& value_in_pixels)
    {
        glm::vec2 position_in_opengl_coords;
        position_in_opengl_coords.x = value_in_pixels.x * (float)((2.0f * s_AspectRatio) / s_ViewportSize.x);
        position_in_opengl_coords.y = value_in_pixels.y * (float)(2.0f / s_ViewportSize.y);
        return position_in_opengl_coords;
    }

    void Renderer::SetQuadZIndex(uint32_t* quadId, float z)
    {
        s_Batches[s_QuadDictionary[*quadId][0]]->SetQuadZIndex(s_QuadDictionary[*quadId][1], z);
    }

    void Renderer::SetQuadPosition(uint32_t* quadId, const glm::vec2& position_in_pixels)
    {
        auto position = ConvertPixelsToOpenGLValues(position_in_pixels);
        auto ptr = GetPtrToQuadVertices(quadId);
        float size_x = ptr[3]->position.x - ptr[0]->position.x;
        float size_y = ptr[1]->position.y - ptr[0]->position.y;
        ptr[0]->position.x = position.x - (size_x / 2.0f);
        ptr[1]->position.x = position.x - (size_x / 2.0f);
        ptr[2]->position.x = position.x + (size_x / 2.0f);
        ptr[3]->position.x = position.x + (size_x / 2.0f);

        ptr[0]->position.y = position.y - (size_y / 2.0f);
        ptr[1]->position.y = position.y + (size_y / 2.0f);
        ptr[2]->position.y = position.y + (size_y / 2.0f);
        ptr[3]->position.y = position.y - (size_y / 2.0f);
    }

    void Renderer::SetQuadDimensions(uint32_t* quadId, const glm::vec2& dimensions_in_pixels)
    {
        auto dimensions = ConvertPixelsToOpenGLValues(dimensions_in_pixels);
        auto ptr = GetPtrToQuadVertices(quadId);
        float position_x = ptr[3]->position.x - (ptr[3]->position.x - ptr[0]->position.x) / 2.0f;
        float position_y = ptr[1]->position.y - (ptr[1]->position.y - ptr[0]->position.y) / 2.0f;

        ptr[0]->position.x = position_x - (dimensions.x / 2.0f);
        ptr[1]->position.x = position_x - (dimensions.x / 2.0f);
        ptr[2]->position.x = position_x + (dimensions.x / 2.0f);
        ptr[3]->position.x = position_x + (dimensions.x / 2.0f);

        ptr[0]->position.y = position_y - (dimensions.y / 2.0f);
        ptr[1]->position.y = position_y + (dimensions.y / 2.0f);
        ptr[2]->position.y = position_y + (dimensions.y / 2.0f);
        ptr[3]->position.y = position_y - (dimensions.y / 2.0f);
    }

    void Renderer::SetQuadColor(uint32_t* quadId, const glm::vec4& color)
    {
        auto ptr = GetPtrToQuadVertices(quadId);
        for (uint16_t i = 0; i < 4; i++)
            ptr[i]->color = color;
    }

    void Renderer::ChangeQuadVertices(uint32_t* quadId, const std::array<Vertex, 4>& vertices)
    {
        s_Batches[s_QuadDictionary[*quadId][0]]->SetQuadVertices(s_QuadDictionary[*quadId][1], vertices);
    }

    void Renderer::ChangeQuadVertices(uint32_t* quadId, const QuadPosType& positionType, const glm::vec2& position_in_pixels,
        const glm::vec2& dimensions_in_pixels, const glm::vec4& color)
    {
        std::array<Vertex, 4> vertices;
        GetQuadVertices(&vertices, positionType, position_in_pixels, dimensions_in_pixels, color);
        s_Batches[s_QuadDictionary[*quadId][0]]->SetQuadVertices(s_QuadDictionary[*quadId][1], vertices);
    }

    std::array<Vertex*, 4>Renderer::GetPtrToQuadVertices(uint32_t* quadId)
    {
        return s_Batches[s_QuadDictionary[*quadId][0]]->GetPtrToQuadVertices(s_QuadDictionary[*quadId][1]);
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
        return (float)X * (float)((2.0f * s_AspectRatio) / s_ViewportSize.x);
    }

    float Renderer::ConvertYAxisPixelValueToOpenGLValue(int Y)
    {
        return (float)Y * (float)(2.0f / s_ViewportSize.y);
    }

    void Renderer::LoadTexture(uint32_t* quadId, const std::string& filePath)
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

        s_Batches[s_QuadDictionary[*quadId][0]]->AddTextureId(textureId);
    }

    void Renderer::PrintQuadDictionary()
    {
        if (s_QuadDictionary.empty())
        {
            FL_WARN("Quad Dictionary is empty!");
            return;
        }

        uint32_t i = 1;
        for (auto& it : s_QuadDictionary)
        {
            FL_LOG("Quad {0}: QuadId: {1}, Batch Index: {2}, Vertex Index: {3}", i, it.first, it.second[0], it.second[1]);
            i++;
        }
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
            FL_LOG("Uniform \"{0}\" not found!", name);
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
