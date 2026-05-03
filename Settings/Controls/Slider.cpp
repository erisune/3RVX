// Copyright (c) 2015, Matthew Malensek.
// Copyright (c) 2026, erisune.
// Distributed under the GPL-3.0 License (see COPYING for details)

#include "Slider.h"

void Slider::Buddy(Control *buddy, bool bottomOrRight) {
    _buddyWnd = buddy->Handle();
    SendMessage(_hWnd, TBM_SETBUDDY,
        (WPARAM) bottomOrRight ? FALSE : TRUE, (LPARAM) _buddyWnd);
}

int Slider::Position() {
    return (int) SendMessage(_hWnd, TBM_GETPOS, 0, 0);
}

void Slider::Position(int position) {
    SendMessage(_hWnd, TBM_SETPOS, (WPARAM) TRUE, (LPARAM) position);
    if (OnSlide) {
        OnSlide();
    }
}

void Slider::Range(int lo, int hi) {
    SendMessage(_hWnd, TBM_SETRANGE, (WPARAM) TRUE, MAKELPARAM(lo, hi));
}

BOOL Slider::Scroll(bool horizontal, WORD request, WORD position) {
    if (OnSlide) {
        return OnSlide();
    }

    return FALSE;
}
