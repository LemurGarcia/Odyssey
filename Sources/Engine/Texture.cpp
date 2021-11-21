
#include "Texture.h"
#include "Engine.h"
#include <global.h>

#define STBI_ASSERT PV_ASSERT
#define STBI_NO_BMP
#define STBI_NO_PSD
#define STBI_NO_TGA
#define STBI_NO_GIF
#define STBI_NO_HDR
#define STBI_NO_PIC
#define STBI_NO_PNM
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glm/gtc/constants.hpp>

#include <array>

static GLint ChannelsToInternalFormat(int channels, Texture::Format format)
{
    switch (channels)
    {
    case 3: return format == Texture::Format::Uint8 ? GL_RGB8 : GL_RGB32F;
    case 4: return format == Texture::Format::Uint8 ? GL_RGBA8 : GL_RGBA32F;
    default:
        PV_FATAL("Unsupported channels: " << channels);
        return 0;
    }
}

static GLenum ChannelsToFormat(int channels)
{
    switch (channels)
    {
    case 3: return GL_RGB;
    case 4: return GL_RGBA;
    default:
        PV_FATAL("Unsupported channels: " << channels);
        return 0;
    }
}

static GLenum FormatToType(Texture::Format format)
{
    switch (format)
    {
    case Texture::Format::Uint8: return GL_UNSIGNED_BYTE;
    case Texture::Format::Float: return GL_FLOAT;
    default:
        PV_FATAL("Unsupported format: " << (uint32_t)format);
        return 0;
    }
}

static GLenum FormatToSize(Texture::Format format)
{
    switch (format)
    {
    case Texture::Format::Uint8: return 1;
    case Texture::Format::Float: return 4;
    default:
        PV_FATAL("Unsupported format: " << (uint32_t)format);
        return 0;
    }
}

// Buffer

Buffer::Buffer(int width, int height)
    : m_width(width)
    , m_height(height)
{
    m_data.resize(m_width * m_height);
}

int Buffer::GetWidth() const
{
    return m_width;
}

int Buffer::GetHeight() const
{
    return m_height;
}

float Buffer::Get(int x, int y) const
{
    uint32_t ux = (uint32_t)x % m_width;
    uint32_t uy = (uint32_t)y % m_height;
    return m_data[ux + uy * (uint32_t)m_width];
}

void Buffer::Set(int x, int y, float val)
{
    if (x < 0 || x > m_width - 1)
        return;
    if (y < 0 || y > m_height - 1)
        return;

    m_data[x + m_width * y] = val;
}

Buffer &Buffer::Draw(const Buffer &src, int x, int y)
{
    loopj(src.GetHeight())
        loopi(src.GetWidth())
            Set(x + i, y + j, src[i + j * src.GetWidth()]);
    return *this;
}

float Buffer::operator[](size_t index) const
{
    return m_data[index];
}

float &Buffer::operator[](size_t index)
{
    return m_data[index];
}

Buffer& Buffer::GaussianFast(const Buffer& src, int radius, int dx, int dy)
{
    PV_ASSERT_MSG(m_width == src.m_width &&
        m_height == src.m_height, "Sizes of buffers must be equal");
    PV_ASSERT_MSG(this != &src, "Source buffer must does not be same with this");

    auto Norm = [](float x) -> float
    {
        const float a = 1.0f / (std::sqrt(2.0f * glm::pi<float>()));
        const float b = -x * x / 2.0f;
        return a * std::exp(b);
    };

    loopj(m_height)
        loopi(m_width)
    {
        float count = 0.0f;
        float sum = 0.0f;
        for (int p = -radius; p <= radius; p++)
        {
            float x = (float)i + dx * p;
            float y = (float)j + dy * p;

            float sx = (float)(x - i) / radius;
            float sy = (float)(y - j) / radius;
            float r = std::sqrt(sx * sx + sy * sy);
            float koef = Norm(r * 3);
            count += koef;
            sum += koef * src.Get((int)x, (int)y);
        }
        m_data[i + j * m_width] = sum / count;
    }
    return *this;
}

Buffer& Buffer::Gaussian(int radius)
{
    Buffer buf(m_width, m_height);
    buf.GaussianFast(*this, radius, 1, 0);
    GaussianFast(buf, radius, 0, 1);
    return *this;
}

Buffer& Buffer::Mul(float value)
{
    loopi(m_width * m_height)
        m_data[i] *= value;
    return *this;
}

// Texture

Texture::Texture(Texture&& other)
    : m_texture_name(std::move(other.m_texture_name))
    , m_handle(other.m_handle)
    , m_width(other.m_width)
    , m_height(other.m_height)
    , m_format(other.m_format)
    , m_channels(other.m_channels)
    , m_data(std::move(other.m_data))
{
    other.m_handle = 0;
}

Texture::Texture(const std::string& name, WrapMode wrap_mode)
    : m_texture_name(name)
{
    int width, height, channels;
    unsigned char* data = stbi_load(Engine::AssetPath(name).c_str(), &width, &height, &channels, 0);
    PV_ASSERT_MSG(data, "Could not load texture: " << name);

    Create(data, width, height, channels, Format::Uint8, wrap_mode, Filter::Linear);

    PV_INFO("Create texture " << name << " - OK");
    stbi_image_free(data);
}

Texture::Texture(const Buffer& buffer, WrapMode wrap_mode)
    : m_texture_name("From buffer")
{
    std::vector<std::array<uint8_t, 4>> data;
    data.resize(buffer.GetWidth() * buffer.GetHeight());
    loopi(buffer.GetWidth() * buffer.GetHeight())
    {
        uint32_t color = std::min(static_cast<uint32_t>(buffer[i] * 255), 255u);
        uint8_t c = static_cast<uint8_t>(color);
        data[i] = {{ c, c, c, c }};
    }
    Create(data.data()->data(), buffer.GetWidth(), buffer.GetHeight(), 4, Format::Uint8, wrap_mode, Filter::Linear);
}

Texture::~Texture()
{
    if (!m_handle)
        return;

    glDeleteTextures(1, &m_handle);
}

GLuint Texture::GetHandle() const
{
    return m_handle;
}

int Texture::GetWidth() const
{
    return m_width;
}

int Texture::GetHeight() const
{
    return m_height;
}

int Texture::GetChannels() const
{
    return m_channels;
}

Buffer Texture::GetPixelsFromChannel(int channel) const
{
    switch (m_format)
    {
    case Format::Uint8: return GetPixelsFromChannel<uint8_t>(channel);
    case Format::Float: return GetPixelsFromChannel<float>(channel);
    default:
        PV_FATAL("Unsupported format: " << (int)m_format);
        return Buffer(0, 0);
    }
}

template<typename Type>
Buffer Texture::GetPixelsFromChannel(int channel) const
{
    PV_ASSERT(sizeof(Type) == FormatToSize(m_format));
    Buffer ret(m_width, m_height);
    size_t index = 0;
    const Type* begin_data = reinterpret_cast<const Type*>(m_data.data());
    for (const Type* data = begin_data; data < begin_data + m_data.size(); data += m_channels)
    {
        ret[index++] = data[channel];
    }
    return ret;
}

void Texture::Create(const GLubyte* data, int width, int height, int channels, Format format, WrapMode wrap_mode, Filter filter)
{
    GL_CHECK_ERROR;
    m_width = width;
    m_height = height;
    m_format = format;
    m_channels = channels;

    m_data.assign(data, data + width * height * channels * FormatToSize(format));

    glGenTextures(1, &m_handle);
    PV_ASSERT_MSG(m_handle, "Unable to create glTexture");
    glBindTexture(GL_TEXTURE_2D, m_handle);

    GLint mag_filter = filter == Filter::Nearest ? GL_NEAREST : GL_LINEAR;
    GLint min_filter = filter == Filter::Nearest ? GL_NEAREST : (filter == Filter::Linear ? GL_LINEAR : GL_LINEAR_MIPMAP_LINEAR);
    GLint clamp_mode = wrap_mode == WrapMode::Clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, clamp_mode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, clamp_mode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        ChannelsToInternalFormat(channels, format),
        m_width,
        m_height,
        0,
        ChannelsToFormat(channels),
        FormatToType(format),
        data);
}