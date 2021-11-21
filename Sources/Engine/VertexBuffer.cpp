
#include "VertexBuffer.h"
#include <global.h>

#include <glm/glm.hpp>
#include <memory>

static std::unique_ptr<VertexBuffer> s_fullscreen_quad;

// Buffer

GLBuffer::GLBuffer()
{
    glGenBuffers(1, &m_handle);
    PV_ASSERT_MSG(m_handle, "Could not create buffer");
}

GLBuffer::GLBuffer(GLBuffer &&other)
    : m_handle(other.m_handle)
{
    other.m_handle = 0;
}

GLBuffer::~GLBuffer()
{
    if (!m_handle)
        return;

    glDeleteBuffers(1, &m_handle);
}

// VertexBuffer

void VertexBuffer::Initialize()
{
    struct Vertex
    {
        glm::vec3 pos;
        glm::vec3 tex;
    };
    static const std::vector<Vertex> vertices =
    {
        { { -1.0f, -1.0f, 1.0f, }, {  0.0f,  0.0f, 1.0f, }, },
        { {  1.0f, -1.0f, 1.0f, }, {  1.0f,  0.0f, 1.0f, }, },
        { { -1.0f,  1.0f, 1.0f, }, {  0.0f,  1.0f, 1.0f, }, },
        { {  1.0f,  1.0f, 1.0f, }, {  1.0f,  1.0f, 1.0f, }, },
    };
    static const std::vector<GLuint> indices = { 0, 1, 2, 3, };

    s_fullscreen_quad = std::make_unique<VertexBuffer>();
    s_fullscreen_quad->Bind();
    s_fullscreen_quad->SetVertexData(vertices.data(), vertices.size() * sizeof(Vertex));
    s_fullscreen_quad->SetIndexData(indices);
    s_fullscreen_quad->SetAttribute(0, 3, sizeof(Vertex), offsetof(Vertex, pos));
    s_fullscreen_quad->SetAttribute(1, 3, sizeof(Vertex), offsetof(Vertex, tex));
    s_fullscreen_quad->Unbind();
}

void VertexBuffer::Deinitialize()
{
    s_fullscreen_quad = nullptr;
}

const VertexBuffer &VertexBuffer::FullScreenQuad()
{
    return *s_fullscreen_quad;
}

VertexBuffer::VertexBuffer()
{
    glGenVertexArrays(1, &m_handle);
    PV_ASSERT_MSG(m_handle, "Could not create vertex array buffer");
}

VertexBuffer::VertexBuffer(VertexBuffer &&other)
    : m_handle(other.m_handle)
    , m_vbo(std::move(other.m_vbo))
    , m_ibo(std::move(other.m_ibo))
    , m_count_indices(other.m_count_indices)
{
    other.m_handle = 0;
}

VertexBuffer::~VertexBuffer()
{
    if (!m_handle)
        return;

    glDeleteVertexArrays(1, &m_handle);
}

void VertexBuffer::SetVertexData(const void *data, GLsizeiptr size)
{
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo.m_handle);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

void VertexBuffer::SetIndexData(const std::vector<GLuint> &indices)
{
    m_count_indices = indices.size();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo.m_handle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
}

void VertexBuffer::SetAttribute(GLuint location, GLint dimension, GLsizei stride, GLuint offset)
{
    glEnableVertexAttribArray(location);
    glVertexAttribPointer(
        location,
        dimension,
        GL_FLOAT,
        false,
        stride,
        reinterpret_cast<void*>(static_cast<size_t>(offset))
    );
}

void VertexBuffer::Bind() const
{
    glBindVertexArray(m_handle);
}

void VertexBuffer::Unbind() const
{
    glBindVertexArray(0);
}

void VertexBuffer::DrawElements() const
{
    Bind();
    glDrawElements(
        GL_TRIANGLE_STRIP,          // mode
        (GLsizei)m_count_indices,   // count
        GL_UNSIGNED_INT,            // type
        nullptr                     // element array buffer offset
    );
    Unbind();
}
