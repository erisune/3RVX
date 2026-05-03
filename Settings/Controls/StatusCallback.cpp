// Copyright (c) 2015, Matthew Malensek.
// Copyright (c) 2026, erisune.
// Distributed under the GPL-3.0 License (see COPYING for details)

#include "StatusCallback.h"

#include "../../3RVX/Logger.h"

STDMETHODIMP StatusCallback::OnStartBinding(DWORD dwReserved, IBinding * pib) {
    return S_OK;
}

STDMETHODIMP StatusCallback::OnStopBinding(HRESULT hresult, LPCWSTR szError) {
    return S_OK;
}

STDMETHODIMP StatusCallback::OnProgress(
        unsigned long ulProgress,
        unsigned long ulProgressMax, 
        unsigned long ulStatusCode,
        LPCWSTR szStatusText) {

    CLOG(L"Download: [%s] %d/%d", szStatusText, ulProgress, ulProgressMax);
    return S_OK;
}

