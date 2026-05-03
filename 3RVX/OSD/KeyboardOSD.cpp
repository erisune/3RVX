// Copyright (c) 2015, Matthew Malensek.
// Copyright (c) 2026, erisune.
// Distributed under the GPL-3.0 License (see COPYING for details)

#include "KeyboardOSD.h"
#include "../HotkeyInfo.h"
#include "../NotifyIcon.h"
#include "../Skin/OSDComponent.h"
#include "../Skin/Skin.h"
#include "../Skin/SkinManager.h"

KeyboardOSD::KeyboardOSD() :
OSD(L"3RVX-KeyDispatcher"),
_mWnd(L"3RVX-KeyboardOSD", L"3RVX Keyboard OSD") {
    RAWINPUTDEVICE rin[1] = { 0 };
    rin[0].dwFlags = RIDEV_INPUTSINK | RIDEV_NOLEGACY;
    rin[0].hwndTarget = this->Handle();
    rin[0].usUsage = 6;
    rin[0].usUsagePage = 1;
    BOOL result = RegisterRawInputDevices(&rin[0], 1, sizeof(RAWINPUTDEVICE));
    if (result == FALSE) {
        Logger::LogLastError();
    }

    SkinManager *skin = SkinManager::Instance();
    OSDComponent *keyboardOSD = skin->KeyboardOSD();
    if (keyboardOSD != nullptr) {
        _mWnd.BackgroundImage(keyboardOSD->background);
        _mWnd.EnableGlass(keyboardOSD->mask);
        _mWnd.Update();
    }

    OSD::InitMeterWnd(_mWnd);

    /* Check initial states */
    (GetKeyState(VK_CAPITAL) & 0x1) != 0 ? ShowIcon(CapsLock) : HideIcon(CapsLock);
    (GetKeyState(VK_NUMLOCK) & 0x1) != 0 ? ShowIcon(NumLock) : HideIcon(NumLock);
    (GetKeyState(VK_SCROLL) & 0x1) != 0 ? ShowIcon(ScrollLock) : HideIcon(ScrollLock);
}

KeyboardOSD::~KeyboardOSD() {
    HideIcon();
}

void KeyboardOSD::Hide() {
    _mWnd.Hide(false);
}

void KeyboardOSD::HideIcon() {
    HideIcon(KeyType::CapsLock);
    HideIcon(KeyType::NumLock);
    HideIcon(KeyType::ScrollLock);
}

void KeyboardOSD::HideIcon(KeyType key) {
    switch (key) {
    case KeyType::CapsLock:
        if (_capsIcon != NULL) {
            delete _capsIcon;
            _capsIcon = NULL;
        }
        break;
    case KeyType::NumLock:
        if (_numIcon != NULL) {
            delete _numIcon;
            _numIcon = NULL;
        }
        break;
    case KeyType::ScrollLock:
        if (_scrollIcon != NULL) {
            delete _scrollIcon;
            _scrollIcon = NULL;
        }
        break;
    default:
        break;
    }
}

void KeyboardOSD::Show(KeyType key) {
    if (OSD::Enabled() == false) {
        return;
    }
    switch (key) {
    case KeyType::CapsLock:
    case KeyType::CapsUnlock:
        if (!_settings->CapsLockEnabled()) {
            return;
        }
        break;
    case KeyType::NumLock:
    case KeyType::NumUnlock:
        if (!_settings->NumLockEnabled()) {
            return;
        }
        break;
    case KeyType::ScrollLock:
    case KeyType::ScrollUnlock:
        if (!_settings->ScrollLockEnabled()) {
            return;
        }
        break;
    case KeyType::PlayPause:
    case KeyType::Play:
    case KeyType::Pause:
    case KeyType::Stop:
    case KeyType::Next:
    case KeyType::Previous:
        if (!_settings->MediaKeysEnabled()) {
            return;
        }
        break;
    case KeyType::Run:
        if (!_settings->ShowRunHotkey()) {
            return;
        }
        break;
    }
    UpdateMeters(key);
    HideOthers(Keyboard);
    _mWnd.Show();
}

void KeyboardOSD::ShowIcon() {
    ShowIcon(KeyType::CapsLock);
    ShowIcon(KeyType::NumLock);
    ShowIcon(KeyType::ScrollLock);
}

void KeyboardOSD::ShowIcon(KeyType key) {
    if (_settings->KeyboardIconsEnabled()) {
        SkinManager* skin = SkinManager::Instance();
        switch (key) {
        case KeyType::CapsLock:
            if (_settings->CapsLockEnabled()) {
                if (_capsIcon == NULL) {
                    _capsIconImage = skin->CapsLockIcon();
                    if (_capsIconImage != nullptr) {
                        _capsIcon = new NotifyIcon(Window::Handle(), L"Caps Lock", _capsIconImage);
                    }
                }
            }
            break;
        case KeyType::NumLock:
            if (_settings->NumLockEnabled()) {
                if (_numIcon == NULL) {
                    _numIconImage = skin->NumLockIcon();
                    if (_numIconImage != nullptr) {
                        _numIcon = new NotifyIcon(Window::Handle(), L"Num Lock", _numIconImage);
                    }
                }
            }
            break;
        case KeyType::ScrollLock:
            if (_settings->ScrollLockEnabled()) {
                if (_scrollIcon == NULL) {
                    _scrollIconImage = skin->ScrollLockIcon();
                    if (_scrollIconImage != nullptr) {
                        _scrollIcon = new NotifyIcon(Window::Handle(), L"Scroll Lock", _scrollIconImage);
                    }
                }
            }
            break;
        default:
            break;
        }
    }
}

void KeyboardOSD::ProcessHotkeys(HotkeyInfo &hki) {
    /* The keyboard OSD does not support any hotkeys. */
    /* Now it does */
    switch (hki.action) {
    case HotkeyInfo::Run:
        Show(KeyType::Run);
        break;
    }
}

void KeyboardOSD::UpdateMeters(KeyType key) {
    SkinManager *skin = SkinManager::Instance();
    std::vector<MeterComponent*> keyboardSet = skin->KeyboardSet();
    MeterComponent *keyComponent = keyboardSet.at(key);
    if (keyComponent != nullptr){
        if (!keyComponent->meters.empty()) {
            std::list<Meter*> meters(keyComponent->meters.begin(), keyComponent->meters.end());
            _mWnd.SetMeters(meters);
            _mWnd.Update(true);
        }
    }
}

void KeyboardOSD::OnDisplayChange() {
    InitMeterWnd(_mWnd);
}

void KeyboardOSD::OnKeyboardInput(WPARAM wParam, LPARAM lParam) {
    HRAWINPUT hri = (HRAWINPUT) lParam;
    UINT pcbSz;

    /* Determine the size of the RAWINPUT structure */
    GetRawInputData((HRAWINPUT) lParam, RID_INPUT,
        NULL, &pcbSz, sizeof(RAWINPUTHEADER));
    LPBYTE lpb = new BYTE[pcbSz];

    /* Retrieve raw input data */
    GetRawInputData((HRAWINPUT) lParam, RID_INPUT,
        lpb, &pcbSz, sizeof(RAWINPUTHEADER));
    RAWINPUT *raw = (RAWINPUT *) lpb;

    USHORT vk = raw->data.keyboard.VKey;
    USHORT flags = raw->data.keyboard.Flags;
    bool locked = ((GetKeyState(vk) & 0x1) != 0);
    if (flags & 0x1) {
        switch (vk) {
        case VK_CAPITAL:
            CLOG(L"Caps Lock state: %s", locked ? L"ON" : L"OFF");
            if (locked) {
                Show(KeyType::CapsLock);
                ShowIcon(KeyType::CapsLock);
            } else {
                Show(KeyType::CapsUnlock);
                HideIcon(KeyType::CapsLock);
            }
            break;

        case VK_NUMLOCK:
		    CLOG(L"Num Lock state: %s", locked ? L"ON" : L"OFF");
            if (locked) {
                Show(KeyType::NumLock);
                ShowIcon(KeyType::NumLock);
            } else {
                Show(KeyType::NumUnlock);
                HideIcon(KeyType::NumLock);
            }
            break;

        case VK_SCROLL:
            CLOG(L"Scroll Lock state: %s", locked ? L"ON" : L"OFF");
            if (locked) {
                Show(KeyType::ScrollLock);
                ShowIcon(KeyType::ScrollLock);
            } else {
                Show(KeyType::ScrollUnlock);
                HideIcon(KeyType::ScrollLock);
            }
            break;

        case VK_MEDIA_PLAY_PAUSE:
            CLOG(L"Media key: Play/Pause");
            Show(KeyType::PlayPause);
            break;

        case VK_MEDIA_STOP:
            CLOG(L"Media key: Stop");
            Show(KeyType::Stop);
            break;

        case VK_MEDIA_NEXT_TRACK:
            CLOG(L"Media key: Next track");
            Show(KeyType::Next);
            break;

        case VK_MEDIA_PREV_TRACK:
            CLOG(L"Media key: Previous track");
            Show(KeyType::Previous);
            break;
        }
    }
    delete[] lpb;
}

LRESULT
KeyboardOSD::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_INPUT:
        OnKeyboardInput(wParam, lParam);
        break;
    }

    return OSD::WndProc(hWnd, message, wParam, lParam);
}
