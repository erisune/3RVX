// Copyright (c) 2015, Matthew Malensek.
// Copyright (c) 2026, erisune.
// Distributed under the GPL-3.0 License (see COPYING for details)

#pragma once

#include "..\MeterWnd\MeterCallbackReceiver.h"
#include "..\NotifyIcon.h"
#include "..\Slider\BrightnessSlider.h"
#include "OSD.h"

class CallbackMeter;
class BrightnessController;
class BrightnessSlider;
class SoundPlayer;

class BrightnessOSD : public OSD, MeterCallbackReceiver {
public:
    BrightnessOSD();
    ~BrightnessOSD();

    void Hide();
    void HideIcon();
    void Show();

    virtual void ProcessHotkeys(HotkeyInfo &hki);

private:
    BrightnessController *_brightnessCtrl;
    float _defaultIncrement;
    bool _validSkin;
    bool _validSlider;

    MeterWnd _mWnd;
    CallbackMeter *_callbackMeter;
    BrightnessSlider *_brightnessSlider;

    NotifyIcon *_icon;
    std::vector<HICON> _iconImages;
    int _lastIcon;

    HMENU _menu;
    UINT _menuFlags;

    SoundPlayer *_soundPlayer;

    virtual void MeterChangeCallback(int units);
    void MeterLevels(float value);
    void ProcessBrightnessHotkeys(HotkeyInfo &hki);
    void UpdateIcon();
    void UpdateIconTip();
    void UpdateIconImage();

    void UpdateBrightnessState();

    virtual void OnDisplayChange();
    void OnMenuEvent(WPARAM wParam);
    void OnNotifyIconEvent(HWND hWnd, LPARAM lParam);
    void OnBrightnessChange(HWND hWnd, WPARAM wParam, LPARAM lParam);

    virtual LRESULT WndProc(HWND hWnd, UINT message,
        WPARAM wParam, LPARAM lParam);

private:
    std::wstring _menuSetStr = L"Settings";
    std::wstring _menuDisplayStr = L"Display settings";
    std::wstring _menuTurnStr = L"Turn off display";
    std::wstring _menuExitStr = L"Exit 3RVX";

    static const int MENU_SETTINGS = 0;
    static const int MENU_DISPLAY = 1;
    static const int MENU_TURNOFF = 2;
    static const int MENU_EXIT = 3;
};