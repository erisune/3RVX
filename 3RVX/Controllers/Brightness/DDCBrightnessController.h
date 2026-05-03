// Copyright (c) 2015, Matthew Malensek.
// Copyright (c) 2026, erisune.
// Distributed under the GPL-3.0 License (see COPYING for details)

#pragma once

#pragma comment(lib, "Dxva2.lib") 

#include <Windows.h>
#include <HighLevelMonitorConfigurationAPI.h>
#include "BrightnessController.h"

class Monitor;

class DDCBrightnessController : public BrightnessController {
public:
    DDCBrightnessController(HWND hWnd, HMONITOR monitor);
    DDCBrightnessController(HWND hWnd, Monitor &monitor);
    ~DDCBrightnessController();

    float Brightness() override;
    void Brightness(float level) override;
    bool SupportsBrightnessAPI() override;

private:
    HWND _notifyHwnd;
    HANDLE _monitorHandle;
    DWORD _minBrightness;
    DWORD _maxBrightness;
    DWORD _curBrightness;
    bool _useBrightnessAPI;

    void InitializeBrightnessValues();
    bool SupportsBrightnessAPI(PHYSICAL_MONITOR &pm);
};