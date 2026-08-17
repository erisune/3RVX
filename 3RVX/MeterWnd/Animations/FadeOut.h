// Copyright (c) 2015, Matthew Malensek.
// Copyright (c) 2026, erisune.
// Distributed under the GPL-3.0 License (see COPYING for details)

#pragma once

#include <cmath>

#include "../Animation.h"

class FadeOut : public Animation {
public:
    FadeOut(int speed, int opacity);

    virtual bool Animate(MeterWnd *meterWnd);
    virtual void Reset(MeterWnd *meterWnd);

    virtual int UpdateInterval();

private:
    unsigned char _opacity;
    int _interval;
    int _step;
    bool _initialized;
    virtual void Init(MeterWnd* meterWnd);
};