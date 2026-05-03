// Copyright (c) 2015, Matthew Malensek.
// Copyright (c) 2026, erisune.
// Distributed under the GPL-3.0 License (see COPYING for details)

#pragma once

#include "../Meter.h"

class StaticImage : public Meter {
public:
    StaticImage(std::wstring bitmapName, int x, int y);
    virtual void Draw(Gdiplus::Bitmap *buffer, Gdiplus::Graphics *graphics);

private:
    Gdiplus::Rect _drawRect;
};