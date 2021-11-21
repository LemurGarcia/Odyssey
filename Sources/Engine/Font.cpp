
#include "Font.h"
#include "Engine.h"
#include "VertexBuffer.h"
#include "Shader.h"
#include "Texture.h"
#include <global.h>

#define STB_RECT_PACK_IMPLEMENTATION
#define STBRP_STATIC
#define STBRP_LARGE_RECTS
#define STBRP_ASSERT assert
#include <stb_rect_pack.h>

#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_assert assert
#define STBTT_STATIC
#include <stb_truetype.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_transform_2d.hpp>

#include <fstream>
#include <streambuf>
#include <map>
#include <memory>

struct Glyph
{
    int u = 0;
    int v = 0;
    float size = 0;
    const int x_offset;
    const int y_offset;
    Buffer buffer;

    Glyph(int width, int height, int x_offset, int y_offset, const unsigned char *data)
        : x_offset(x_offset)
        , y_offset(y_offset)
        , buffer(width, height)
    {
        loopi(width * height)
            buffer[i] = data[i] / 255.0f;
    }
};

static std::map<wchar_t, Glyph> s_glyphs;

constexpr int FONT_TEXTURE_SIZE = 512;
static std::unique_ptr<Texture> s_font_texture;
static std::unique_ptr<Program> s_font_program;

void Font::Initialize()
{
    Shader font_shader("FontFragmentShader", R"(
        uniform sampler2D tex_font;
        in vec2 texcoord;
        uniform vec4 color;
        out vec4 out_color;
        void main(void)
        {
            float alpha = texture(tex_font, texcoord).r;
            out_color = vec4(color.rgb, color.a * alpha);
        })",
        GL_FRAGMENT_SHADER);

    s_font_program = std::make_unique<Program>(Shader::SimpleVertexShader(), font_shader);
}

void Font::Deinitialize()
{
    s_font_texture = nullptr;
    s_font_program = nullptr;
}

void Font::Resize(float size)
{
    std::string font_name = Engine::AssetPath("Fonts/Gwent.ttf");
    std::ifstream file(font_name, std::ios::binary);
    PV_ASSERT_MSG(file, "The font " << font_name << " doesn't exists");

    std::string ttf_buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    const int font_offset = stbtt_GetFontOffsetForIndex((const unsigned char*)ttf_buffer.data(), 0);
    PV_ASSERT_MSG(font_offset >= 0, "FontData is incorrect");

    stbtt_fontinfo font;
    int result = stbtt_InitFont(&font, (const unsigned char*)ttf_buffer.data(), font_offset);
    PV_ASSERT_MSG(result, "Could not init font");

    std::vector<stbrp_rect> rects;
    s_glyphs.clear();

    auto GetCodepointBitmap = [&font, size, &rects](int code, std::map<wchar_t, Glyph>& glyphs)
    {
        const int padding = 1;
        int width, height, x_offset, y_offset;
        unsigned char* bitmap = stbtt_GetCodepointBitmap(&font, 0, stbtt_ScaleForPixelHeight(&font, size), code, &width, &height, &x_offset, &y_offset);

        glyphs.emplace(std::piecewise_construct,
            std::forward_as_tuple(code),
            std::forward_as_tuple(width, height, x_offset, y_offset, bitmap));

        stbtt_FreeBitmap(bitmap, font.userdata);

        stbrp_rect rect =
        {
            code,
            width + padding,
            height + padding,
        };
        rects.push_back(rect);
    };

    for (wchar_t code : LR"alphabet( abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,!?-+\/():;_|%=[]><)alphabet")
        GetCodepointBitmap(static_cast<int>(code), s_glyphs);

    // Cyrillic letters
    for (int code = 0x0410; code <= 0x044f; code++)
        GetCodepointBitmap(static_cast<int>(code), s_glyphs);

    // Start packing
    stbtt_pack_context spc = {};
    result = stbtt_PackBegin(&spc, nullptr, FONT_TEXTURE_SIZE, FONT_TEXTURE_SIZE, 0, 0, nullptr);
    PV_ASSERT_MSG(result, "Could not pack font to texture");

    stbtt_PackSetOversampling(&spc, 1, 1);

    result = stbrp_pack_rects((stbrp_context*)spc.pack_info, (stbrp_rect*)rects.data(), (int)rects.size());

    // End packing
    stbtt_PackEnd(&spc);

    if (!result)
    {
        PV_WARNING("Unable to create font atlas for size = " << size << ", then decrease size to 75%");
        return Resize(size * 0.75f);
    }

    Buffer font_buffer(FONT_TEXTURE_SIZE, FONT_TEXTURE_SIZE);
    for (const stbrp_rect &rect : rects)
    {
        Glyph &glyph = s_glyphs.at((wchar_t)rect.id);
        glyph.u = rect.x;
        glyph.v = rect.y;
        glyph.size = size;
        font_buffer.Draw(glyph.buffer, rect.x, rect.y);
    }

    s_font_texture = std::make_unique<Texture>(font_buffer, Texture::WrapMode::Clamp);

    PV_INFO("Font " << font_name << " for size = " << size << " was rendered");
}

void Font::Render(const std::wstring &text, const glm::vec2 &pos)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    ProgramGuard program_guard(*s_font_program);
    s_font_program->SetTexture("tex_font", *s_font_texture, 0);

    glm::vec4 current_color(1, 1, 1, 1);
    glm::vec2 current_pos = pos;
    loopi (text.size())
    {
        wchar_t symbol = text[i];
        if (symbol == L'#')
        {
            symbol = text[++i];
            if (symbol == L'r')      { current_color = { 1,     0.25f, 0.25f, 1 }; }
            else if (symbol == L'g') { current_color = { 0,     1,     0,     1 }; }
            else if (symbol == L'b') { current_color = { 0.25f, 0.25f, 1,     1 }; }
            else if (symbol == L'w') { current_color = { 1,     1,     1,     1 }; }
            else if (symbol == L'B') { current_color = { 0,     0,     0,     1 }; }
            else if (symbol == L'y') { current_color = { 1,     1,     0,     1 }; }
            else if (symbol == L'G') { current_color = { 0.5f,  0.5f,  0.5f,  1 }; }
            continue;
        }

        auto glyph = s_glyphs.find(symbol);
        if (glyph == s_glyphs.end())
            glyph = s_glyphs.find(L'?');

        if (symbol == L' ')
        {
            current_pos.x += 0.5f * glyph->second.size / Engine::GetWidth();
            continue;
        }

        int width = glyph->second.buffer.GetWidth();
        int height = glyph->second.buffer.GetHeight();
        int x_pos = glyph->second.u;
        int y_pos = glyph->second.v;
        int x_offset = glyph->second.x_offset;
        int y_offset = glyph->second.y_offset;

        float symbol_width = (float)width / Engine::GetWidth();
        float symbol_height = (float)height / Engine::GetHeight();
        float symbol_x_offset = (float)x_offset / Engine::GetWidth();
        float symbol_y_offset = (float)y_offset / Engine::GetHeight();
        current_pos.x += symbol_width;
        glm::mat3 mat_pos = glm::translate(glm::scale(glm::mat3(1), { 1, -1 }), current_pos + glm::vec2(symbol_x_offset, symbol_y_offset));
        mat_pos = glm::scale(mat_pos, { symbol_width, symbol_height });
        current_pos.x += symbol_width;
        current_pos.x += 0.05f * glyph->second.size / Engine::GetWidth();

        glm::mat3 mat_tex = glm::scale(glm::translate(glm::mat3(1), { 0, 1 }), { (float)width / s_font_texture->GetWidth(), -(float)height / s_font_texture->GetHeight() });
        mat_tex = glm::translate(mat_tex, { (float)x_pos / width, (float)y_pos / height });

        s_font_program->SetUniform("mat_pos", mat_pos);
        s_font_program->SetUniform("mat_tex", mat_tex);
        s_font_program->SetUniform("color", current_color);
        VertexBuffer::FullScreenQuad().DrawElements();
    }
    glDisable(GL_BLEND);
}
