// Copyright (c) 2015, Matthew Malensek.
// Copyright (c) 2026, erisune.
// Distributed under the GPL-3.0 License (see COPYING for details)

#pragma once

#include "SettingsTab.h"

#include <list>
#include <string>

class General : public SettingsTab {
public:
    General() :
    SettingsTab(NULL, MAKEINTRESOURCE(IDD_GENERAL), L"General") {

    }

    virtual void SaveSettings();

private:
    std::wstring _url;

    virtual void Initialize();
    virtual void LoadSettings();
    virtual bool LoadSkins();

    std::wstring GetLanguageName(std::wstring langFileName, bool translator = false);
    std::wstring GetLanguageFileName(std::wstring langName);
    std::list<std::wstring> FindLanguages(std::wstring dir);
    std::list<std::wstring> FindSkins(std::wstring dir);
    void LoadSkinInfo(std::wstring skinName);
    void LoadLanguageInfo(std::wstring languageName);
    bool RunOnStartup();
    bool RunOnStartup(bool enable);
    bool CheckForUpdates();

private:
    /* Controls: */
    GroupBox *_behaviorGroup;
    Checkbox *_startup;
    Checkbox *_showStartup;
    Checkbox *_sounds;
    Checkbox *_glass;
    Checkbox *_autoUpdate;
    Button *_checkNow;

    GroupBox *_skinGroup;
    ComboBox *_skin;
    Label *_author;
    Button *_website;
    Button *_refresh;
    Button *_skinOptions;

    GroupBox *_languageGroup;
    ComboBox *_language;
    Label *_translator;

private:
    static const wchar_t REGKEY_NAME[];
    static const wchar_t REGKEY_RUN[];
};