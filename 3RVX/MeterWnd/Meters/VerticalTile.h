// Copyright (c) 2015, Matthew Malensek.
// Copyright (c) 2026, erisune.
// Distributed under the GPL-3.0 License (see COPYING for details)

#pragma once

#include "HorizontalTile.h"

class VerticalTile : public HorizontalTile {
public:
    VerticalTile(std::wstring bitmapName, int x, int y, int units) :
    HorizontalTile(bitmapName, x, y, units) { }

    virtual void Draw(Gdiplus::Bitmap *buffer, Gdiplus::Graphics *graphics);
};