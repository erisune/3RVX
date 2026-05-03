// Copyright (c) 2015, Matthew Malensek.
// Copyright (c) 2026, erisune.
// Distributed under the GPL-3.0 License (see COPYING for details)

#pragma once

#include <thread>

#include "../Controls/Dialog.h"

#include "Updater.h"
#include "Version.h"

class ProgressBar;
class Button;

class ProgressWindow : public Dialog {
public:
    ProgressWindow(HWND parent, Version version);

private:
    Version _version;
    Button *_cancel;
    ProgressBar *_progress;

    void Initialize();

    std::thread _dlThread;
    void Download();

};