// Copyright (c) 2015, Matthew Malensek.
// Copyright (c) 2026, erisune.
// Distributed under the GPL-3.0 License (see COPYING for details)

#include "About.h"

#include "../resource.h"
#include "../Updater/Updater.h"
#include "../Updater/Version.h"

void About::Initialize() {
    _title = new Label(LBL_TITLE, *this, false);
    std::wstring version = Updater::MainAppVersion().ToString();
    _title->Text(L"3RVX " + version);
}

void About::LoadSettings() {

}

void About::SaveSettings() {

}
