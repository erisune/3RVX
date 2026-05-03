// Copyright (c) 2015, Matthew Malensek.
// Copyright (c) 2026, erisune.
// Distributed under the GPL-3.0 License (see COPYING for details)

#include "Bitstrip.h"

Bitstrip::Bitstrip(std::wstring bitmapName, int x, int y, int units) :
Meter(bitmapName, x, y, units) {
    _rect.Height = _bitmap->GetHeight() / _units;
}

void Bitstrip::Draw(Gdiplus::Bitmap *buffer, Gdiplus::Graphics *graphics) {
    int units = CalcUnits();
    int stripY = (units - 1) * _rect.Height;

    if (units == 0) {
        /* The mute OSD should be shown here, but we'll do something sane
         * rather than go negative. */
        stripY = 0;
    }

    graphics->DrawImage(_bitmap, _rect, 0, stripY, _rect.Width, _rect.Height,
        Gdiplus::UnitPixel, &_imageAttributes, NULL, NULL);

    UpdateDrawnValues();
}