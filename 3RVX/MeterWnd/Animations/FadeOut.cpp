// Copyright (c) 2015, Matthew Malensek.
// Distributed under the BSD 2-Clause License (see LICENSE.txt for details)

#include "FadeOut.h"

#include "..\MeterWnd.h"
#include "..\..\Logger.h"

FadeOut::FadeOut(int speed) :
Animation(speed) {
    _initialized = false;
}

bool FadeOut::Animate(MeterWnd *meterWnd) {
    byte current = meterWnd->Transparency();
    int newTrans = current - _step;
    if (newTrans < 0) {
        newTrans = 0;
        return true;
    }

    meterWnd->Transparency(newTrans);
    return false;
}

void FadeOut::Init(MeterWnd* meterWnd) {
    _opacity = meterWnd->Transparency();
    /* Determine the best step/interval combination that gets us a nice linear
     * animation without being excessively early or late (based on the speed) */
    int bestError = _opacity;
    int bestInterval = 10;
    for (int i = 10; i <= 20; ++i) {
        int si = max(_speed / i, 1);
        int error = _opacity - _opacity / si * si;
        if (error < bestError) {
            bestError = error;
            bestInterval = i;
        }
    }

    _interval = bestInterval;
    _step = _opacity / max(_speed / _interval, 1);
    _initialized = true;
}

void FadeOut::Reset(MeterWnd *meterWnd) {
    if (!_initialized) {
        FadeOut::Init(meterWnd);
    }
    meterWnd->Transparency(_opacity);
}

int FadeOut::UpdateInterval() {
    return _interval;
}
