// Copyright (c) 2015, Matthew Malensek.
// Distributed under the BSD 2-Clause License (see LICENSE.txt for details)

#include "KeyboardOSD.h"
#include "../NotifyIcon.h"
#include "../Skin/OSDComponent.h"
#include "../Skin/Skin.h"
#include "../Skin/SkinManager.h"

KeyboardOSD::KeyboardOSD() :
OSD(L"3RVX-KeyOSDDispatcher"),
_mWnd(L"3RVX-KeyboardOSD", L"3RVX-KeyboardOSD") {
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
        for (Meter* m : keyboardOSD->meters) {
            _mWnd.AddMeter(m);
        }
        _mWnd.Update();
    }

    OSD::InitMeterWnd(_mWnd);

    /* Check initial states */
    (GetKeyState(VK_CAPITAL) & 0x1) != 0 ? ShowIcon(CapsLock) : HideIcon(CapsLock);
    (GetKeyState(VK_NUMLOCK) & 0x1) != 0 ? ShowIcon(NumLock) : HideIcon(NumLock);
    (GetKeyState(VK_SCROLL) & 0x1) != 0 ? ShowIcon(ScrollLock) : HideIcon(ScrollLock);
}

KeyboardOSD::~KeyboardOSD() {

}

void KeyboardOSD::Hide() {

}

void KeyboardOSD::HideIcon() {
    delete _capsIcon;
    delete _numIcon;
    delete _scrollIcon;
}

void KeyboardOSD::HideIcon(LockKey key) {
    switch (key) {
    case CapsLock:
        delete _capsIcon;
        break;
    case NumLock:
        delete _numIcon;
        break;
    case ScrollLock:
        delete _scrollIcon;
        break;
    default:
        break;
    }
}

void KeyboardOSD::ShowIcon(LockKey key) {
    if (_settings->KeyboardIconsEnabled()) {
        SkinManager* skin = SkinManager::Instance();
        switch (key) {
        case CapsLock:
            if (_settings->CapsLockEnabled()) {
                _capsIconImage = skin->CapsLockIcon();
                if (_capsIconImage != nullptr) {
                    _capsIcon = new NotifyIcon(Window::Handle(), L"Caps Lock", _capsIconImage);
                }
            }
            break;
        case NumLock:
            if (_settings->NumLockEnabled()) {
                _numIconImage = skin->NumLockIcon();
                if (_numIconImage != nullptr) {
                    _numIcon = new NotifyIcon(Window::Handle(), L"Num Lock", _numIconImage);
                }
            }
            break;
        case ScrollLock:
            if (_settings->ScrollLockEnabled()) {
                _scrollIconImage = skin->ScrollLockIcon();
                if (_scrollIconImage != nullptr) {
                    _scrollIcon = new NotifyIcon(Window::Handle(), L"Scroll Lock", _scrollIconImage);
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
    return;
}

void KeyboardOSD::OnDisplayChange() {
    InitMeterWnd(_mWnd);
}

LRESULT
KeyboardOSD::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_INPUT: {
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
        if (vk == VK_CAPITAL && (flags & 0x1)) {
            bool locked = ((GetKeyState(VK_CAPITAL) & 0x1) != 0);
            CLOG(L"Caps Lock state: %s", locked ? L"ON" : L"OFF");
            if (locked) {
                ShowIcon(CapsLock);
            } else {
                HideIcon(CapsLock);
            }
        }

		if (vk == VK_NUMLOCK && (flags & 0x1)) {
			bool locked = ((GetKeyState(VK_NUMLOCK) & 0x1) != 0);
			CLOG(L"Num Lock state: %s", locked ? L"ON" : L"OFF");
            if (locked) {
                ShowIcon(NumLock);
            } else {
                HideIcon(NumLock);
            }
		}

        if (vk == VK_SCROLL && (flags & 0x1)) {
            bool locked = ((GetKeyState(VK_SCROLL) & 0x1) != 0);
            CLOG(L"Scroll Lock state: %s", locked ? L"ON" : L"OFF");
            if (locked) {
                ShowIcon(ScrollLock);
            } else {
                HideIcon(ScrollLock);
            }
        }

        delete[] lpb;
        break;
    }
    }

    return OSD::WndProc(hWnd, message, wParam, lParam);
}
