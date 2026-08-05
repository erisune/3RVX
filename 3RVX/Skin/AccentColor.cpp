// Copyright (c) 2016, Matthew Malensek.
// Copyright (c) 2026, erisune.
// Distributed under the GPL-3.0 License (see COPYING for details)

#include "AccentColor.h"

#include <dwmapi.h>
#include <VersionHelpers.h>

#include "../Logger.h"

AccentColor *AccentColor::instance = nullptr;

AccentColor::AccentColor() {
    Refresh();
}

AccentColor::~AccentColor() {
    FreeModule(_dwmLib);
}

AccentColor *AccentColor::Instance() {
    if (instance == nullptr) {
        instance = new AccentColor();
    }

    return instance;
}

UINT32 AccentColor::Color() const {
    return _color;
}

void AccentColor::Color(UINT32 color) {
    _color = color;
    _override = true;
}

void AccentColor::UseSystemColor(bool enable) {
    _override = !enable;
    Refresh();
}

void AccentColor::Refresh() {
    if (_override == true) {
        return;
    }

    if (IsWindowsVistaOrGreater()) {
        if (IsWindows7OrGreater() && _useUndocumented == true) {
            INT64 color = ColorizationParamColor();
            if (color >= 0) {
                _color = (UINT32) color;
                return;
            }
            /* If an error occurs (-1), fall through to the next technique. */
        }

        DWORD color;
        BOOL opaque;
        /* pfOpaqueBlend always returns true in modern Windows versions */
        HRESULT hr = DwmGetColorizationColor(&color, &opaque);
        if (SUCCEEDED(hr)) {
            _color = color;
            return;
        }
    }

    /* Our last hope if both methods above failed: */
    COLORREF crCaption = GetSysColor(COLOR_ACTIVECAPTION);
    DWORD caption = 0xFF000000 | GetRValue(crCaption) << 16 | GetGValue(crCaption) << 8 | GetBValue(crCaption) << 0;
    _color = caption;
}

bool AccentColor::LoadColorizationParameter(LPCWSTR lpValueName, DWORD &lpValue) {
    const wchar_t REGKEY_DWM[] = L"Software\\Microsoft\\Windows\\DWM";
    HKEY hKey = NULL;
    LONG result = RegOpenKeyEx(HKEY_CURRENT_USER, REGKEY_DWM, NULL, KEY_READ, &hKey);
    DWORD cbSize = sizeof(DWORD);
    DWORD dwValue = 0;
    if (result == ERROR_SUCCESS) {
        result = RegQueryValueEx(hKey, lpValueName, NULL, NULL,
            reinterpret_cast<LPBYTE>(&dwValue), &cbSize);
        if (result == ERROR_SUCCESS) {
            lpValue = dwValue;
        }
        RegCloseKey(hKey);
    }
    return result;
}

/* WARNING: This method uses an undocumented API. May crash in the future. */
INT64 AccentColor::ColorizationParamColor() {
    struct DwmColorizationParams {
        UINT ColorizationColor;
        UINT ColorizationAfterglow;
        UINT ColorizationColorBalance;
        UINT ColorizationAfterglowBalance;
        UINT ColorizationBlurBalance;
        /* 
         * DwmpReadColorizationParameters doesn't read these
         * in modern Windows versions, so let's not depend on them
         *
         * UINT ColorizationGlassReflectionIntensity;
         * UINT ColorizationOpaqueBlend;
         */
    };

    if (_dwmLib == NULL) {
        /* May be the first time executing this method */
        _dwmLib = LoadLibrary(L"dwmapi.dll");
        if (_dwmLib == NULL) {
            CLOG(L"Could not load DWM library");
            return -1;
        }
    }

    HRESULT(WINAPI *DwmGetColorizationParameters)(DwmColorizationParams *colorParams) = nullptr;
    constexpr unsigned short ColorizationParamOrd = 127;

    *(FARPROC *) &DwmGetColorizationParameters
        = GetProcAddress(_dwmLib, reinterpret_cast<LPCSTR>(ColorizationParamOrd));
    if (DwmGetColorizationParameters == nullptr) {
        CLOG(L"Could not retrieve DwmGetColorizationParameters address");
        return -1;
    }

    DwmColorizationParams params = { 0 };
    HRESULT hr = DwmGetColorizationParameters(&params);
    if (FAILED(hr)) {
        CLOG(L"Could not retrieve DWM colorization parameters");
        return -1;
    }

    /* Load DWM overrides if present (OpenGlass) */
    DWORD dwColorizationColorBalance = params.ColorizationColorBalance;
    DWORD dwColorizationBlurBalance = params.ColorizationBlurBalance;
    DWORD dwColorizationAfterglowBalance = params.ColorizationAfterglowBalance;
    LoadColorizationParameter(L"ColorizationColorBalanceOverride", dwColorizationColorBalance);
    LoadColorizationParameter(L"ColorizationBlurBalanceOverride", dwColorizationBlurBalance);
    LoadColorizationParameter(L"ColorizationAfterglowBalanceOverride", dwColorizationAfterglowBalance);

    /* Check opaque blend */
    DWORD dwOpaqueBlend = 0;
    LoadColorizationParameter(L"ColorizationOpaqueBlend", dwOpaqueBlend);

    /* Convert colorization parameters to ARGB */
    UINT uBalance = 0;
    if (dwOpaqueBlend) {
        uBalance = dwColorizationColorBalance;
    }
    else if (dwColorizationBlurBalance < 50) {
        if (dwColorizationBlurBalance <= 23) {
            uBalance = dwColorizationColorBalance + 25;
        }
        else {
            uBalance = 95 - dwColorizationAfterglowBalance;
        }
    }
    else {
        uBalance = 100 - dwColorizationBlurBalance;
    }

    DWORD dwColor = params.ColorizationColor & 0xFFFFFF |
        (static_cast<int>((static_cast<float>(uBalance - 10) * 0.75 / 100.0 + 0.1) * 255.0 + 0.5) << 24);

    return (INT64) dwColor;
}
