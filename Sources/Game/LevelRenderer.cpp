#include "LevelRenderer.h"
#include "Engine/Font.h"

LevelRenderer::LevelRenderer()
    : m_level()
{
}

void LevelRenderer::Draw()
{
    Point size = m_level.GetSize();
    for (int j = 0; j < size.x; j++)
    {
        auto curr_string = m_level.GetStringByLine(j);
        Font::Render(L"#g" + curr_string, { -0.95f, -0.85f + j * 0.05f });
    }
    //Font::Render(L"#gFPS = " + std::to_wstring(CalcFPS(delta)), { -0.95f, -0.85f });
    Font::Render(L"#WHe-hey! #W", { -0.95f, -0.75f });
}
