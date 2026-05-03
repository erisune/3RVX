// Copyright (c) 2026, erisune.
// Distributed under the GPL-3.0 License (see COPYING for details)

#pragma once

#include <vector>

#include "..\Controllers\Volume\CoreAudio.h"
#include "..\Controllers\Volume\VolumeController.h"
#include "..\MeterWnd\Animations\FadeOut.h"
#include "..\MeterWnd\MeterCallbackReceiver.h"
#include "..\MeterWnd\MeterWnd.h"
#include "..\NotifyIcon.h"
#include "OSD.h"

class CallbackMeter;
class Skin;
class SoundPlayer;

class MicrophoneOSD : public OSD, MeterCallbackReceiver {
public:
    MicrophoneOSD();
    ~MicrophoneOSD();

    void Hide();
    void HideIcon();
    void Show(bool mute = false);

    virtual void ProcessHotkeys(HotkeyInfo& hki);

    enum MicrophoneIcon {
        DisabledIcon,
        MutedIcon,
        EnabledIcon
    };

private:
    CoreAudio* _volumeCtrl;
    float _defaultIncrement;
    float _lastVolume;
    bool _muted;
    bool _monitorSession;
    bool _subscribeVolEvents;
    bool _unlockUnmute;
    bool _validSkin;

    MeterWnd _mWnd;
    CallbackMeter* _callbackMeter;
    MeterWnd _muteWnd;

    NotifyIcon* _icon;
    std::vector<HICON> _iconImages;
    int _lastIcon;

    HMENU _menu;
    HMENU _deviceMenu;
    UINT _menuFlags;
    std::vector<VolumeController::DeviceInfo> _deviceList;
    std::wstring _selectedDevice;
    std::wstring _selectedDesc;

    SoundPlayer* _soundPlayer;

    void LoadSkin();
    void MeterLevels(float value);
    virtual void MeterChangeCallback(int units);
    void ProcessVolumeHotkeys(HotkeyInfo& hki);
    void UpdateIcon();
    void UpdateIconImage();
    void UpdateIconTip();
    void UpdateDeviceMenu();
    void UnMute();

    /// <summary>
    /// Re-initializes volume state variables (used after a device change
    /// occurs).
    /// </summary>
    void UpdateVolumeState();

    void OnDeviceChange();
    virtual void OnDisplayChange();
    void OnMenuEvent(WPARAM wParam);
    void OnNotifyIconEvent(HWND hWnd, LPARAM lParam);
    void OnSessionChange(WPARAM wParam);
    void OnVolumeChange(HWND hWnd, WPARAM wParam, LPARAM lParam);

    virtual LRESULT
        WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
    std::wstring _menuSetStr = L"Settings";
    std::wstring _menuDevStr = L"Selected device";
    std::wstring _menuMixerStr = L"Open Volume Mixer";
    std::wstring _menuRecStr = L"Recording devices";
    std::wstring _menuExitStr = L"Exit 3RVX";

    std::wstring _iconMuteStr = L"Muted";

private:
    static const int MENU_SETTINGS = 0;
    static const int MENU_MMSYS = 1;
    static const int MENU_MIXER = 2;
    static const int MENU_EXIT = 3;
    static const int MENU_DEVICE = 0xF000;

};