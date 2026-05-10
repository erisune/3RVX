// Copyright (c) 2026, erisune.
// Distributed under the GPL-3.0 License (see COPYING for details)

#include "Slide.h"

#include "..\MeterWnd.h"
#include "..\..\Logger.h"

Slide::Slide(int speed) :
    Animation(speed) {
    _initialized = false;
}

bool Slide::Animate(MeterWnd* meterWnd) {
    if (meterWnd->Bitmap() == nullptr) {
        return false;
    }
    switch (_direction) {
        int newPos;
    case SlideToBottom:
        newPos = meterWnd->Y() + _step;
        if (newPos > _rect.bottom + meterWnd->Height()) {
            return true;
        }
        meterWnd->Y(newPos);
        break;
    case SlideToTop:
        newPos = meterWnd->Y() - _step;
        if (newPos < _rect.top - meterWnd->Height()) {
            return true;
        }
        meterWnd->Y(newPos);
        break;
    case SlideToRight:
        newPos = meterWnd->X() + _step;
        if (newPos > _rect.right + meterWnd->Width()) {
            return true;
        }
        meterWnd->X(newPos);
        break;
    case SlideToLeft:        
        newPos = meterWnd->X() - _step;
        if (newPos < _rect.left - meterWnd->Width()) {
            return true;
        }
        meterWnd->X(newPos);
        break;
    }
    meterWnd->Update();
    _ticks++;
    if (_ticks >= _speed / _distance) {
        _step++;
        _ticks = 0;
    }
    return false;
}

void Slide::Init(MeterWnd* meterWnd) {
    if (meterWnd->Bitmap() == nullptr) {
        return;
    }
    _point = { meterWnd->X(), meterWnd->Y() };
    HMONITOR monitor = MonitorFromPoint(_point, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mInfo = {};
    mInfo.cbSize = sizeof(mInfo);
    GetMonitorInfo(monitor, &mInfo);
    _rect = mInfo.rcWork;

    /* Get closest edge */
    int iRight = _rect.right - meterWnd->X() - meterWnd->Width();
    int iLeft = _rect.left + meterWnd->X();
    int iBottom = _rect.bottom - meterWnd->Y();
    int iTop = _rect.top + meterWnd->Y() + meterWnd->Height();

    _distance = iBottom;
    _direction = SlideToBottom;
    if (iTop < _distance) {
        _distance = iTop;
        _direction = SlideToTop;
    }
    if (iRight < _distance) {
        _distance = iRight;
        _direction = SlideToRight;
    }
    if (iLeft < _distance) {
        _distance = iLeft;
        _direction = SlideToLeft;
    }

    int bestError = _distance;
    int bestInterval = 8;
    for (int i = 8; i <= 12; ++i) {
        int si = max(_speed / i, 1);
        int error = _distance - _distance / si * si;
        if (error < bestError) {
            bestError = error;
            bestInterval = i;
        }
    }

    _interval = bestInterval;
    _step = max(_distance / (2 * max(_speed / _interval, 1)), 1);
    _initialized = true;
}

void Slide::Reset(MeterWnd* meterWnd) {
    if (meterWnd->Bitmap() == nullptr) {
        return;
    }
    if (!_initialized) {
        Slide::Init(meterWnd);
    } else {
        _step = max(_distance / (2 * max(_speed / _interval, 1)), 1);
    }
    _ticks = 0;
    meterWnd->X(_point.x);
    meterWnd->Y(_point.y);
    meterWnd->Update();
}

int Slide::UpdateInterval() {
    return _interval;
}
