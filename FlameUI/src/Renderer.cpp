#include "Renderer.h"
#include <iostream>
#include "opengl_math_types.h"
#include "opengl_helper_funcs.h"
#include "opengl_debug.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

#include "msdfgen/msdfgen.h"
#include "msdfgen/msdfgen-ext.h"

namespace FlameUI {
    std::vector<Renderer::Batch> Renderer::s_Batches;

    /**
     * `s_QuadDictionary`
     * In the `s_QuadDictionary` unordered_map, first element is the quad id, which is unique for every quad.
     * In the `s_QuadDictionary` unordered_map, second element is an array of two unsigned ints,
     * whose first value is the index of the batch where the quad is in, in the `s_Batches` vector,
     * whereas the second element is the index of the first vertex of the quad in the Vertices buffer, stored in the `Batch` struct.
     * The two indexes are enough to uniquely identify a quad in this Batch Renderer.
     */
    std::unordered_map<uint32_t, uint32_t[2]> Renderer::s_QuadDictionary;
    std::unordered_map<std::string, GLint> Renderer::m_uniformloc_cache;

    float Renderer::s_AspectRatio = (float)(1280.0f / 720.0f);
    glm::mat4 Renderer::s_Proj_Matrix = glm::ortho(-s_AspectRatio, s_AspectRatio, -1.0f, 1.0f, -1.0f, 1.0f);

    Renderer::FontProps Renderer::s_FontProps = { 1.0f, 0.5f, 8.0f };
    std::unordered_map<char, Renderer::Character> Renderer::s_Characters;
    std::string Renderer::s_UserFontFilePath = "";
    std::string Renderer::s_DefaultFontFilePath = FL_PROJECT_DIR + std::string("FlameUI/resources/fonts/OpenSans-Regular.ttf");

    void Renderer::OnResize(uint32_t window_width, uint32_t window_height)
    {
        s_AspectRatio = ((float)window_width / (float)window_height);
        s_Proj_Matrix = glm::ortho(-s_AspectRatio, s_AspectRatio, -1.0f, 1.0f, -1.0f, 1.0f);

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

    void Renderer::GetQuadVertices(
        std::array<Vertex, 4>* vertices,
        const QuadPosType& positionType,
        const fuiVec2<int>& position_in_pixels,
        const fuiVec2<uint32_t>& dimensions_in_pixels,
        const fuiVec4<float>& color
    )
    {
        fuiVec2<float> position = ConvertPixelsToOpenGLValues(position_in_pixels);
        fuiVec2<float> dimensions = ConvertPixelsToOpenGLValues({ (int)dimensions_in_pixels.X, (int)dimensions_in_pixels.Y });

        Vertex Vertices[4];

        switch (positionType)
        {
        case FL_QUAD_POS_BOTTOM_LEFT_VERTEX:
            (*vertices)[0].position = { position.X,                position.Y,                0.0f };
            (*vertices)[1].position = { position.X,                position.Y + dimensions.Y, 0.0f };
            (*vertices)[2].position = { position.X + dimensions.X, position.Y + dimensions.Y, 0.0f };
            (*vertices)[3].position = { position.X + dimensions.X, position.Y,                0.0f };
            break;
        case FL_QUAD_POS_CENTER:
            (*vertices)[0].position = { -(dimensions.X / 2.0f) + position.X, -(dimensions.Y / 2.0f) + position.Y, 0.0f };
            (*vertices)[1].position = { -(dimensions.X / 2.0f) + position.X, +(dimensions.Y / 2.0f) + position.Y, 0.0f };
            (*vertices)[2].position = { +(dimensions.X / 2.0f) + position.X, +(dimensions.Y / 2.0f) + position.Y, 0.0f };
            (*vertices)[3].position = { +(dimensions.X / 2.0f) + position.X, -(dimensions.Y / 2.0f) + position.Y, 0.0f };
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
        const fuiVec2<int>& position_in_pixels,
        const fuiVec2<uint32_t>& dimensions_in_pixels,
        const fuiVec4<float>& color,
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
        const fuiVec2<int>& position_in_pixels,
        float scale,
        const fuiVec4<float>& color
    )
    {
        static uint16_t slot = 0;
        auto position = position_in_pixels;
        for (std::string::const_iterator it = text.begin(); it != text.end(); it++)
        {
            Character character = s_Characters[*it];

            float xpos = position.X + character.Bearing.x * scale;
            float ypos = position.Y - (character.Size.y - character.Bearing.y) * scale;

            float w = character.Size.x * scale;
            float h = character.Size.y * scale;

            std::array<FlameUI::Vertex, 4> vertices;
            vertices[0].position = { xpos,     ypos,     0.0f };
            vertices[1].position = { xpos,     ypos + h, 0.0f };
            vertices[2].position = { xpos + w, ypos + h, 0.0f };
            vertices[3].position = { xpos + w, ypos,     0.0f };

            for (auto& vertex : vertices)
            {
                vertex.position.X = ConvertXAxisPixelValueToOpenGLValue(vertex.position.X);
                vertex.position.Y = ConvertYAxisPixelValueToOpenGLValue(vertex.position.Y);
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
            position.X += (character.Advance) * scale + 1.0f;
        }
    }

    void Renderer::Batch::OnDraw()
    {
        if (Vertices.size() != 0)
        {
            GL_CHECK_ERROR(glBindBuffer(GL_ARRAY_BUFFER, RendererIds.v_bufferId));
            GL_CHECK_ERROR(glBufferSubData(GL_ARRAY_BUFFER, 0, Vertices.size() * sizeof(Vertex), Vertices.data()));

            for (uint8_t i = 0; i < RendererIds.textureIds.size(); i++)
            {
                GL_CHECK_ERROR(glActiveTexture(GL_TEXTURE0 + i));
                GL_CHECK_ERROR(glBindTexture(GL_TEXTURE_2D, RendererIds.textureIds[i]));
            }

            GL_CHECK_ERROR(glUseProgram(RendererIds.shaderId));
            GL_CHECK_ERROR(glUniformMatrix4fv(GetUniformLocation("u_ViewProjectionMatrix", RendererIds.shaderId), 1, GL_FALSE, glm::value_ptr(s_Proj_Matrix)));

            GL_CHECK_ERROR(glBindVertexArray(RendererIds.v_arrayId));
            GL_CHECK_ERROR(glDrawElements(GL_TRIANGLES, s_Max_Indices_Per_Batch, GL_UNSIGNED_INT, 0));
        }
    }

    void Renderer::OnDraw()
    {
        for (auto& batch : s_Batches)
            batch.OnDraw();
    }

    void Renderer::LoadFont(const std::string& filePath)
    {
        msdfgen::FreetypeHandle* ft = msdfgen::initializeFreetype();
        FL_ASSERT(ft, "Failed to initialize Font Handle!");

        msdfgen::FontHandle* msdffontHandle = loadFont(ft, filePath.c_str());
        FL_ASSERT(msdffontHandle, "Failed to load font from the file path: {0}", filePath);

        double border_width = 4.0;

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

    fuiVec2<float> Renderer::ConvertPixelsToOpenGLValues(const fuiVec2<int>& value_in_pixels)
    {
        fuiVec2<float> position_in_opengl_coords;
        position_in_opengl_coords.X = value_in_pixels.X * (float)((2.0f * s_AspectRatio) / 1280.0f);
        position_in_opengl_coords.Y = value_in_pixels.Y * (float)(2.0f / 720.0f);
        return position_in_opengl_coords;
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
