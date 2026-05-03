// Copyright (c) 2015, Matthew Malensek.
// Copyright (c) 2026, erisune.
// Distributed under the GPL-3.0 License (see COPYING for details)

#include "StaticImage.h"

StaticImage::StaticImage(std::wstring bitmapName, int x, int y) :
Meter(bitmapName, x, y, 1) {

}

void StaticImage::Draw(Gdiplus::Bitmap *buffer, Gdiplus::Graphics *graphics) {
    graphics->DrawImage(_bitmap, _rect,
        0, 0, _rect.Width, _rect.Height, Gdiplus::UnitPixel);

    UpdateDrawnValues();
}