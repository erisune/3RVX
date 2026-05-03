// Copyright (c) 2015, Matthew Malensek.
// Copyright (c) 2026, erisune.
// Distributed under the GPL-3.0 License (see COPYING for details)

#include "VerticalTile.h"

void VerticalTile::Draw(Gdiplus::Bitmap *buffer, Gdiplus::Graphics *graphics)
{
    int currentUnits = CalcUnits();
    int height = _rect.Height * currentUnits;

    graphics->FillRectangle(_texture, _rect.X, _rect.Y, _rect.Width, height);

    UpdateDrawnValues();
}