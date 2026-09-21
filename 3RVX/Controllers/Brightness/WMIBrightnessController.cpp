// Copyright (c) 2017, Matthew Malensek.
// Copyright (c) 2026, erisune.
// Distributed under the GPL-3.0 License (see COPYING for details)

#include "WMIBrightnessController.h"

WMIBrightnessController::WMIBrightnessController(HWND hWnd) {
    HRESULT hr;
    _brightness = 1.0f;
    _useBrightnessAPI = false;

    /* Create WMI locator */
    IWbemLocator *pLoc = NULL;
    hr = CoCreateInstance(
        CLSID_WbemLocator,
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_IWbemLocator,
        (LPVOID *) &pLoc);

    if (FAILED(hr)) {
        CLOG(L"Failed to create WMI locator");
        return;
    }

    /* Connect to WMI namespace */
    _pSvc = NULL;
    hr = pLoc->ConnectServer(
        _bstr_t(L"ROOT\\WMI"),
        NULL,
        NULL,
        NULL,
        0,
        NULL,
        NULL,
        &_pSvc);

    if (FAILED(hr)) {
        CLOG(L"Failed to connect to WMI namespace");
        if (pLoc) {
            pLoc->Release();
        }
        return;
    }

    /* Set up proxy auth to impersonate the client */
    hr = CoSetProxyBlanket(
       _pSvc,
       RPC_C_AUTHN_WINNT,
       RPC_C_AUTHZ_NONE,
       NULL,
       RPC_C_AUTHN_LEVEL_PKT,
       RPC_C_IMP_LEVEL_IMPERSONATE,
       NULL,
       EOAC_NONE);

    if (FAILED(hr)) {
        CLOG(L"Failed to set proxy blanket: 0x%x", hr);
        COMUtil::SafeRelease(_pSvc);
        COMUtil::SafeRelease(pLoc);
        return;
    }

    IEnumWbemClassObject* pEnumerator = NULL;
    hr = _pSvc->ExecQuery(
        bstr_t("WQL"),
        bstr_t("SELECT * FROM WmiMonitorBrightness"),
        WBEM_FLAG_RETURN_IMMEDIATELY,
        NULL,
        &pEnumerator);

    IWbemClassObject *pclsObj = NULL;
    IWbemClassObject *pclsMethod = NULL;
    ULONG uReturn = 0;

    while (pEnumerator) {
        hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
        if (uReturn == 0) {
            break;
        }

        // VARIANT vtProp;
        BSTR x;
        pclsObj->GetObjectText(0, &x);
        CLOG(L"x-> %s", x);
        SysFreeString(x);
        // VariantClear(&vtProp);

        CapableMonitor mon = { 0 };
        mon.wbemObject = pclsObj;

        if (hr == WBEM_S_NO_ERROR) {
            VARIANT vtCurrent;
            VariantInit(&vtCurrent);
            pclsObj->Get(L"CurrentBrightness",
                0,
                &vtCurrent,
                NULL,
                NULL);
            CLOG(L"CurrentBrightness from object: %d", vtCurrent.bVal);
            _brightness = min(_brightness, vtCurrent.bVal / 100.f);
            _useBrightnessAPI = true;
            VariantClear(&vtCurrent);
        }
    }

    hr = _pSvc->ExecQuery(
        bstr_t("WQL"),
        bstr_t("SELECT * FROM WmiMonitorBrightnessMethods"),
        WBEM_FLAG_RETURN_IMMEDIATELY,
        NULL,
        &pEnumerator);

    while (pEnumerator) {
        hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
        if (uReturn == 0) {
            break;
        }

        BSTR x;
        pclsObj->GetObjectText(0, &x);
        CLOG(L"x-> %s", x);
        SysFreeString(x);

        CapableMonitor mon = { 0 };
        mon.wbemObject = pclsObj;

        /* WmiSetBrightness stuff */
        hr = _pSvc->GetObject(
            bstr_t("WmiMonitorBrightnessMethods"),
            0,
            NULL,
            &pclsMethod,
            NULL);

        if (hr == WBEM_S_NO_ERROR) {
            CLOG(L"WmiMonitorBrightnessMethods: OK");

            IWbemClassObject *pclsParams = NULL;
            hr = pclsMethod->GetMethod(
                bstr_t("WmiSetBrightness"),
                0,
                &pclsParams,
                NULL);

            if (hr == WBEM_S_NO_ERROR) {
                CLOG(L"WmiSetBrightness: OK");

                IWbemClassObject *pclsInstance = NULL;
                hr = pclsParams->SpawnInstance(0, &pclsInstance);

                if (hr == WBEM_S_NO_ERROR) {
                    CLOG(L"Adding instance");
                    mon.wbemInstance = pclsInstance;
                    _instances.push_back(mon);
                }
            }
        }
    }

    COMUtil::SafeRelease(pLoc);

    if (_useBrightnessAPI) {
        _notifyHwnd = hWnd;
    }
}

WMIBrightnessController::~WMIBrightnessController() {
    COMUtil::SafeRelease(_pSvc);
    for (CapableMonitor &mon : _instances) {
        COMUtil::SafeRelease(mon.wbemObject);
        COMUtil::SafeRelease(mon.wbemInstance);
    }
}

bool WMIBrightnessController::SupportsBrightnessAPI() {
    return _useBrightnessAPI;
}

float WMIBrightnessController::Brightness() {
    return _brightness;
}

void WMIBrightnessController::Brightness(float level) {
    if (level > 1.0f) {
        level = 1.0f;
    }
    else if (level < 0.0f) {
        level = 0.0f;
    }

    if (level == Brightness()) {
        return;
    }

    BYTE nLevel = static_cast<BYTE>(level * 100.0f);
    CLOG(L"Brightness level (WMI): %d", nLevel);

    HRESULT hr = S_OK;
    for (CapableMonitor &mon : _instances) {
        CLOG("Trying to set brightness (WMI) for instance: %f", level);
        IWbemClassObject *pInstance = mon.wbemInstance;
        IWbemClassObject *pObject = mon.wbemObject;
        VARIANT vtTimeout = { };
        vtTimeout.vt = VT_I4;
        vtTimeout.ullVal = 0;

        hr = pInstance->Put(
            bstr_t("Timeout"),
            0,
            &vtTimeout,
            CIM_UINT32);
        VariantClear(&vtTimeout);

        CLOG("HRESULT Timeout 0x%x", hr);

        VARIANT vtBrightness = { };
        vtBrightness.vt = VT_UI1;
        vtBrightness.bVal = nLevel;

        hr = pInstance->Put(
            bstr_t("Brightness"),
            0,
            &vtBrightness,
            CIM_UINT8);
        VariantClear(&vtBrightness);

        VARIANT vtPath;
        VariantInit(&vtPath);

        hr = pObject->Get(
            bstr_t("__PATH"),
            0,
            &vtPath,
            NULL,
            NULL);

        hr = _pSvc->ExecMethod(
            vtPath.bstrVal,
            bstr_t("WmiSetBrightness"),
            0,
            NULL,
            pInstance,
            NULL,
            NULL);
        VariantClear(&vtPath);

        CLOG("ExecMethod WmiSetBrightness 0x%x", hr);
        CLOG("Setting brightness level (WMI) to %f", level);
    }
    if (hr == WBEM_S_NO_ERROR) {
        _brightness = level;
        PostMessage(_notifyHwnd, MSG_BRI_CHNG, static_cast<WPARAM>(1), 0);
    }
}