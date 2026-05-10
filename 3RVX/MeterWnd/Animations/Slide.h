// Copyright (c) 2026, erisune.
// Distributed under the GPL-3.0 License (see COPYING for details)

#pragma once

#include <Windows.h>

#include "../Animation.h"

class Slide : public Animation {
public:
    Slide(int speed);

    virtual bool Animate(MeterWnd* meterWnd);
    virtual void Reset(MeterWnd* meterWnd);

    virtual int UpdateInterval();

private:
    enum SlideDirection {
        SlideToBottom,
        SlideToTop,
        SlideToRight,
        SlideToLeft,
    };
    SlideDirection _direction;
    POINT _point;
    RECT _rect;
    int _distance;
    int _interval;
    int _step;
    int _ticks;
    bool _initialized;
    virtual void Init(MeterWnd* meterWnd);
};