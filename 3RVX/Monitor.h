// Copyright (c) 2015, Matthew Malensek.
// Copyright (c) 2026, erisune.
// Distributed under the GPL-3.0 License (see COPYING for details)

#pragma once

#include <Windows.h>
#include <list>
#include <string>
#include <unordered_map>

class Monitor {
public:
    Monitor() {

    }

    Monitor(HMONITOR handle, std::wstring name,
        std::wstring friendlyName, std::wstring devicePath,
        int x, int y, int width, int height) :
    _handle(handle),
    _name(name),
    _friendlyName(friendlyName),
    _devicePath(devicePath),
    _x(x),
    _y(y),
    _width(width),
    _height(height) {

    }

    Monitor(HMONITOR handle, std::wstring name,
        std::wstring friendlyName, std::wstring devicePath,
        RECT rect) :
    _handle(handle),
    _name(name),
    _friendlyName(friendlyName),
    _devicePath(devicePath),
    _x(rect.left),
    _y(rect.top),
    _width(rect.right - rect.left),
    _height(rect.bottom - rect.top) {

    }

    int X() {
        return _x;
    }

    int Y() {
        return _y;
    }

    int Width() {
        return _width;
    }

    int Height() {
        return _height;
    }

    HMONITOR Handle() {
        return _handle;
    }

    std::wstring Name() {
        return _name;
    }

    std::wstring FriendlyName() {
        return _friendlyName;
    }

    std::wstring DevicePath() {
        return _devicePath;
    }

private:
    HMONITOR _handle;
    std::wstring _name;
    std::wstring _friendlyName;
    std::wstring _devicePath;
    int _x;
    int _y;
    int _width;
    int _height;
};