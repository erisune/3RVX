// Copyright (c) 2015, Matthew Malensek.
// Copyright (c) 2026, erisune.
// Distributed under the GPL-3.0 License (see COPYING for details)

#pragma once

#pragma comment(lib, "Dxva2.lib") 

#include <Windows.h>
#include <HighLevelMonitorConfigurationAPI.h>
#include "BrightnessController.h"
#include "../../DisplayManager.h"

class Monitor;

class DDCBrightnessController : public BrightnessController {
public:
    DDCBrightnessController(HWND hWnd);
    DDCBrightnessController(HWND hWnd, Monitor &monitor);
    ~DDCBrightnessController();

    float Brightness() override;
    void Brightness(float level) override;
    bool SupportsBrightnessAPI() override;

private:
    HWND _notifyHwnd;
    struct CapableMonitor {
        HANDLE hPhysicalMonitor;
        DWORD dwMinBrightness;
        DWORD dwMaxBrightness;
        DWORD dwCurBrightness;
    };
    std::vector<CapableMonitor> _capableMonitors;
    bool _useBrightnessAPI;
    float _brightness;

    void ReadMinimumBrightnessFromCapableMonitors();
    void DetectCapableMonitors();
    void InitializeBrightnessValues(CapableMonitor &monitor);
    bool SupportsBrightnessAPI(PHYSICAL_MONITOR &pm);
};