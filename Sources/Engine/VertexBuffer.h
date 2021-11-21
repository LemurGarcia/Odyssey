
#pragma once

#include <GL/glew.h>
#include <vector>

class GLBuffer
{
    friend class VertexBuffer;
public:
    GLBuffer(const GLBuffer&) = delete;
    GLBuffer(GLBuffer &&other);
    GLBuffer();
    ~GLBuffer();

private:
    GLuint m_handle = 0;
};

class VertexBuffer
{
public:
    static void Initialize();
    static void Deinitialize();
    static const VertexBuffer &FullScreenQuad();

    VertexBuffer(const VertexBuffer&) = delete;
    VertexBuffer(VertexBuffer &&other);
    VertexBuffer();
    ~VertexBuffer();

    void SetVertexData(const void *data, GLsizeiptr size);
    void SetIndexData(const std::vector<GLuint> &indices);
    void SetAttribute(GLuint location, GLint dimension, GLsizei stride, GLuint offset);
    void Bind() const;
    void Unbind() const;
    void DrawElements() const;

private:
    GLuint m_handle = 0;

    GLBuffer m_vbo;
    GLBuffer m_ibo;
    size_t m_count_indices = 0;
};
