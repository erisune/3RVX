// Copyright (c) 2015, Matthew Malensek.
// Copyright (c) 2026, erisune.
// Distributed under the GPL-3.0 License (see COPYING for details)

#include "CallbackMeter.h"

void CallbackMeter::Draw(Gdiplus::Bitmap *buffer, Gdiplus::Graphics *graphics) {
    int units = CalcUnits();
    _receiver.MeterChangeCallback(units);
    UpdateDrawnValues();
}

std::wstring CallbackMeter::ToString() {
    std::wstring baseStr = Meter::ToString();
    baseStr.append(L"\n");
    baseStr.append(L"(Callback Meter)");
    return baseStr;
}