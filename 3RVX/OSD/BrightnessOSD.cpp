// Copyright (c) 2015, Matthew Malensek.
// Copyright (c) 2026, erisune.
// Distributed under the GPL-3.0 License (see COPYING for details)

#include "BrightnessOSD.h"

#include "../Controllers/Brightness/BrightnessController.h"
#include "../Controllers/Brightness/DDCBrightnessController.h"
#include "../DisplayManager.h"
#include "../LanguageTranslator.h"
#include "../HotkeyInfo.h"
#include "../MeterWnd/Meters/CallbackMeter.h"
#include "../Skin/OSDComponent.h"
#include "../Skin/Skin.h"
#include "../Skin/SkinManager.h"

BrightnessOSD::BrightnessOSD() :
OSD(L"3RVX-BrightnessDispatcher"),
_mWnd(L"3RVX-BrightnessOSD", L"3RVX Brightness OSD") {

    SkinManager *skin = SkinManager::Instance();
    OSDComponent *brightnessOSD = skin->BrightnessOSD();
    if (brightnessOSD == nullptr) {
        _validSkin = false;
    } else {
        _validSkin = true;
        _mWnd.BackgroundImage(brightnessOSD->background);
        _mWnd.EnableGlass(brightnessOSD->mask);

        for (Meter *m : brightnessOSD->meters) {
            _mWnd.AddMeter(m);
        }

        /* Add a callback meter with the default brightness increment */
        _callbackMeter = new CallbackMeter(brightnessOSD->defaultUnits, *this);
        _mWnd.AddMeter(_callbackMeter);

        /* Default brightness increment */
        _defaultIncrement = static_cast<float>(10000 / brightnessOSD->defaultUnits) / 10000.0f;
        CLOG(L"Default brightness increment: %f", _defaultIncrement);

        _mWnd.Update();
    }

    OSD::InitMeterWnd(_mWnd);

    _brightnessCtrl = new DDCBrightnessController(
        Window::Handle(), DisplayManager::Primary().Handle());

    /* Check if brightness API is supported */
    if (!_brightnessCtrl->SupportsBrightnessAPI()) {
        return;
    }

    if (skin->BrightnessSlider() == nullptr) {
        _validSlider = false;
    } else {
        _validSlider = true;
    }

    /* Create the slider */
    _brightnessSlider = new BrightnessSlider(*_brightnessCtrl);

    /* Set up notification icon */
    if (_settings->BrightnessIconEnabled()) {
        _iconImages = skin->BrightnessIconset();
        if (_iconImages.size() > 0) {
            LanguageTranslator *translator = _settings->Translator();
            _brightnessStr = translator->Translate(_brightnessStr);
            _icon = new NotifyIcon(Window::Handle(), _brightnessStr, _iconImages[0]);
        }

        LanguageTranslator *translator = _settings->Translator();
        _menuDisplayStr = translator->Translate(_menuDisplayStr);
        _menuTurnStr = translator->Translate(_menuTurnStr);
        _menuSetStr = translator->Translate(_menuSetStr);
        _menuExitStr = translator->Translate(_menuExitStr);

        _menu = CreatePopupMenu();

        InsertMenu(_menu, -1, MF_ENABLED, MENU_DISPLAY, _menuDisplayStr.c_str());
        InsertMenu(_menu, -1, MF_SEPARATOR, NULL, NULL);
        InsertMenu(_menu, -1, MF_ENABLED, MENU_TURNOFF, _menuTurnStr.c_str());
        InsertMenu(_menu, -1, MF_ENABLED, MENU_SETTINGS, _menuSetStr.c_str());
        InsertMenu(_menu, -1, MF_SEPARATOR, NULL, NULL);
        InsertMenu(_menu, -1, MF_ENABLED, MENU_EXIT, _menuExitStr.c_str());

        /* Menu accepts both left and right clicks on its items: */
        _menuFlags = TPM_RIGHTBUTTON;
        if (GetSystemMetrics(SM_MENUDROPALIGNMENT) != 0) {
            _menuFlags |= TPM_RIGHTALIGN;
        } else {
            _menuFlags |= TPM_LEFTALIGN;
        }
    }

    /* Enable sound effects, if any */
    if (_validSkin) {
        if (_settings->SoundEffectsEnabled()) {
            _soundPlayer = brightnessOSD->sound;
        }
    }

    UpdateBrightnessState();
}

BrightnessOSD::~BrightnessOSD() {
    DestroyMenu(_menu);
    delete _icon;
    delete _brightnessCtrl;
    delete _brightnessSlider;
    delete _callbackMeter;
}

void BrightnessOSD::MeterLevels(float level) {
    if (_validSkin) {
        _mWnd.MeterLevels(level);
        _mWnd.Update();
    }
}

void BrightnessOSD::MeterChangeCallback(int units) {
    /* Callback meter changed by at least 1 unit */
}

void BrightnessOSD::Hide() {
    _mWnd.Hide(false);
}

void BrightnessOSD::Show() {
    if (OSD::Enabled() == false) {
        return;
    }

    _mWnd.Show();
}

void BrightnessOSD::HideIcon() {
    delete _icon;
}

void BrightnessOSD::UpdateIcon() {
    UpdateIconImage();
    UpdateIconTip();
}

void BrightnessOSD::UpdateIconImage() {
    if (_icon == NULL) {
        return;
    }

    int icon = 0;
    size_t vUnits = _iconImages.size() - 1;
    icon = static_cast<int>(round(_brightnessCtrl->Brightness() * vUnits));

    if (icon != _lastIcon) {
        _icon->UpdateIcon(_iconImages[icon]);
        _lastIcon = icon;
    }
}

void BrightnessOSD::UpdateIconTip() {
    if (_icon == NULL) {
        return;
    }

    float b = _brightnessCtrl->Brightness();
    std::wstring perc = std::to_wstring(static_cast<int>(b * 100.0f));
    std::wstring level = _brightnessStr + L": " + perc + L"%";
    _icon->UpdateToolTip(level);
}

void BrightnessOSD::ProcessHotkeys(HotkeyInfo &hki) {
    switch (hki.action) {
	case HotkeyInfo::IncreaseBrightness:
    case HotkeyInfo::DecreaseBrightness:
        ProcessBrightnessHotkeys(hki);
        break;
    case HotkeyInfo::SetBrightness:
    {
        /* I should probably rename the enum */
        HotkeyInfo::VolumeKeyArgTypes type = HotkeyInfo::VolumeArgType(hki);
        if (type == HotkeyInfo::VolumeKeyArgTypes::NoArgs) {
            return;
        } else if (type == HotkeyInfo::VolumeKeyArgTypes::Units) {
            int numUnits = hki.ArgToInt(0);
            _brightnessCtrl->Brightness(numUnits * _defaultIncrement);
        } else if (type == HotkeyInfo::VolumeKeyArgTypes::Percentage) {
            float perc = static_cast<float>(hki.ArgToDouble(0)) / 100.0f;
            _brightnessCtrl->Brightness(perc);
        }
        SendMessage(Window::Handle(), BrightnessController::MSG_BRI_CHNG,
            NULL, static_cast<LPARAM>(1));
        break;
    }
    case HotkeyInfo::BrightnessSlider:
        if (_validSlider) {
            if (_brightnessCtrl->SupportsBrightnessAPI()) {
                _brightnessSlider->MeterLevels(_brightnessCtrl->Brightness());
                if (_brightnessSlider->Active()) {
                    _brightnessSlider->Hide(true);
                } else {
                    SendMessage(Window::Handle(), MSG_NOTIFYICON, NULL, WM_LBUTTONUP);
                }
            }
        }
        break;
    }
}

void BrightnessOSD::ProcessBrightnessHotkeys(HotkeyInfo &hki) {
    float currentBrightness = _brightnessCtrl->Brightness();
    HotkeyInfo::VolumeKeyArgTypes type = HotkeyInfo::VolumeArgType(hki);

    if (type == HotkeyInfo::VolumeKeyArgTypes::Percentage) {
        /* Deal with percentage-based amounts */
        float amount = static_cast<float>(hki.ArgToDouble(0)) / 100.0f;
        if (hki.action == HotkeyInfo::HotkeyActions::DecreaseBrightness) {
            amount = -amount;
        }
        _brightnessCtrl->Brightness(currentBrightness + amount);
    } else {
        /* Unit-based amounts */
        double unitIncrement = 1.0;
        int currentUnit = _callbackMeter->CalcUnits();
        if (currentBrightness <= 0.000001f) {
            currentUnit = 0;
        }

        if (hki.action == HotkeyInfo::DecreaseBrightness) {
            unitIncrement = -1.0;
        }

        if (type == HotkeyInfo::VolumeKeyArgTypes::Units) {
            unitIncrement *= hki.ArgToDouble(0);
        }

        if (unitIncrement - static_cast<int>(unitIncrement) < 0.0001) {
            _brightnessCtrl->Brightness(
                static_cast<float>(currentUnit + unitIncrement) * _defaultIncrement);
        } else {
            _brightnessCtrl->Brightness(_brightnessCtrl->Brightness()
                + (static_cast<float>(unitIncrement * _defaultIncrement)));
        }
    }

    /* Tell 3RVX that we changed the brightness */
    SendMessage(Window::Handle(), BrightnessController::MSG_BRI_CHNG,
        NULL, static_cast<LPARAM>(1));
}

void BrightnessOSD::UpdateBrightnessState() {
    float b = _brightnessCtrl->Brightness();
    MeterLevels(b);
    UpdateIcon();
}

void BrightnessOSD::OnDisplayChange() {
    delete _brightnessCtrl;
    _brightnessCtrl = new DDCBrightnessController(
        Window::Handle(), DisplayManager::Primary().Handle());
    InitMeterWnd(_mWnd);
}

void BrightnessOSD::OnMenuEvent(WPARAM wParam) {
    int menuItem = LOWORD(wParam);
    switch (menuItem) {
    case MENU_SETTINGS:
        Settings::LaunchSettingsApp();
        break;

    case MENU_DISPLAY: {
        CLOG(L"Menu: Display settings");
        HINSTANCE code = ShellExecute(NULL, L"open", L"control",
            L"desk.cpl", NULL, SW_SHOWNORMAL);
        break;
    }

    case MENU_TURNOFF: {
        CLOG(L"Menu: Turn off display");
        SendMessage(_masterWnd, WM_SYSCOMMAND, SC_MONITORPOWER, static_cast<LPARAM>(2));
        break;
    }

    case MENU_EXIT:
        CLOG(L"Menu: Exit: %Ix", reinterpret_cast<DWORD_PTR>(_masterWnd));
        SendMessage(_masterWnd, WM_CLOSE, NULL, NULL);
        break;
    }
}

void BrightnessOSD::OnNotifyIconEvent(HWND hWnd, LPARAM lParam) {
    if (lParam == WM_LBUTTONUP) {
        if (_validSlider) {
            if (_brightnessCtrl->SupportsBrightnessAPI()) {
                _brightnessSlider->MeterLevels(_brightnessCtrl->Brightness());
                if (_brightnessSlider->Active()) {
                    _brightnessSlider->Hide();
                } else {
                    _brightnessSlider->Show();
                }
            }
        }
    } else if (lParam == WM_RBUTTONUP) {
        POINT p;
        GetCursorPos(&p);
        SetForegroundWindow(hWnd);
        TrackPopupMenuEx(_menu, _menuFlags, p.x, p.y,
            Window::Handle(), NULL);
        PostMessage(hWnd, WM_NULL, 0, 0);
    }
}

void BrightnessOSD::OnBrightnessChange(HWND hWnd, WPARAM wParam, LPARAM lParam) {
    float b = _brightnessCtrl->Brightness();

    if (_validSlider) {
        _brightnessSlider->MeterLevels(b);
    }

    UpdateIcon();

    if (!_brightnessSlider->Visible()) {
        MeterLevels(b);
        Show();

        if (_soundPlayer) {
            _soundPlayer->Play();
        }
        HideOthers(Brightness);
    }
}

LRESULT
BrightnessOSD::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case BrightnessController::MSG_BRI_CHNG:
        OnBrightnessChange(hWnd, wParam, lParam);
        break;

    case MSG_NOTIFYICON:
        OnNotifyIconEvent(hWnd, lParam);
        break;

    case WM_COMMAND:
        OnMenuEvent(wParam);
        break;
    }

    return OSD::WndProc(hWnd, message, wParam, lParam);
}

