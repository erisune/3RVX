// Copyright (c) 2015, Matthew Malensek.
// Distributed under the BSD 2-Clause License (see LICENSE.txt for details)

#pragma once

#include "OSD.h"

class NotifyIcon;

class KeyboardOSD : public OSD {
public:
    KeyboardOSD();
    ~KeyboardOSD();

    enum LockKey {
        CapsLock,
        NumLock,
        ScrollLock
    };

    void Hide();
    void HideIcon();
    void HideIcon(LockKey key);
    void ShowIcon(LockKey key);

    void ProcessHotkeys(HotkeyInfo &hki);

private:
    MeterWnd _mWnd;

    NotifyIcon *_capsIcon;
    NotifyIcon *_numIcon;
    NotifyIcon *_scrollIcon;
    HICON _capsIconImage;
    HICON _numIconImage;
    HICON _scrollIconImage;

    virtual void OnDisplayChange();

    virtual LRESULT WndProc(HWND hWnd, UINT message,
        WPARAM wParam, LPARAM lParam);
};
