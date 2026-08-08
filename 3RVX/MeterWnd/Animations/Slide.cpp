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
    HMONITOR hMonitor = MonitorFromPoint(_point, MONITOR_DEFAULTTOPRIMARY);
    switch (_direction) {
    case SlideToBottom:
        {
            long nPos = meterWnd->Y() + _step;
            long nLimit = _rect.bottom;
            POINT p1 = { 0, nLimit };
            POINT p2 = { 0, _rect.top };
            HMONITOR hAux1 = MonitorFromPoint(p1, MONITOR_DEFAULTTONEAREST);
            HMONITOR hAux2 = MonitorFromPoint(p2, MONITOR_DEFAULTTONEAREST);
            if (hMonitor != hAux1 || hMonitor != hAux2) {
                nLimit = _rect.bottom - meterWnd->Height();
            }
            if (nPos > nLimit) {
                return true;
            }
            meterWnd->Y(nPos);
            for (LayeredWnd *clone : meterWnd->Clones()) {
                long nClonePos = clone->Y() + _step;
                clone->Y(nClonePos);
            }
        }
        break;
    case SlideToTop:
        {
            long nPos = meterWnd->Y() - _step;
            long nLimit = _rect.top - meterWnd->Height();
            POINT p1 = { 0, nLimit };
            POINT p2 = { 0, _rect.bottom };
            HMONITOR hAux1 = MonitorFromPoint(p1, MONITOR_DEFAULTTONEAREST);
            HMONITOR hAux2 = MonitorFromPoint(p2, MONITOR_DEFAULTTONEAREST);
            if (hMonitor != hAux1 || hMonitor != hAux2) {
                nLimit = _rect.top;
            }
            if (nPos < nLimit) {
                return true;
            }
            meterWnd->Y(nPos);
            for (LayeredWnd *clone : meterWnd->Clones()) {
                long nClonePos = clone->Y() - _step;
                clone->Y(nClonePos);
            }
        }
        break;
    case SlideToRight:
        {
            long nPos = meterWnd->X() + _step;
            long nLimit = _rect.right;
            POINT p1 = { nLimit, 0 };
            POINT p2 = { _rect.left, 0 };
            HMONITOR hAux1 = MonitorFromPoint(p1, MONITOR_DEFAULTTONEAREST);
            HMONITOR hAux2 = MonitorFromPoint(p2, MONITOR_DEFAULTTONEAREST);
            if (hMonitor != hAux1 || hMonitor != hAux2) {
                nLimit = _rect.right - meterWnd->Width();
            }
            if (nPos > nLimit) {
                return true;
            }
            meterWnd->X(nPos);
            for (LayeredWnd *clone : meterWnd->Clones()) {
                long nClonePos = clone->X() + _step;
                clone->X(nClonePos);
            }
        }
        break;
    case SlideToLeft:
        {
            long nPos = meterWnd->X() - _step;
            long nLimit = _rect.left - meterWnd->Width();
            POINT p1 = { nLimit, 0 };
            POINT p2 = { _rect.right, 0 };
            HMONITOR hAux1 = MonitorFromPoint(p1, MONITOR_DEFAULTTONEAREST);
            HMONITOR hAux2 = MonitorFromPoint(p2, MONITOR_DEFAULTTONEAREST);
            if (hMonitor != hAux1 || hMonitor != hAux2) {
                nLimit = _rect.left;
            }
            if (nPos < nLimit) {
                return true;
            }
            meterWnd->X(nPos);
            for (LayeredWnd *clone : meterWnd->Clones()) {
                long nClonePos = clone->X() - _step;
                clone->X(nClonePos);
            }
        }
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
    _clonePoints.clear();
    for (LayeredWnd *clone : meterWnd->Clones()) {
        POINT p = { clone->X(), clone->Y() };
        _clonePoints.push_back(p);
    }
    HMONITOR monitor = MonitorFromPoint(_point, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mInfo = {};
    mInfo.cbSize = sizeof(mInfo);
    GetMonitorInfo(monitor, &mInfo);
    RECT rcWork = mInfo.rcWork;
    _rect = mInfo.rcMonitor;

    /* Get closest edge */
    long sides[] = {
        rcWork.bottom - meterWnd->Y(),
        rcWork.top + meterWnd->Y() + meterWnd->Height(),
        rcWork.right - meterWnd->X() - meterWnd->Width(),
        rcWork.left + meterWnd->X()
    };

    long distances[] = {
        _rect.bottom - meterWnd->Y(),
        _rect.top + meterWnd->Y() + meterWnd->Height(),
        _rect.right - meterWnd->X() - meterWnd->Width(),
        _rect.left + meterWnd->X()
    };

    long nSide = sides[0];
    for (int i = 1; i < 4; i++) {
        if (sides[i] < nSide) {
            nSide = sides[i];
            _direction = static_cast<SlideDirection>(i);
        }
    }

    _distance = distances[_direction];

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
    int nIndex = 0;
    for (LayeredWnd *clone : meterWnd->Clones()) {
        clone->X(_clonePoints.at(nIndex).x);
        clone->Y(_clonePoints.at(nIndex).y);
        nIndex++;
    }
    meterWnd->Update();
}

int Slide::UpdateInterval() {
    return _interval;
}
