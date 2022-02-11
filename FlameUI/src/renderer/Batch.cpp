#include "Batch.h"
#include <fstream>
#include <sstream>
#include <string>
#include <tuple>
#include "Renderer.h"

namespace FlameUI {
    void Batch::AddQuad(Vertex* vertices)
    {
        for (uint8_t i = 0; i < 4; i++)
            m_Vertices.push_back(vertices[i]);
    }

    void Batch::OnDraw()
    {
        if (!m_Vertices.size())
            return;

        glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferId);
        glBufferSubData(GL_ARRAY_BUFFER, 0, m_Vertices.size() * sizeof(Vertex), m_Vertices.data());

        glUseProgram(m_ShaderProgramId);
        glUniform1f(Renderer::GetUniformLocation("u_TitleBarHeight", m_ShaderProgramId), Renderer::ConvertYAxisPixelValueToOpenGLValue(TITLE_BAR_HEIGHT));

        glBindVertexArray(m_VertexArrayId);
        glDrawElements(GL_TRIANGLES, (m_Vertices.size() / 4) * 6, GL_UNSIGNED_INT, 0);
    }

    void Batch::Empty()
    {
        m_Vertices.clear();
    }

    void Batch::Init()
    {
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

        glGenVertexArrays(1, &m_VertexArrayId);
        glBindVertexArray(m_VertexArrayId);

        glGenBuffers(1, &m_VertexBufferId);
        glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferId);
        glBufferData(GL_ARRAY_BUFFER, MAX_VERTICES * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

        glBindVertexArray(m_VertexArrayId);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 13 * sizeof(float), (void*)offsetof(Vertex, position));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 13 * sizeof(float), (void*)offsetof(Vertex, color));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 13 * sizeof(float), (void*)offsetof(Vertex, texture_uv));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 13 * sizeof(float), (void*)offsetof(Vertex, texture_index));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, 13 * sizeof(float), (void*)offsetof(Vertex, quad_dimensions));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, 13 * sizeof(float), (void*)offsetof(Vertex, element_type_index));

        glGenBuffers(1, &m_IndexBufferId);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBufferId);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glBindVertexArray(m_VertexArrayId);

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

        glm::vec4 panelTitleBarColor = Renderer::GetThemeInfo().panelTitleBarColor;
        glUseProgram(m_ShaderProgramId);
        glUniform4f(Renderer::GetUniformLocation("u_PanelTitleBarColor", m_ShaderProgramId), panelTitleBarColor.x, panelTitleBarColor.y, panelTitleBarColor.z, panelTitleBarColor.w);
        glUseProgram(0);
    }
}