
#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <string>

class Shader
{
    friend class Program;
public:
    static const Shader &SimpleVertexShader();
    static const Shader &SimpleFragmentShader();

    Shader() = delete;
    Shader(const Shader&) = delete;
    Shader(Shader &&other) = delete;
    Shader(const std::string &name, const std::string &source, GLenum type);
    ~Shader();

private:
    GLuint m_handle = 0;
};

class Program
{
public:
    static void Initialize();
    static void Deinitialize();
    static const Program &SimpleProgram();

    Program() = delete;
    Program(const Program&) = delete;
    Program(Program&& other);
    Program(const Shader &vertex_shader, const Shader &fragment_shader);
    ~Program();

    void Use() const;
    void Unuse() const;

    void SetUniform(const std::string& name, float scalar) const;
    void SetUniform(const std::string& name, const glm::vec2& vector) const;
    void SetUniform(const std::string& name, const glm::vec4& vector) const;
    void SetUniform(const std::string& name, const glm::mat3& matrix) const;
    void SetTexture(const std::string& name, const class Texture& texture, GLint level) const;

private:
    GLint GetUniformLocation(const std::string& name) const;

private:
    GLuint m_handle = 0;
};

class ProgramGuard
{
public:
    ProgramGuard(const Program &program);
    ~ProgramGuard();

private:
    const Program &m_program;
};
