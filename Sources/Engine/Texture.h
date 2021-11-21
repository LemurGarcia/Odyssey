
#pragma once

#include <GL/glew.h>
#include <string>
#include <vector>

class Buffer
{
public:
    Buffer() = delete;
    Buffer(const Buffer&) = default;
    Buffer(Buffer&&) = default;
    Buffer(int width, int height);

    int GetWidth() const;
    int GetHeight() const;
    float Get(int x, int y) const;

    void Set(int x, int y, float val);
    Buffer &Draw(const Buffer &src, int x, int y);

    float operator[](size_t index) const;
    float &operator[](size_t index);

    Buffer& GaussianFast(const Buffer& src, int radius, int dx, int dy);
    Buffer& Gaussian(int radius);
    Buffer& Mul(float value);

private:
    const int m_width;
    const int m_height;
    std::vector<float> m_data;
};

class Texture
{
public:
    enum class WrapMode
    {
        Clamp,
        Repeat,
    };
    enum class Filter
    {
        Nearest,
        Linear,
    };
    enum class Format
    {
        Uint8,
        Float,
    };

    Texture(Texture&& other);
    Texture(const std::string& name, WrapMode wrap_mode);
    Texture(const Buffer& buffer, WrapMode wrap_mode);
    ~Texture();

    GLuint GetHandle() const;
    int GetWidth() const;
    int GetHeight() const;
    int GetChannels() const;

    Buffer GetPixelsFromChannel(int channel) const;

private:
    template<typename Type>
    Buffer GetPixelsFromChannel(int channel) const;
    void Create(const GLubyte* data, int width, int height, int channels, Format format, WrapMode wrap_mode, Filter filter);

private:
    std::string m_texture_name;
    GLuint m_handle = 0;
    int m_width = 0;
    int m_height = 0;
    int m_channels = 0;
    Format m_format = Format::Uint8;
    std::vector<GLubyte> m_data;
};
