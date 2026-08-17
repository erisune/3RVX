// Copyright (c) 2015, Matthew Malensek.
// Copyright (c) 2026, erisune.
// Distributed under the GPL-3.0 License (see COPYING for details)

#pragma once

class MeterWnd;

class Animation {
public:
    Animation(int speed, int opacity) :
        _speed(speed),
        _opacity(opacity) {

    }

    virtual bool Animate(MeterWnd *meterWnd) = 0;
    virtual void Reset(MeterWnd *meterWnd) = 0;

    virtual int UpdateInterval() = 0;

protected:
    int _speed;
    int _opacity;
};