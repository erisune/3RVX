// Copyright (c) 2015, Matthew Malensek.
// Copyright (c) 2026, erisune.
// Distributed under the GPL-3.0 License (see COPYING for details)

#pragma once

#include "SettingsTab.h"

class About : public SettingsTab {
public:
    About() :
    SettingsTab(NULL, MAKEINTRESOURCE(IDD_ABOUT), L"About") {

    }

    virtual void SaveSettings();

protected:
    virtual void Initialize();
    virtual void LoadSettings();

private:
    /* Controls: */
    Label *_title;
};