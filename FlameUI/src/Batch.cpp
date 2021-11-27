#include "Batch.h"
#include "Core.h"
#include <fstream>
#include <sstream>
#include <string>
#include <tuple>
#include "Renderer.h"

namespace FlameUI {
    ///
    /// BasicQuadBatch Implementation ------------------------------------------------------------------------------
    ///

    bool BasicQuadBatch::IsFull()
    {
        if (m_Vertices.size() == s_Max_Vertices)
            return true;
        return false;
    }

    BasicQuadBatch::~BasicQuadBatch()
    {
        glDeleteBuffers(1, &m_VertexBufferId);
        glDeleteBuffers(1, &m_IndexBufferId);
        glDeleteVertexArrays(1, &m_VertexArrayId);
        glDeleteProgram(m_ShaderProgramId);
    }

    void BasicQuadBatch::AddQuad(const std::array<Vertex, 4>& vertices, uint32_t* location)
    {
        if (location)
            *location = m_Vertices.size();
        for (auto& vertex : vertices)
            m_Vertices.push_back(vertex);
    }

    void BasicQuadBatch::AddTextureId(uint32_t textureId)
    {
    }

    std::array<Vertex*, 4> BasicQuadBatch::GetPtrToQuadVertices(uint32_t location)
    {
        std::array<Vertex*, 4> ptr;
        for (uint8_t i = 0; i < 4; i++)
            ptr[i] = &m_Vertices[location + i];
        return ptr;
    }

    void BasicQuadBatch::SetQuadVertices(uint32_t location, const std::array<Vertex, 4>& vertices)
    {
        for (uint8_t i = 0; i < 4; i++)
            m_Vertices[location + i] = vertices[i];
    }

    void BasicQuadBatch::SetQuadZIndex(uint32_t location, float z)
    {
        for (uint8_t i = 0; i < 4; i++)
            m_Vertices[location + i].position.z = z;
    }

    void BasicQuadBatch::Init()
    {
        uint32_t indices[s_Max_Indices];
        size_t offset = 0;
        for (size_t i = 0; i < s_Max_Indices; i += 6)
        {
            indices[0 + i] = 1 + offset;
            indices[1 + i] = 2 + offset;
            indices[2 + i] = 3 + offset;

            indices[3 + i] = 3 + offset;
            indices[4 + i] = 0 + offset;
            indices[5 + i] = 1 + offset;

            offset += 4;
        }

        GL_CHECK_ERROR(glGenVertexArrays(1, &m_VertexArrayId));
        GL_CHECK_ERROR(glBindVertexArray(m_VertexArrayId));

        GL_CHECK_ERROR(glGenBuffers(1, &m_VertexBufferId));
        GL_CHECK_ERROR(glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferId));
        GL_CHECK_ERROR(glBufferData(GL_ARRAY_BUFFER, s_Max_Vertices * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW));

        GL_CHECK_ERROR(glBindVertexArray(m_VertexArrayId));

        GL_CHECK_ERROR(glEnableVertexAttribArray(0));
        GL_CHECK_ERROR(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)offsetof(Vertex, position)));
        GL_CHECK_ERROR(glEnableVertexAttribArray(1));
        GL_CHECK_ERROR(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)offsetof(Vertex, color)));
        GL_CHECK_ERROR(glEnableVertexAttribArray(2));
        GL_CHECK_ERROR(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)offsetof(Vertex, texture_uv)));
        GL_CHECK_ERROR(glEnableVertexAttribArray(3));
        GL_CHECK_ERROR(glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)offsetof(Vertex, texture_index)));

        GL_CHECK_ERROR(glGenBuffers(1, &m_IndexBufferId));
        GL_CHECK_ERROR(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBufferId));
        GL_CHECK_ERROR(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));

        GL_CHECK_ERROR(glBindVertexArray(m_VertexArrayId));

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
        m_ShaderProgramId = glCreateProgram();

        // Attach our shaders to our program
        glAttachShader(m_ShaderProgramId, vertex_shader);
        glAttachShader(m_ShaderProgramId, fragment_shader);

        // Link our program
        glLinkProgram(m_ShaderProgramId);

        // Note the different functions here: glGetProgram* instead of glGetShader*.
        GLint isLinked = 0;
        glGetProgramiv(m_ShaderProgramId, GL_LINK_STATUS, (int*)&isLinked);
        if (isLinked == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetProgramiv(m_ShaderProgramId, GL_INFO_LOG_LENGTH, &maxLength);

            // The maxLength includes the NULL character
            std::vector<GLchar> infoLog(maxLength);
            glGetProgramInfoLog(m_ShaderProgramId, maxLength, &maxLength, &infoLog[0]);

            // We don't need the program anymore.
            glDeleteProgram(m_ShaderProgramId);
            // Don't leak shaders either.
            glDeleteShader(vertex_shader);
            glDeleteShader(fragment_shader);

            FL_ERROR("Error linking shader program:\n{0}", infoLog.data());
        }

        // Always detach shaders after a successful link.
        glDetachShader(m_ShaderProgramId, vertex_shader);
        glDetachShader(m_ShaderProgramId, fragment_shader);
    }

    void BasicQuadBatch::OnDraw()
    {
        if (m_Vertices.size())
        {
            GL_CHECK_ERROR(glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferId));
            GL_CHECK_ERROR(glBufferSubData(GL_ARRAY_BUFFER, 0, m_Vertices.size() * sizeof(Vertex), m_Vertices.data()));

            GL_CHECK_ERROR(glUseProgram(m_ShaderProgramId));
            GL_CHECK_ERROR(glBindVertexArray(m_VertexArrayId));
            GL_CHECK_ERROR(glDrawElements(GL_TRIANGLES, s_Max_Indices, GL_UNSIGNED_INT, 0));
        }
    }

    ///
    /// TexturedQuadBatch Implementation ------------------------------------------------------------------------------
    ///

    bool TexturedQuadBatch::IsFull()
    {
        if (m_Vertices.size() == s_Max_Vertices)
            return true;
        return false;
    }

    void TexturedQuadBatch::AddQuad(const std::array<Vertex, 4>& vertices, uint32_t* location)
    {
        *location = m_Vertices.size();
        for (auto& vertex : vertices)
            m_Vertices.push_back(vertex);
    }

    void TexturedQuadBatch::AddTextureId(uint32_t textureId)
    {
        m_TextureIds.emplace_back(textureId);
    }

    std::array<Vertex*, 4> TexturedQuadBatch::GetPtrToQuadVertices(uint32_t location)
    {
        std::array<Vertex*, 4> ptr;
        for (uint8_t i = 0; i < 4; i++)
            ptr[i] = &m_Vertices[location + i];
        return ptr;
    }

    void TexturedQuadBatch::SetQuadVertices(uint32_t location, const std::array<Vertex, 4>& vertices)
    {
        for (uint8_t i = 0; i < 4; i++)
            m_Vertices[location + i] = vertices[i];
    }

    void TexturedQuadBatch::SetQuadZIndex(uint32_t location, float z)
    {
        for (uint8_t i = 0; i < 4; i++)
            m_Vertices[location + i].position.z = z;
    }

    TexturedQuadBatch::~TexturedQuadBatch()
    {
        glDeleteBuffers(1, &m_VertexBufferId);
        glDeleteBuffers(1, &m_IndexBufferId);
        glDeleteVertexArrays(1, &m_VertexArrayId);
        glDeleteProgram(m_ShaderProgramId);
        for (auto& textureId : m_TextureIds)
            glDeleteTextures(1, &textureId);
    }

    void TexturedQuadBatch::Init()
    {
        uint32_t indices[s_Max_Indices];
        size_t offset = 0;
        for (size_t i = 0; i < s_Max_Indices; i += 6)
        {
            indices[0 + i] = 1 + offset;
            indices[1 + i] = 2 + offset;
            indices[2 + i] = 3 + offset;

            indices[3 + i] = 3 + offset;
            indices[4 + i] = 0 + offset;
            indices[5 + i] = 1 + offset;

            offset += 4;
        }

        GL_CHECK_ERROR(glGenVertexArrays(1, &m_VertexArrayId));
        GL_CHECK_ERROR(glBindVertexArray(m_VertexArrayId));

        GL_CHECK_ERROR(glGenBuffers(1, &m_VertexBufferId));
        GL_CHECK_ERROR(glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferId));
        GL_CHECK_ERROR(glBufferData(GL_ARRAY_BUFFER, s_Max_Vertices * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW));

        GL_CHECK_ERROR(glBindVertexArray(m_VertexArrayId));

        GL_CHECK_ERROR(glEnableVertexAttribArray(0));
        GL_CHECK_ERROR(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)offsetof(Vertex, position)));
        GL_CHECK_ERROR(glEnableVertexAttribArray(1));
        GL_CHECK_ERROR(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)offsetof(Vertex, color)));
        GL_CHECK_ERROR(glEnableVertexAttribArray(2));
        GL_CHECK_ERROR(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)offsetof(Vertex, texture_uv)));
        GL_CHECK_ERROR(glEnableVertexAttribArray(3));
        GL_CHECK_ERROR(glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)offsetof(Vertex, texture_index)));

        GL_CHECK_ERROR(glGenBuffers(1, &m_IndexBufferId));
        GL_CHECK_ERROR(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBufferId));
        GL_CHECK_ERROR(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));

        GL_CHECK_ERROR(glBindVertexArray(m_VertexArrayId));

        auto [vertexSource, fragmentSource] = Renderer::ReadShaderSource(FL_PROJECT_DIR + std::string("FlameUI/resources/shaders/TexturedQuad.glsl"));
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
        m_ShaderProgramId = glCreateProgram();

        // Attach our shaders to our program
        glAttachShader(m_ShaderProgramId, vertex_shader);
        glAttachShader(m_ShaderProgramId, fragment_shader);

        // Link our program
        glLinkProgram(m_ShaderProgramId);

        // Note the different functions here: glGetProgram* instead of glGetShader*.
        GLint isLinked = 0;
        glGetProgramiv(m_ShaderProgramId, GL_LINK_STATUS, (int*)&isLinked);
        if (isLinked == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetProgramiv(m_ShaderProgramId, GL_INFO_LOG_LENGTH, &maxLength);

            // The maxLength includes the NULL character
            std::vector<GLchar> infoLog(maxLength);
            glGetProgramInfoLog(m_ShaderProgramId, maxLength, &maxLength, &infoLog[0]);

            // We don't need the program anymore.
            glDeleteProgram(m_ShaderProgramId);
            // Don't leak shaders either.
            glDeleteShader(vertex_shader);
            glDeleteShader(fragment_shader);

            FL_ERROR("Error linking shader program:\n{0}", infoLog.data());
        }

        // Always detach shaders after a successful link.
        glDetachShader(m_ShaderProgramId, vertex_shader);
        glDetachShader(m_ShaderProgramId, fragment_shader);

        GL_CHECK_ERROR(glUseProgram(m_ShaderProgramId));
        int samplers[s_Max_Texture_Slots];
        for (uint16_t i = 0; i < s_Max_Texture_Slots; i++)
            samplers[i] = (int)i;
        GL_CHECK_ERROR(glUniform1iv(Renderer::GetUniformLocation("u_TextureSamplers", m_ShaderProgramId), s_Max_Texture_Slots, samplers));

        GL_CHECK_ERROR(glUseProgram(0));
    }

    void TexturedQuadBatch::OnDraw()
    {
        if (m_Vertices.size())
        {
            GL_CHECK_ERROR(glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferId));
            GL_CHECK_ERROR(glBufferSubData(GL_ARRAY_BUFFER, 0, m_Vertices.size() * sizeof(Vertex), m_Vertices.data()));

            for (uint8_t i = 0; i < m_TextureIds.size(); i++)
            {
                GL_CHECK_ERROR(glActiveTexture(GL_TEXTURE0 + i));
                GL_CHECK_ERROR(glBindTexture(GL_TEXTURE_2D, m_TextureIds[i]));
            }

            GL_CHECK_ERROR(glUseProgram(m_ShaderProgramId));
            GL_CHECK_ERROR(glBindVertexArray(m_VertexArrayId));
            GL_CHECK_ERROR(glDrawElements(GL_TRIANGLES, s_Max_Indices, GL_UNSIGNED_INT, 0));
        }
    }

    ///
    /// TextBatch Implementation ------------------------------------------------------------------------------
    ///

    bool TextBatch::IsFull()
    {
        if (m_Vertices.size() == s_Max_Vertices)
            return true;
        return false;
    }

    void TextBatch::AddQuad(const std::array<Vertex, 4>& vertices, uint32_t* location)
    {
        *location = m_Vertices.size();
        for (auto& vertex : vertices)
            m_Vertices.push_back(vertex);
    }

    void TextBatch::AddTextureId(uint32_t textureId)
    {
        m_TextureIds.emplace_back(textureId);
    }

    std::array<Vertex*, 4> TextBatch::GetPtrToQuadVertices(uint32_t location)
    {
        std::array<Vertex*, 4> ptr;
        for (uint8_t i = 0; i < 4; i++)
            ptr[i] = &m_Vertices[location + i];
        return ptr;
    }

    void TextBatch::SetQuadVertices(uint32_t location, const std::array<Vertex, 4>& vertices)
    {
        for (uint8_t i = 0; i < 4; i++)
            m_Vertices[location + i] = vertices[i];
    }

    void TextBatch::SetQuadZIndex(uint32_t location, float z)
    {
        for (uint8_t i = 0; i < 4; i++)
            m_Vertices[location + i].position.z = z;
    }

    TextBatch::~TextBatch()
    {
        glDeleteBuffers(1, &m_VertexBufferId);
        glDeleteBuffers(1, &m_IndexBufferId);
        glDeleteVertexArrays(1, &m_VertexArrayId);
        glDeleteProgram(m_ShaderProgramId);
        for (auto& textureId : m_TextureIds)
            glDeleteTextures(1, &textureId);
    }

    void TextBatch::Init()
    {
        uint32_t indices[s_Max_Indices];
        size_t offset = 0;
        for (size_t i = 0; i < s_Max_Indices; i += 6)
        {
            indices[0 + i] = 1 + offset;
            indices[1 + i] = 2 + offset;
            indices[2 + i] = 3 + offset;

            indices[3 + i] = 3 + offset;
            indices[4 + i] = 0 + offset;
            indices[5 + i] = 1 + offset;

            offset += 4;
        }

        GL_CHECK_ERROR(glGenVertexArrays(1, &m_VertexArrayId));
        GL_CHECK_ERROR(glBindVertexArray(m_VertexArrayId));

        GL_CHECK_ERROR(glGenBuffers(1, &m_VertexBufferId));
        GL_CHECK_ERROR(glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferId));
        GL_CHECK_ERROR(glBufferData(GL_ARRAY_BUFFER, s_Max_Vertices * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW));

        GL_CHECK_ERROR(glBindVertexArray(m_VertexArrayId));

        GL_CHECK_ERROR(glEnableVertexAttribArray(0));
        GL_CHECK_ERROR(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)offsetof(Vertex, position)));
        GL_CHECK_ERROR(glEnableVertexAttribArray(1));
        GL_CHECK_ERROR(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)offsetof(Vertex, color)));
        GL_CHECK_ERROR(glEnableVertexAttribArray(2));
        GL_CHECK_ERROR(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)offsetof(Vertex, texture_uv)));
        GL_CHECK_ERROR(glEnableVertexAttribArray(3));
        GL_CHECK_ERROR(glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)offsetof(Vertex, texture_index)));

        GL_CHECK_ERROR(glGenBuffers(1, &m_IndexBufferId));
        GL_CHECK_ERROR(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBufferId));
        GL_CHECK_ERROR(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));

        GL_CHECK_ERROR(glBindVertexArray(m_VertexArrayId));

        auto [vertexSource, fragmentSource] = Renderer::ReadShaderSource(FL_PROJECT_DIR + std::string("FlameUI/resources/shaders/Font.glsl"));
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
        m_ShaderProgramId = glCreateProgram();

        // Attach our shaders to our program
        glAttachShader(m_ShaderProgramId, vertex_shader);
        glAttachShader(m_ShaderProgramId, fragment_shader);

        // Link our program
        glLinkProgram(m_ShaderProgramId);

        // Note the different functions here: glGetProgram* instead of glGetShader*.
        GLint isLinked = 0;
        glGetProgramiv(m_ShaderProgramId, GL_LINK_STATUS, (int*)&isLinked);
        if (isLinked == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetProgramiv(m_ShaderProgramId, GL_INFO_LOG_LENGTH, &maxLength);

            // The maxLength includes the NULL character
            std::vector<GLchar> infoLog(maxLength);
            glGetProgramInfoLog(m_ShaderProgramId, maxLength, &maxLength, &infoLog[0]);

            // We don't need the program anymore.
            glDeleteProgram(m_ShaderProgramId);
            // Don't leak shaders either.
            glDeleteShader(vertex_shader);
            glDeleteShader(fragment_shader);

            FL_ERROR("Error linking shader program:\n{0}", infoLog.data());
        }

        // Always detach shaders after a successful link.
        glDetachShader(m_ShaderProgramId, vertex_shader);
        glDetachShader(m_ShaderProgramId, fragment_shader);

        GL_CHECK_ERROR(glUseProgram(m_ShaderProgramId));

        int samplers[s_Max_Texture_Slots];
        for (uint16_t i = 0; i < s_Max_Texture_Slots; i++)
            samplers[i] = (int)i;

        GL_CHECK_ERROR(glUniform1iv(Renderer::GetUniformLocation("u_TextureSamplers", m_ShaderProgramId), s_Max_Texture_Slots, samplers));

        GL_CHECK_ERROR(glUniform1f(Renderer::GetUniformLocation("u_PixelRange", m_ShaderProgramId), Renderer::GetFontProps().PixelRange));
        GL_CHECK_ERROR(glUniform1f(Renderer::GetUniformLocation("u_Strength", m_ShaderProgramId), Renderer::GetFontProps().Strength));

        GL_CHECK_ERROR(glUseProgram(0));
    }

    void TextBatch::OnDraw()
    {
        if (m_Vertices.size())
        {
            GL_CHECK_ERROR(glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferId));
            GL_CHECK_ERROR(glBufferSubData(GL_ARRAY_BUFFER, 0, m_Vertices.size() * sizeof(Vertex), m_Vertices.data()));

            for (uint8_t i = 0; i < m_TextureIds.size(); i++)
            {
                GL_CHECK_ERROR(glActiveTexture(GL_TEXTURE0 + i));
                GL_CHECK_ERROR(glBindTexture(GL_TEXTURE_2D, m_TextureIds[i]));
            }

            GL_CHECK_ERROR(glUseProgram(m_ShaderProgramId));
            GL_CHECK_ERROR(glBindVertexArray(m_VertexArrayId));
            GL_CHECK_ERROR(glDrawElements(GL_TRIANGLES, s_Max_Indices, GL_UNSIGNED_INT, 0));
        }
    }
}