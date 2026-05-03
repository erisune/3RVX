// Copyright (c) 2015, Matthew Malensek.
// Copyright (c) 2026, erisune.
// Distributed under the GPL-3.0 License (see COPYING for details)

#pragma once

#include "../Controls/Dialog.h"
#include "../resource.h"

class Label;

class HotkeyInput : public Dialog {
public:
    HotkeyInput(HWND parent);

private:
    /* Controls */
    Label *_prompt;

    void Initialize();
};