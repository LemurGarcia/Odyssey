
#include "Shader.h"
#include "Texture.h"
#include <global.h>

#include <glm/gtc/type_ptr.hpp>

#include <memory>

static std::unique_ptr<Shader> s_simple_vertex_shader;
static std::unique_ptr<Shader> s_simple_fragment_shader;
static std::unique_ptr<Program> s_simple_program;

// Shader

const Shader &Shader::SimpleVertexShader()
{
    return *s_simple_vertex_shader;
}

const Shader &Shader::SimpleFragmentShader()
{
    return *s_simple_fragment_shader;
}

Shader::Shader(const std::string &name, const std::string &source, GLenum type)
{
    GL_CHECK_ERROR;
    m_handle = glCreateShader(type);
    PV_ASSERT_MSG(m_handle, "Impossible to create a new Shader");

    std::string extended_source =
#if _GLES
        "#version 300 es\n";
    if (type == GL_FRAGMENT_SHADER)
        extended_source += "precision mediump float;\n";
#else
        "#version 400\n";
#endif
    extended_source += source;

    const GLchar* shader_text(extended_source.data());
    glShaderSource(m_handle, 1, &shader_text, nullptr);
    glCompileShader(m_handle);

    GLint compile_status;
    glGetShaderiv(m_handle, GL_COMPILE_STATUS, &compile_status);
    if (compile_status != GL_TRUE)
    {
        GLsizei logsize = 0;
        glGetShaderiv(m_handle, GL_INFO_LOG_LENGTH, &logsize);

        std::string log;
        log.resize(logsize);
        glGetShaderInfoLog(m_handle, logsize, &logsize, &log[0]);

        PV_ERROR("compilation error: " << name);
        PV_FATAL(log);
    }

    PV_INFO("Shader " << name << " compiled successfully");
}

Shader::~Shader()
{
    glDeleteShader(m_handle);
}

// Program

void Program::Initialize()
{
    static const std::string vertex = R"(
        layout(location = 0) in vec3 in_position;
        layout(location = 1) in vec3 in_texcoord;
        uniform mat3 mat_tex;
        uniform mat3 mat_pos;
        out vec2 texcoord;
        void main()
        {
            texcoord = (mat_tex * in_texcoord).xy;
            texcoord.y = 1.0 - texcoord.y;
            gl_Position = vec4(mat_pos * in_position, 1.0);
        })";
    s_simple_vertex_shader = std::make_unique<Shader>("SimpleVertexShader", vertex, GL_VERTEX_SHADER);

    static const std::string fragment = R"(
        uniform sampler2D tex;
        in vec2 texcoord;
        out vec4 out_color;
        void main(void)
        {
            out_color = texture(tex, texcoord);
        })";
    s_simple_fragment_shader = std::make_unique<Shader>("SimpleFragmentShader", fragment, GL_FRAGMENT_SHADER);

    s_simple_program = std::make_unique<Program>(*s_simple_vertex_shader, *s_simple_fragment_shader);
}

void Program::Deinitialize()
{
    s_simple_vertex_shader = nullptr;
    s_simple_fragment_shader = nullptr;
    s_simple_program = nullptr;
}

const Program &Program::SimpleProgram()
{
    return *s_simple_program;
}

Program::Program(Program&& other)
    : m_handle(other.m_handle)
{
    other.m_handle = 0;
}

Program::Program(const Shader &vertex_shader, const Shader &fragment_shader)
{
    GL_CHECK_ERROR;
    m_handle = glCreateProgram();
    PV_ASSERT_MSG(m_handle, "Impossible to create a new shader program");

    glAttachShader(m_handle, vertex_shader.m_handle);
    glAttachShader(m_handle, fragment_shader.m_handle);

    glLinkProgram(m_handle);
    GLint result;
    glGetProgramiv(m_handle, GL_LINK_STATUS, &result);
    if (result != GL_TRUE)
    {
        PV_ERROR("linkage error");

        GLsizei logsize = 0;
        glGetProgramiv(m_handle, GL_INFO_LOG_LENGTH, &logsize);

        std::string log;
        log.resize(logsize);
        glGetProgramInfoLog(m_handle, logsize, &logsize, &log[0]);
        PV_FATAL(log);
    }
}

Program::~Program()
{
    glDeleteProgram(m_handle);
}

void Program::Use() const
{
    glUseProgram(m_handle);
}

void Program::Unuse() const
{
    glUseProgram(0);
}

void Program::SetUniform(const std::string& name, float scalar) const
{
    glUniform1f(GetUniformLocation(name), scalar);
}

void Program::SetUniform(const std::string& name, const glm::vec2& vector) const
{
    glUniform2fv(GetUniformLocation(name), 1, glm::value_ptr(vector));
}

void Program::SetUniform(const std::string& name, const glm::vec4& vector) const
{
    glUniform4fv(GetUniformLocation(name), 1, glm::value_ptr(vector));
}

void Program::SetUniform(const std::string& name, const glm::mat3& matrix) const
{
    glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(matrix));
}

void Program::SetTexture(const std::string& name, const Texture& texture, GLint level) const
{
    glUniform1i(GetUniformLocation(name), level);
    glActiveTexture((GLenum)(GL_TEXTURE0 + level));
    glBindTexture(GL_TEXTURE_2D, texture.GetHandle());
}

GLint Program::GetUniformLocation(const std::string& name) const
{
    GLint location = glGetUniformLocation(m_handle, name.c_str());
    return location;
}

// ProgramGuard

ProgramGuard::ProgramGuard(const Program &program)
    : m_program(program)
{
    m_program.Use();
}

ProgramGuard::~ProgramGuard()
{
    m_program.Unuse();
}
