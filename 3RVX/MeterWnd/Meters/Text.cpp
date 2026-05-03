// Copyright (c) 2015, Matthew Malensek.
// Copyright (c) 2026, erisune.
// Distributed under the GPL-3.0 License (see COPYING for details)

#include "Text.h"
#include <algorithm>

Text::Text(int x, int y, int width, int height,
        Gdiplus::Font *font, Gdiplus::StringAlignment align,
        std::wstring color, byte transparency,
        std::wstring formatString, byte glow) :
Meter(x, y, 100),
_font(font->Clone()),
_formatString(formatString) {
    _rect.Width = width;
    _rect.Height = height;

    _strFormat.SetAlignment(align);

    unsigned long c = wcstol(color.c_str(), '\0', 16);
    unsigned long a = transparency << 24;
    _fontColor = new Gdiplus::SolidBrush(c | a);

    _replaceIndex = _formatString.find(L"[[PERC]]");
    _glow = glow;
}

Text::~Text()
{
    delete _font;
    delete _fontColor;
}

void Text::Draw(Gdiplus::Bitmap* buffer, Gdiplus::Graphics* graphics)
{
    int units = CalcUnits();

    Gdiplus::RectF layoutRect((float)_rect.X, (float)_rect.Y,
        (float)_rect.Width, (float)_rect.Height);

    graphics->SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);

    std::wstring tempstr(_formatString);
    if (_replaceIndex != std::wstring::npos)
    {
        wchar_t perc[4];
        _itow_s(units * (100 / _units), perc, 10);
        tempstr.replace(_replaceIndex, 8, perc);
    }
    const wchar_t *str = tempstr.c_str();

    if (_glow > 0)
    {   
        graphics->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        Gdiplus::FontFamily glowFontFamily;
        _font->GetFamily(&glowFontFamily);
        Gdiplus::GraphicsPath path;
        path.AddString(str, -1, &glowFontFamily, _font->GetStyle(),
            _font->GetSize() * 1.345f, layoutRect, &_strFormat);
        for (float i = 0; i < _glow; i++)
        {
            Gdiplus::Pen pen(Gdiplus::Color(12, 255, 255, 255), i);
            pen.SetLineJoin(Gdiplus::LineJoinRound);
            graphics->DrawPath(&pen, &path);
        }
    }
    graphics->DrawString(str, -1, _font, layoutRect,
        &_strFormat, _fontColor);

    UpdateDrawnValues();
}