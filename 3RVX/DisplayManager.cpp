// Copyright (c) 2015, Matthew Malensek.
// Copyright (c) 2026, erisune.
// Distributed under the GPL-3.0 License (see COPYING for details)

#include "DisplayManager.h"

#include <d3d9.h>
#pragma comment(lib, "d3d9.lib")
#include "Logger.h"

static std::unordered_map<std::wstring, Monitor> monitorMap;

std::unordered_map<std::wstring, Monitor> &DisplayManager::MonitorMap() {
    return monitorMap;
}

void DisplayManager::UpdateMonitorMap() {
    monitorMap.clear();
    EnumDisplayMonitors(NULL, NULL, &MonitorEnumProc, NULL);
}

Monitor DisplayManager::Primary() {
    /* The Primary or 'Main' monitor is at (0, 0). */
    const POINT p = { 0, 0 };
    HMONITOR monitor = MonitorFromPoint(p, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mInfo = Info(monitor);
    DEVICEINFO dInfo = DeviceInfo(monitor);
    return Monitor(monitor, L"Primary", dInfo.friendlyName, dInfo.devicePath, mInfo.rcMonitor);
}

Monitor DisplayManager::MonitorAtPoint(POINT &pt, bool workingArea) {
    Monitor m;
    HMONITOR monitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONULL);
    if (monitor != NULL) {
        MONITORINFO mInfo = Info(monitor);
        DEVICEINFO dInfo = DeviceInfo(monitor);
        if (workingArea) {
            return Monitor(monitor, L"Monitor@Point", dInfo.friendlyName, dInfo.devicePath, mInfo.rcWork);
        } else {
            return Monitor(monitor, L"Monitor@Point", dInfo.friendlyName, dInfo.devicePath, mInfo.rcMonitor);
        }
    }

    return m;
}

Monitor DisplayManager::MonitorAtWindow(HWND hWnd, bool workingArea) {
    Monitor m;
    HMONITOR monitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONULL);
    if (monitor != NULL) {
        MONITORINFO mInfo = Info(monitor);
        DEVICEINFO dInfo = DeviceInfo(monitor);
        if (workingArea) {
            return Monitor(monitor, L"Monitor@Window", dInfo.friendlyName, dInfo.devicePath, mInfo.rcWork);
        } else {
            return Monitor(monitor, L"Monitor@Window", dInfo.friendlyName, dInfo.devicePath, mInfo.rcMonitor);
        }
    }

    return m;
}

const int DisplayManager::Width(HMONITOR monitor) {
    MONITORINFO mInfo = Info(monitor);
    RECT mDims = mInfo.rcMonitor;
    return mDims.right - mDims.left;
}

const int DisplayManager::Height(HMONITOR monitor) {
    MONITORINFO mInfo = Info(monitor);
    RECT mDims = mInfo.rcMonitor;
    return mDims.bottom - mDims.top;
}

RECT DisplayManager::Rect(HMONITOR monitor) {
    return Info(monitor).rcMonitor;
}

bool DisplayManager::IsFullscreen(HWND hWnd) {
    HWND fg = GetForegroundWindow();
    if (hWnd == NULL || fg == NULL) {
        return false;
    }

    HWND shell = GetShellWindow();
    if (fg == shell) {
        return false;
    }

    HWND dt = GetDesktopWindow();
    if (fg == dt) {
        return false;
    }

    RECT wndRect = { 0 };
    GetWindowRect(fg, &wndRect);
    Monitor wm = MonitorAtWindow(hWnd);
    if ((wndRect.bottom - wndRect.top) == wm.Height() &&
            (wndRect.right - wndRect.left) == wm.Width()) {
        return true;
    }
    return false;
}

bool DisplayManager::Direct3DOccluded(HWND hWnd) {
    IDirect3D9Ex *pDirect3DEx;
    LPDIRECT3DDEVICE9EX pDeviceEx;
    DWORD behaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
    D3DPRESENT_PARAMETERS d3dpp = { 0 };
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;

    Direct3DCreate9Ex(D3D_SDK_VERSION, &pDirect3DEx);
    HRESULT hr = pDirect3DEx->CreateDeviceEx(
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        hWnd,
        behaviorFlags,
        &d3dpp,
        NULL,
        &pDeviceEx);

    hr = pDeviceEx->CheckDeviceState(NULL);
    bool occluded = (hr == S_PRESENT_OCCLUDED);

    pDeviceEx->Release();
    pDirect3DEx->Release();

    return occluded;
}

MONITORINFO DisplayManager::Info(HMONITOR monitor) {
    MONITORINFO mInfo = {};
    mInfo.cbSize = sizeof(MONITORINFO);
    GetMonitorInfo(monitor, &mInfo);
    return mInfo;
}

DisplayManager::DEVICEINFO DisplayManager::DeviceInfo(HMONITOR hMonitor) {
    DEVICEINFO dInfo = {};
    MONITORINFOEXW mInfo = {};
    mInfo.cbSize = sizeof(mInfo);
    GetMonitorInfo(hMonitor, &mInfo);

    UINT32 nPaths;
    UINT32 nModes;
    GetDisplayConfigBufferSizes(QDC_ONLY_ACTIVE_PATHS, &nPaths, &nModes);
    std::vector<DISPLAYCONFIG_PATH_INFO> paths(nPaths);
    std::vector<DISPLAYCONFIG_MODE_INFO> modes(nModes);
    QueryDisplayConfig(QDC_ONLY_ACTIVE_PATHS, &nPaths, paths.data(), &nModes, modes.data(), NULL);

    for (DISPLAYCONFIG_PATH_INFO &path : paths) {
        DISPLAYCONFIG_SOURCE_DEVICE_NAME dcSource = {};
        dcSource.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_SOURCE_NAME;
        dcSource.header.size = sizeof(DISPLAYCONFIG_SOURCE_DEVICE_NAME);
        dcSource.header.adapterId = path.sourceInfo.adapterId;
        dcSource.header.id = path.sourceInfo.id;

        if (DisplayConfigGetDeviceInfo(&dcSource.header) == ERROR_SUCCESS) {
            if (wcscmp(mInfo.szDevice, dcSource.viewGdiDeviceName) == 0) {
                DISPLAYCONFIG_TARGET_DEVICE_NAME dcTarget = {};
                dcTarget.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_NAME;
                dcTarget.header.size = sizeof(DISPLAYCONFIG_TARGET_DEVICE_NAME);
                dcTarget.header.adapterId = path.sourceInfo.adapterId;
                dcTarget.header.id = path.targetInfo.id;

                if (DisplayConfigGetDeviceInfo(&dcTarget.header) == ERROR_SUCCESS) {
                    dInfo.devicePath = dcTarget.monitorDevicePath;
                    std::wstring friendlyName = dcTarget.monitorFriendlyDeviceName;
                    if (!friendlyName.empty()) {
                        std::wstring deviceName = mInfo.szDevice;
                        dInfo.friendlyName = friendlyName + L" (#" + deviceName.substr(11, -1) + L")";
                        return dInfo;
                    }
                }
            }
        }
    }

    /* Fallback, get generic name */
    DISPLAY_DEVICE dd = {};
    dd.cb = sizeof(DISPLAY_DEVICE);
    if (EnumDisplayDevices(mInfo.szDevice, 0, &dd, NULL)) {
        std::wstring deviceString = dd.DeviceString;
        std::wstring deviceName = mInfo.szDevice;
        dInfo.friendlyName = deviceString + L" (#" + deviceName.substr(11, -1) + L")";
        return dInfo;
    }

    dInfo.friendlyName = mInfo.szDevice;
    return dInfo;
}

std::list<DISPLAY_DEVICE> DisplayManager::ListAllDevices() {
    std::list<DISPLAY_DEVICE> devs;
    DISPLAY_DEVICE dev = {};
    dev.cb = sizeof(DISPLAY_DEVICE);
    for (int i = 0; EnumDisplayDevices(NULL, i, &dev, NULL) != 0; ++i) {
        if (dev.StateFlags & DISPLAY_DEVICE_ACTIVE) {
            devs.push_back(dev);
        }
    }
    return devs;
}

BOOL CALLBACK DisplayManager::MonitorEnumProc(
    HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData) {

    MONITORINFOEX mInfo = {};
    mInfo.cbSize = sizeof(MONITORINFOEX);
    GetMonitorInfo(hMonitor, &mInfo);

    std::wstring monitorName = std::wstring(mInfo.szDevice);
    DEVICEINFO dInfo = DeviceInfo(hMonitor);
    Monitor mon(hMonitor, monitorName, dInfo.friendlyName, dInfo.devicePath, mInfo.rcMonitor);
    monitorMap[monitorName] = mon;

    return TRUE;
}