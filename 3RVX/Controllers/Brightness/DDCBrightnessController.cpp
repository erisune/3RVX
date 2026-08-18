// Copyright (c) 2015, Matthew Malensek.
// Copyright (c) 2026, erisune.
// Distributed under the GPL-3.0 License (see COPYING for details)

#include "DDCBrightnessController.h"

#include "../../Monitor.h"
#include "../../Logger.h"

DDCBrightnessController::DDCBrightnessController(HWND hWnd) {
    DetectCapableMonitors();

    if (_useBrightnessAPI) {
        _notifyHwnd = hWnd;
    }
}

DDCBrightnessController::DDCBrightnessController(HWND hWnd, Monitor &monitor) :
DDCBrightnessController(hWnd) {
    /* TODO: Rework this for specific monitors */
}

DDCBrightnessController::~DDCBrightnessController() {
    for (CapableMonitor monitor : _capableMonitors) {
        DestroyPhysicalMonitor(monitor.hPhysicalMonitor);
    }
    _capableMonitors.clear();
}

void DDCBrightnessController::DetectCapableMonitors() {
    BOOL result = FALSE;
    /* Assuming that the monitor map is already initialized by now */
    for (std::pair<const std::wstring, Monitor> &pair : DisplayManager::MonitorMap()) {
        Monitor monitor = pair.second;
        DWORD numPhysicalMonitors = 0;
        result = GetNumberOfPhysicalMonitorsFromHMONITOR(
            monitor.Handle(), &numPhysicalMonitors);

        if (result == FALSE || numPhysicalMonitors <= 0) {
            CLOG(L"Could not get physical monitors");
            continue; /* Next! */
        }

        CLOG(L"Number of physical monitors detected: %d", numPhysicalMonitors);
        PHYSICAL_MONITOR *monitors = new PHYSICAL_MONITOR[numPhysicalMonitors];
        result = GetPhysicalMonitorsFromHMONITOR(
            monitor.Handle(), numPhysicalMonitors, monitors);
        for (unsigned int i = 0; i < numPhysicalMonitors; ++i) {
            CLOG(L"Monitor: %s", monitors[i].szPhysicalMonitorDescription);
            bool bUseBrightnessAPI = SupportsBrightnessAPI(monitors[i]);
            QCLOG(L"Supports *MonitorBrightness APIs: %s",
                bUseBrightnessAPI ? L"YES" : L"NO");
            if (bUseBrightnessAPI) {
                CapableMonitor mon = { 0 };
                /* Before, we used the first compatible monitor found. */
                /* And now, we use every compatible monitor handle available */
                mon.hPhysicalMonitor = monitors[i].hPhysicalMonitor;
                InitializeBrightnessValues(mon);
                _capableMonitors.push_back(mon);
                break;
            }
        }
        delete[] monitors;
    }

    _useBrightnessAPI = !_capableMonitors.empty();

    ReadMinimumBrightnessFromCapableMonitors();
}

void DDCBrightnessController::InitializeBrightnessValues(CapableMonitor &monitor) {
    DWORD dwMin, dwCur, dwMax;
    BOOL result = GetMonitorBrightness(monitor.hPhysicalMonitor, &dwMin, &dwCur, &dwMax);
    if (result == FALSE) {
        Logger::LogLastError();
    }
    /* I'm using a variable to store the current brightness value
     * because GetMonitorBrightness is a little slow. */
    monitor.dwMinBrightness = dwMin;
    monitor.dwCurBrightness = dwCur;
    monitor.dwMaxBrightness = dwMax;
    CLOG(L"Got brightness: [%d, %d] %d", dwMin, dwMax, dwCur);
}

void DDCBrightnessController::ReadMinimumBrightnessFromCapableMonitors() {
    /* Should only be called when initializing the controller */
    float fBrightness = 1.0f;
    for (CapableMonitor &m : _capableMonitors) {
        float fMonitorBrightness = static_cast<float>((m.dwCurBrightness - m.dwMinBrightness)) / (m.dwMaxBrightness - m.dwMinBrightness);
        fBrightness = min(fBrightness, fMonitorBrightness);
    }
    _brightness = fBrightness;
}

float DDCBrightnessController::Brightness() {
    return _brightness;
}

void DDCBrightnessController::Brightness(float level) {
    if (level > 1.0f) {
        level = 1.0f;
    }
    else if (level < 0.0f) {
        level = 0.0f;
    }

    if (_capableMonitors.empty()) {
        DetectCapableMonitors();
    }

    if (level == Brightness()) {
        /* No change, don't send DDC command. */
        return;
    }

    BOOL result = false;
    for (CapableMonitor &m : _capableMonitors) {
        DWORD setLevel = static_cast<DWORD>(round((m.dwMaxBrightness - m.dwMinBrightness) * level));
        CLOG("Setting brightness level to %d", setLevel);
        result = SetMonitorBrightness(m.hPhysicalMonitor, setLevel);
    }
    if (result) {
        _brightness = level;
        PostMessage(_notifyHwnd, MSG_BRI_CHNG, static_cast<WPARAM>(1), 0);
    }
}

bool DDCBrightnessController::SupportsBrightnessAPI() {
    return _useBrightnessAPI;
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
