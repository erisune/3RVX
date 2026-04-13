#include "HideWindowsOSD.h"

static HWND hOSD = NULL;

void HideWindowsOSD::Init() {
    if (hOSD != NULL) {
        HideOSD();
    }

    if ((hOSD = FindOSDWindow()) != NULL) {
        CLOG(L"OSD window handle found");
        HideOSD();
    }
}

HWND HideWindowsOSD::FindOSDWindow() {
    HWND hFound = NULL;
    HWND hParent = NULL;
    bool bMatched = false;

    if (IsWindows1122H2OrGreater()) {
        CLOG(L"RtlGetVersion reports Windows 11 Build 22621 or greater");
        HWND hChild = NULL;
        while ((hParent = FindWindowEx(NULL, hParent, L"XamlExplorerHostIslandWindow", L"")) != NULL)
        {
            if ((hChild = FindWindowEx(hParent, NULL, L"Windows.UI.Composition.DesktopWindowContentBridge", L"DesktopWindowXamlSource")) != NULL)
            {
                if (FindWindowEx(hChild, NULL, L"Windows.UI.Input.InputSite.WindowClass", L"") != NULL)
                {
                    if (!bMatched) {
                        hFound = hParent;
                        bMatched = true;
                    } else {
                        CLOG(L"Found more than one match");
                        return NULL;
                    }
                }
            }
        }
    }
    else {
        while ((hParent = FindWindowEx(NULL, hParent, L"NativeHWNDHost", L"")) != NULL)
        {
            if (FindWindowEx(hParent, NULL, L"DirectUIHWND", L"") != NULL)
            {
                if (!bMatched) {
                    hFound = hParent;
                    bMatched = true;
                } else {
                    CLOG(L"Found more than one match");
                    return NULL;
                }
            }
        }
    }

    if (hFound == NULL) {
        CLOG(L"OSD window handle not found");
    }

    return hFound;
}

bool HideWindowsOSD::IsWindows1122H2OrGreater() {
    HMODULE hModule = GetModuleHandle(L"ntdll.dll");
    if (hModule) {
        RtlGetVersionPtr pRtlGetVersion;
        pRtlGetVersion = (RtlGetVersionPtr)GetProcAddress(hModule, "RtlGetVersion");
        if (pRtlGetVersion != nullptr) {
            RTL_OSVERSIONINFOW vi = {};
            vi.dwOSVersionInfoSize = sizeof(vi);
            if (pRtlGetVersion(&vi) == STATUS_SUCCESS) {
                return vi.dwBuildNumber >= 22621;
            }
        }
    }
    return false;
}

void HideWindowsOSD::HideOSD() {
    ShowWindow(hOSD, SW_MINIMIZE);
}

void HideWindowsOSD::ShowOSD() {
    ShowWindow(hOSD, SW_RESTORE);
}
