// Copyright (c) 2015, Matthew Malensek.
// Distributed under the BSD 2-Clause License (see LICENSE.txt for details)

#pragma once

#include "KeyType.h"
#include "OSD.h"

class NotifyIcon;

class KeyboardOSD : public OSD {
public:
    KeyboardOSD();
    ~KeyboardOSD();

    void Hide();
    void HideIcon();
    void HideIcon(KeyType key);
    void Show();
    void Show(KeyType key);
    void ShowIcon();
    void ShowIcon(KeyType key);

    void ProcessHotkeys(HotkeyInfo &hki);

private:
    MeterWnd _mWnd;

    NotifyIcon *_capsIcon;
    NotifyIcon *_numIcon;
    NotifyIcon *_scrollIcon;
    HICON _capsIconImage;
    HICON _numIconImage;
    HICON _scrollIconImage;

    void UpdateMeters(KeyType key);

    virtual void OnDisplayChange();
    void OnKeyboardInput(WPARAM wParam, LPARAM lParam);

    virtual LRESULT WndProc(HWND hWnd, UINT message,
        WPARAM wParam, LPARAM lParam);
};
