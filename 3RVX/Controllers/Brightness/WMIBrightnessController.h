// Copyright (c) 2017, Matthew Malensek.
// Copyright (c) 2026, erisune.
// Distributed under the GPL-3.0 License (see COPYING for details)

#pragma once

#include <Windows.h>
#include <comutil.h>
#include <Wbemidl.h>
#include <vector>

#include "../../COMUtil.h"
#include "../../Logger.h"
#include "BrightnessController.h"

#pragma comment(lib, "wbemuuid.lib")

class WMIBrightnessController : public BrightnessController {
public:
    WMIBrightnessController(HWND hWnd);
    ~WMIBrightnessController();

    float Brightness() override;
    void Brightness(float level) override;
    bool SupportsBrightnessAPI() override;

private:
    HWND _notifyHwnd;
    struct CapableMonitor {
        IWbemClassObject *wbemObject;
        IWbemClassObject *wbemInstance;
    };
    std::vector<CapableMonitor> _instances;
    IWbemServices *_pSvc;
    bool _useBrightnessAPI;
    float _brightness;
};