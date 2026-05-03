// Copyright (c) 2015, Matthew Malensek.
// Copyright (c) 2026, erisune.
// Distributed under the GPL-3.0 License (see COPYING for details)

#pragma once

#include "..\MeterWnd\MeterWnd.h"

class SliderKnob;

class SliderWnd : public MeterWnd {
public:
    bool Active() const;
    void Hide(bool hotkeyAction = false);

protected:
    bool _dragging;
    bool _vertical;

    SliderWnd(LPCWSTR className, LPCWSTR title, HINSTANCE hInstance = NULL);

    virtual void Show();

    void Knob(SliderKnob *knob);
    virtual void SliderChanged() = 0;
    virtual void ScrollUp() = 0;
    virtual void ScrollDown() = 0;
    virtual void KeyPress(unsigned int vk) = 0;

private:
    bool _ignoreInput;
    bool _active;
    SliderKnob *_knob;
    int _dragOffset;

    void PositionWindow();
    bool MouseOverKnob(int x, int y);
    bool MouseOverTrack(int x, int y);
    void UpdateKnob(int x, int y);

    virtual LRESULT WndProc(HWND hWnd, UINT message,
        WPARAM wParam, LPARAM lParam);

    static constexpr const int TIMER_IGNORE_INPUT = 50;
    static constexpr const int IGNORE_DURATION = 100;
    static constexpr const int TIMER_ACTIVE = 51;
    static constexpr const int ACTIVE_DELAY = 200;
};