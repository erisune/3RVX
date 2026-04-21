// Copyright (c) 2015, Matthew Malensek.
// Distributed under the BSD 2-Clause License (see LICENSE.txt for details)

#include "DDCBrightnessController.h"

#include "../../Monitor.h"
#include "../../Logger.h"

DDCBrightnessController::DDCBrightnessController(HMONITOR monitor) {
    BOOL result;
    DWORD numPhysicalMonitors = 0;
    result = GetNumberOfPhysicalMonitorsFromHMONITOR(
        monitor, &numPhysicalMonitors);

    if (result == FALSE || numPhysicalMonitors <= 0) {
        CLOG(L"Could not get physical monitors");
        return;
    }

    CLOG(L"Number of physical monitors detected: %d", numPhysicalMonitors);
    PHYSICAL_MONITOR *monitors = new PHYSICAL_MONITOR[numPhysicalMonitors];
    result = GetPhysicalMonitorsFromHMONITOR(
        monitor, numPhysicalMonitors, monitors);
    for (unsigned int i = 0; i < numPhysicalMonitors; ++i) {
        CLOG(L"Monitor: %s", monitors[i].szPhysicalMonitorDescription);
        bool supportsAPI = SupportsBrightnessAPI(monitors[i]);
        QCLOG(L"Supports *MonitorBrightness APIs: %s",
            supportsAPI ? L"YES" : L"NO");
        if (supportsAPI) {
            /* For now, we use the first compatible monitor found. */
            _monitorHandle = monitors[i].hPhysicalMonitor;
            break;
        }
    }
    delete[] monitors;

    InitializeBrightnessValues();
}

DDCBrightnessController::DDCBrightnessController(Monitor &monitor) :
DDCBrightnessController(monitor.Handle()) {

}

DDCBrightnessController::~DDCBrightnessController() {
    DestroyPhysicalMonitor(_monitorHandle);
}

void DDCBrightnessController::InitializeBrightnessValues() {
    DWORD dwMin, dwCur, dwMax;
    BOOL result = GetMonitorBrightness(_monitorHandle, &dwMin, &dwCur, &dwMax);
    if (result == FALSE) {
        Logger::LogLastError();
    }
    /* I'm using a variable to store the current brightness value
     * because GetMonitorBrightness is a little slow. */
    _minBrightness = dwMin;
    _curBrightness = dwCur;
    _maxBrightness = dwMax;
    CLOG(L"Got brightness: [%d, %d] %f", dwMin, dwMax, dwCur);
}

float DDCBrightnessController::Brightness() {
    return static_cast<float>((_curBrightness - _minBrightness)) / (_maxBrightness - _minBrightness);
}

void DDCBrightnessController::Brightness(float level) {
    if (level > 1.0f) {
        level = 1.0f;
    }
    else if (level < 0.0f) {
        level = 0.0f;
    }

    if (level == Brightness()) {
        /* No change, don't send DDC command. */
        return;
    }

    DWORD setLevel = static_cast<DWORD>(round((_maxBrightness - _minBrightness) * level));
    CLOG("Setting brightness level to %d", setLevel);
    BOOL result = SetMonitorBrightness(_monitorHandle, setLevel);
    if (result) {
        _curBrightness = setLevel;
    }
}

bool DDCBrightnessController::SupportsBrightnessAPI(PHYSICAL_MONITOR &pm) {
    DWORD caps, color;
    BOOL result = GetMonitorCapabilities(pm.hPhysicalMonitor, &caps, &color);
    if (result == FALSE) {
        QCLOG(L"Monitor does not support DDC/CI");
        return false;
    }
    return ((caps & MC_CAPS_BRIGHTNESS) == MC_CAPS_BRIGHTNESS);
}
