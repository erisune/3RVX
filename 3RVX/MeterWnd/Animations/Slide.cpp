#include "Slide.h"

#include "..\MeterWnd.h"
#include "..\..\Logger.h"

Slide::Slide(int speed) :
    Animation(speed) {
    _initialized = false;
}

bool Slide::Animate(MeterWnd* meterWnd) {
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
    return false;
}

void Slide::Init(MeterWnd* meterWnd) {
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

    int iMin = iBottom;
    _direction = SlideToBottom;
    if (iTop < iMin) {
        iMin = iTop;
        _direction = SlideToTop;
    }
    if (iRight < iMin) {
        iMin = iRight;
        _direction = SlideToRight;
    }
    if (iLeft < iMin) {
        iMin = iLeft;
        _direction = SlideToLeft;
    }

    int bestError = iMin;
    int bestInterval = 10;
    for (int i = 10; i <= 20; ++i) {
        int si = max(_speed / i, 1);
        int error = iMin - iMin / si * si;
        if (error < bestError) {
            bestError = error;
            bestInterval = i;
        }
    }

    _interval = bestInterval;
    _step = iMin / (_speed / _interval);
    _initialized = true;
}

void Slide::Reset(MeterWnd* meterWnd) {
    if (!_initialized) {
        Slide::Init(meterWnd);
    }
    meterWnd->X(_point.x);
    meterWnd->Y(_point.y);
    meterWnd->Update();
}

int Slide::UpdateInterval() {
    return _interval;
}
