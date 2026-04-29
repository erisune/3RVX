// Copyright (c) 2015, Matthew Malensek.
// Distributed under the BSD 2-Clause License (see LICENSE.txt for details)

#include "General.h"
#include "SkinOptions.h"

#include <shellapi.h>

#include "../../3RVX/3RVX.h"
#include "../../3RVX/DefaultSettings.h"
#include "../../3RVX/Error.h"
#include "../../3RVX/LanguageTranslator.h"
#include "../../3RVX/Logger.h"
#include "../../3RVX/Settings.h"
#include "../../3RVX/Skin/SkinInfo.h"
#include "../../3RVX/StringUtils.h"
#include "../resource.h"
#include "../Updater/ProgressWindow.h"
#include "../Updater/Updater.h"

const wchar_t General::REGKEY_NAME[] = L"3RVX";
const wchar_t General::REGKEY_RUN[]
    = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";

void General::Initialize() {
    _behaviorGroup = new GroupBox(GRP_BEHAVIOR, *this);
    _startup = new Checkbox(CHK_STARTUP, *this);
    _showStartup = new Checkbox(CHK_SHOWSTARTUP, *this);
    _sounds = new Checkbox(CHK_SOUNDS, *this);
    _glass = new Checkbox(CHK_GLASS, *this);
    _autoUpdate = new Checkbox(CHK_AUTOUPDATE, *this);
    _checkNow = new Button(BTN_CHECK, *this);
    _checkNow->OnClick = std::bind(&General::CheckForUpdates, this);

    _skinGroup = new GroupBox(GRP_SKIN, *this);
    _skin = new ComboBox(CMB_SKIN, *this);
    _skin->OnSelectionChange = [this]() {
        LoadSkinInfo(_skin->Selection());
        return true;
    };
    _author = new Label(LBL_AUTHOR, *this);
    _website = new Button(BTN_WEBSITE, *this);
    _website->OnClick = [this]() {
        if (_url != L"") {
            ShellExecute(NULL, L"open", _url.c_str(),
                NULL, NULL, SW_SHOWNORMAL);
        }
        return true;
    };

    _refresh = new Button(BTN_REFRESH, *this);
    _refresh->Icon(MAKEINTRESOURCE(IDI_REFRESH));
    _refresh->OnClick = std::bind(&General::LoadSkins, this);
    _skinOptions = new Button(BTN_SKINOPTS, *this);
    _skinOptions->OnClick = [this]() {
        SkinOptions opts(TabPage::DialogHandle(), _skin->Selection());
        opts.Show();
        return true;
    };
    _languageGroup = new GroupBox(GRP_LANGUAGE, *this);
    _language = new ComboBox(CMB_LANG, *this);
    _language->OnSelectionChange = [this]() {
        LoadLanguageInfo(GetLanguageFileName(_language->Selection()));
        return true;
    };
    _translator = new Label(LBL_TRANSLATOR, *this);
}

bool General::LoadSkins() {
    Settings *settings = Settings::Instance();
    std::wstring selection = _skin->Selection();
    _skin->Clear();
    /* Determine which skins are available */
    std::list<std::wstring> skins = FindSkins(Settings::SkinDir().c_str());
    for (std::wstring skin : skins) {
        _skin->AddItem(skin);
    }
    _skin->Select(selection);
    return true;
}

void General::LoadSettings() {
    Settings *settings = Settings::Instance();
    LanguageTranslator *lt = settings->Translator();
    _startup->Checked(RunOnStartup());
    _showStartup->Checked(settings->ShowOnStartup());
    _sounds->Checked(settings->SoundEffectsEnabled());
    _glass->Checked(settings->GlassEffectsEnabled());
    _autoUpdate->Checked(settings->AutomaticUpdates());

    LoadSkins();

    /* Update the combo box with the current skin */
    std::wstring current = settings->CurrentSkin();
    int idx = _skin->Select(current);
    if (idx == CB_ERR) {
        _skin->Select(DefaultSettings::Skin);
    }
    LoadSkinInfo(current);

    /* Populate the language box */
    std::list<std::wstring> languages = FindLanguages(
        settings->LanguagesDir().c_str());
    for (std::wstring language : languages) {
        size_t ext = language.find(L".xml");
        if (ext == language.npos) {
            continue;
        }
        /* List only files in the new format */
        std::wstring langName = GetLanguageName(language);
        if (!langName.empty()) {
            _language->AddItem(langName);
        }
    }
    std::wstring currentLangFile = settings->LanguageName() + L".xml";
    std::wstring currentLang = GetLanguageName(currentLangFile);
    _language->Select(currentLang);
    LoadLanguageInfo(settings->LanguageName());
}

std::wstring General::GetLanguageName(std::wstring langFileName, bool translator) {
    CLOG(L"Reading language name in XML: %s", langFileName.c_str());
    std::wstring langFile = Settings::LanguagesDir() + L"\\" + langFileName;
    FILE *fp;
    _wfopen_s(&fp, langFile.c_str(), L"rb");
    if (fp == NULL) {
        return L"";
    }
    tinyxml2::XMLDocument xml;
    tinyxml2::XMLElement *root;
    tinyxml2::XMLError result = xml.LoadFile(fp);
    fclose(fp);
    if (result != tinyxml2::XMLError::XML_SUCCESS) {
        return L"";
    }
    root = xml.GetDocument()->FirstChildElement("translation");
    if (root == NULL) {
        return L"";
    }
    tinyxml2::XMLElement *trans;
    if (translator) {
        trans = root->FirstChildElement("author");
    } else {
        trans = root->FirstChildElement("language");
    }
    
    if (trans == NULL) {
        return L"";
    }
    return StringUtils::Widen(trans->Attribute("name"));
}

std::wstring General::GetLanguageFileName(std::wstring langName) {
    std::list<std::wstring> languages = FindLanguages(
        Settings::LanguagesDir().c_str());
    for (std::wstring language : languages) {
        size_t ext = language.find(L".xml");
        if (ext == language.npos) {
            continue;
        }
        std::wstring name = GetLanguageName(language);
        if (!name.empty()) {
            if (langName == name) {
                return language.substr(0, ext);
            }
        }
    }
    return DefaultSettings::Language;
}

void General::SaveSettings() {
    CLOG(L"Saving: General");
    Settings *settings = Settings::Instance();

    RunOnStartup(_startup->Checked());
    settings->ShowOnStartup(_showStartup->Checked());
    settings->SoundEffectsEnabled(_sounds->Checked());
    settings->GlassEffectsEnabled(_glass->Checked());
    settings->AutomaticUpdates(_autoUpdate->Checked());

    std::wstring skin = _skin->Selection();
    settings->CurrentSkin(skin);
    if (skin != settings->CurrentSkin()) {
        /* Clear variants if skin has been changed */
        settings->CurrentVariant(L"");
    }

    std::wstring lang = GetLanguageFileName(_language->Selection());
    if (lang != settings->LanguageName()) {
        settings->LanguageName(lang);
        _3RVX::SettingsMessage(_3RVX::MSG_MUSTRESTART, NULL);
    }
}

bool General::RunOnStartup() {
    long res;
    HKEY key = NULL;
    bool run = false;

    res = RegOpenKeyEx(HKEY_CURRENT_USER, REGKEY_RUN, NULL, KEY_READ, &key);
    if (res == ERROR_SUCCESS) {
        res = RegQueryValueEx(key, REGKEY_NAME, NULL, NULL, NULL, NULL);
        run = (res == ERROR_SUCCESS);
        RegCloseKey(key);
    }

    return run;
}

bool General::RunOnStartup(bool enable) {
    long res;
    HKEY key = NULL;
    bool ok = false;

    std::wstring path = Settings::AppDir() + L"\\3RVX.exe";

    res = RegOpenKeyEx(HKEY_CURRENT_USER, REGKEY_RUN,
        NULL, KEY_ALL_ACCESS, &key);
    if (res == ERROR_SUCCESS) {
        if (enable) {
            res = RegSetValueEx(key, REGKEY_NAME, NULL, REG_SZ,
                (LPBYTE) path.c_str(), static_cast<DWORD>(path.size() + 1) * sizeof(TCHAR));
            ok = (res == ERROR_SUCCESS);
        } else {
            res = RegDeleteValue(key, REGKEY_NAME);
            ok = (res == ERROR_SUCCESS);
        }
        RegCloseKey(key);
    }

    return ok;
}

std::list<std::wstring> General::FindSkins(std::wstring dir) {
    std::list<std::wstring> skins;
    WIN32_FIND_DATA ffd;
    HANDLE hFind;

    CLOG(L"Finding skins in: %s", dir.c_str());
    dir += L"\\*";
    hFind = FindFirstFile(dir.c_str(), &ffd);
    if (hFind == INVALID_HANDLE_VALUE) {
        CLOG(L"FindFirstFile() failed");
        return skins;
    }

    do {
        std::wstring fName(ffd.cFileName);
        if (fName.at(0) == L'.') {
            continue;
        }
        if (ffd.dwFileAttributes & ~FILE_ATTRIBUTE_DIRECTORY) {
            continue;
        }
        std::wstring skinXML = Settings::Instance()->SkinXML(fName);
        if (FindFirstFile(skinXML.c_str(), &ffd) == INVALID_HANDLE_VALUE) {
            QCLOG(L"XML not found in folder %s, skipping", fName.c_str());
            continue;
        }
        QCLOG(L"%s", fName.c_str());
        skins.push_back(fName);
    } while (FindNextFile(hFind, &ffd));
    FindClose(hFind);

    return skins;
}

void General::LoadSkinInfo(std::wstring skinName) {
    std::wstring skinXML = Settings::Instance()->SkinXML(skinName);
    SkinInfo s(skinXML);

    std::wstring transAuthor
        = Settings::Instance()->Translator()->TranslateAndReplace(
        L"Author: {1}", s.Author());
    _author->Text(transAuthor);

    std::wstring url = s.URL();
    if (url == L"") {
        _website->Disable();
    } else {
        _url = s.URL();
        _website->Enable();
    }
}

void General::LoadLanguageInfo(std::wstring languageName) {
    if (languageName == DefaultSettings::Language) {
        _translator->Text(L"");
        return;
    }
    std::wstring langFileName = languageName + L".xml";
    std::wstring langTranslator = GetLanguageName(langFileName, true);
    std::wstring translator = Settings::Instance()->Translator()->TranslateAndReplace(
        L"Translator: {1}", langTranslator);
    _translator->Text(translator);
}

std::list<std::wstring> General::FindLanguages(std::wstring dir) {
    std::list<std::wstring> languages;
    WIN32_FIND_DATA ffd;
    HANDLE hFind;

    CLOG(L"Finding language translations in: %s", dir.c_str());
    dir += L"\\*.xml";
    hFind = FindFirstFile(dir.c_str(), &ffd);
    if (hFind == INVALID_HANDLE_VALUE) {
        CLOG(L"FindFirstFile() failed");
        return languages;
    }

    do {
        std::wstring fName(ffd.cFileName);

        if (fName.at(0) == L'.') {
            continue;
        }

        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            continue;
        }

        QCLOG(L"%s", fName.c_str());
        languages.push_back(fName);
    } while (FindNextFile(hFind, &ffd));
    FindClose(hFind);

    return languages;
}

bool General::CheckForUpdates() {
    _checkNow->Enabled(false);
    HCURSOR waitCursor = LoadCursor(NULL, IDC_WAIT);
    if (waitCursor) {
        SetCursor(waitCursor);
    }

    Version local = Updater::MainAppVersion();
    Version remote = Updater::RemoteVersion();

    if (remote.ToInt() == 0) {
        /* If the remote version is 0, there must've been a connection error. */
        Error::ErrorMessage(Error::GENERR_UPDATEDL,
            L"Update server appears to be offline.");
        goto cleanup;
    }

    if (remote.NewerThan(local)) {
        Settings *settings = Settings::Instance();
        LanguageTranslator *translator = settings->Translator();

        int msgResult = MessageBox(
            DialogHandle(),
            translator->TranslateAndReplace(
                L"A new version of 3RVX ({1}) is available. Install now?",
                remote.ToString()).c_str(),
            translator->Translate(L"Update Available").c_str(),
            MB_YESNO | MB_ICONQUESTION);

        if (msgResult == IDYES) {
            ProgressWindow pw(TabPage::DialogHandle(), remote);
            INT_PTR result = pw.Show();
            if (result == 0) {
                /* Everything went file. Shut down the settings app. */
                SendMessage(_3RVX::MasterSettingsHwnd(), WM_CLOSE, 0, 0);
            }
        }

    } else {
        MessageBox(
            DialogHandle(),
            L"Your copy of 3RVX is up-to-date.",
            L"Update Check",
            MB_OK | MB_ICONINFORMATION);
    }

cleanup:
    HCURSOR arrowCursor = LoadCursor(NULL, IDC_ARROW);
    if (arrowCursor) {
        SetCursor(arrowCursor);
    }
    _checkNow->Enabled(true);
    return true;
}