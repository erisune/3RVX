// Copyright (c) 2015, Matthew Malensek.
// Copyright (c) 2026, erisune.
// Distributed under the GPL-3.0 License (see COPYING for details)

#pragma once

#include "../Meter.h"
#include <string>

class Text : public Meter {
public:
    Text(int x, int y, int width, int height,
        Gdiplus::Font *font, Gdiplus::StringAlignment align,
        std::wstring color, byte transparency,
        std::wstring formatString, byte glow);
    ~Text();

    virtual void Draw(Gdiplus::Bitmap *buffer, Gdiplus::Graphics *graphics);

protected:
    Gdiplus::Font *_font;
    Gdiplus::SolidBrush *_fontColor;
    Gdiplus::StringFormat _strFormat;
    std::wstring _formatString;
    size_t _replaceIndex; // index of [[PERC]] string
    byte _glow;
};