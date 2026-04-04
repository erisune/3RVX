#pragma once

#include <Windows.h>
#include "Logger.h"

#define STATUS_SUCCESS 0x00000000

class HideWindowsOSD {
public:
    static void Init();
    static void HideOSD();
    static void ShowOSD();

private:
    static HWND FindOSDWindow();
    static bool IsWindows1122H2OrGreater();
    typedef NTSTATUS(WINAPI* RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);
};
