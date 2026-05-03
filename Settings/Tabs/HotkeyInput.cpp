// Copyright (c) 2016, Matthew Malensek.
// Copyright (c) 2026, erisune.
// Distributed under the GPL-3.0 License (see COPYING for details)

#include "HotkeyInput.h"

#include "../../3RVX/Logger.h"
#include "../Controls/Controls.h"
#include "../UITranslator.h"
#include "KeyGrabber.h"

HotkeyInput::HotkeyInput(HWND parent) :
Dialog(parent, MAKEINTRESOURCE(IDD_HOTKEYPROMPT)) {
    
}

void HotkeyInput::Initialize() {
    UITranslator::TranslateWindowText(DialogHandle());
    _prompt = new Label(LBL_PROMPT, *this);
    KeyGrabber::Instance()->SetHwnd(DialogHandle());
    KeyGrabber::Instance()->Grab();
}
