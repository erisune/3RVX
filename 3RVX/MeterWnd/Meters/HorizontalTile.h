// Copyright (c) 2015, Matthew Malensek.
// Copyright (c) 2026, erisune.
// Distributed under the GPL-3.0 License (see COPYING for details)

#pragma once

#include "../Meter.h"

class HorizontalTile : public Meter {
public:
    HorizontalTile(std::wstring bitmapName,
        int x, int y, int units, bool reverse = false);
    ~HorizontalTile();

    virtual void Draw(Gdiplus::Bitmap *buffer, Gdiplus::Graphics *graphics);

protected:
    Gdiplus::TextureBrush *_texture;
    Gdiplus::Matrix *_shiftMat;
    bool _reverse;
};