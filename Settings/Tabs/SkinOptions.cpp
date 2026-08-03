// Copyright (c) 2026, erisune.
// Distributed under the GPL-3.0 License (see COPYING for details)

#include "SkinOptions.h"

#include "../../3RVX/3RVX.h"
#include "../../3RVX/DefaultSettings.h"
#include "../../3RVX/LanguageTranslator.h"
#include "../../3RVX/Logger.h"
#include "../../3RVX/Settings.h"
#include "../../3RVX/StringUtils.h"
#include "../../3RVX/Skin/VariantInfo.h"
#include "../Controls/Controls.h"
#include "../UITranslator.h"

SkinOptions::SkinOptions(HWND parent, std::wstring skinName) :
Dialog(parent, MAKEINTRESOURCE(IDD_SKINOPTS)) {
    _skinName = skinName;
}

void SkinOptions::Initialize() {
    Center();
    UITranslator::TranslateWindowText(DialogHandle());
    _variantGroup = new GroupBox(GRP_VARIANT, *this);
    _variant = new ComboBox(CMB_VARIANT, *this);
    _variant->OnSelectionChange = [this]() {
        LoadVariantInfo(_variant->Selection());
        return true;
    };
    _variantAuthor = new Label(LBL_VARIANTAUTHOR, *this);

    _propertiesGroup = new GroupBox(GRP_PROPERTIES, *this);
    _opacityLabel = new Label(LBL_OPACITY, *this);
    _opacity = new EditBox(ED_OPACITY, *this);
    _opacitySpinner = new Spinner(SP_OPACITY, *this);
    _opacitySpinner->Buddy(ED_OPACITY);
    _opacitySpinner->Range(MIN_OPACITY, MAX_OPACITY);
    _sizeLabel = new Label(LBL_SIZE, *this);
    _size = new EditBox(ED_SIZE, *this);
    _sizeSpinner = new Spinner(SP_SIZE, *this);
    _sizeSpinner->Buddy(ED_SIZE);
    _sizeSpinner->Range(MIN_SIZE, MAX_SIZE);
    _useAccentColor = new Checkbox(CHK_ACCENT, *this);
    _propertiesGroup->AddChildren({
        _opacityLabel,
        _opacity,
        _opacitySpinner,
        _sizeLabel,
        _size,
        _sizeSpinner,
        _useAccentColor
    });

    _ok = new Button(BTN_OPTSOK, *this, true);
    _ok->OnClick = [this]() {
        SaveSettings();
        Close();
        return true;
    };
    _cancel = new Button(BTN_OPTSCANCEL, *this, true);
    _cancel->OnClick = [this]() {
        Close();
        return true;
    };

    LoadVariants(_skinName);
    LoadSettings();
}

void SkinOptions::LoadSettings() {
    Settings *settings = Settings::Instance();

    std::wstring variant = settings->CurrentVariant();
    if (variant.empty()) {
        _variant->Select(0);
    } else {
        _variant->Select(variant);
    }

    int opacity = min(max(MIN_OPACITY, settings->Opacity()), MAX_OPACITY);
    _opacity->Text(opacity);

    int size = min(max(MIN_SIZE, settings->Size()), MAX_SIZE);
    _size->Text(size);

    _useAccentColor->Checked(settings->UseAccentColor());
}

void SkinOptions::SaveSettings() {
    Settings *settings = Settings::Instance();

    int opacity = min(max(MIN_OPACITY, _opacity->TextAsInt()), MAX_OPACITY);
    settings->Opacity(opacity);

    int size = min(max(MIN_SIZE, _size->TextAsInt()), MAX_SIZE);
    settings->Size(size);
    
    settings->CurrentSkin(_skinName);
    if (_variant->SelectionIndex() == 0) {
        settings->CurrentVariant(L"");
    } else {
        settings->CurrentVariant(_variant->Selection());
    }

    settings->UseAccentColor(_useAccentColor->Checked());

    Settings::Instance()->Save();

    CLOG(L"Notifying 3RVX process of settings change");
    _3RVX::Message(_3RVX::MSG_LOAD, NULL, true);
}

bool SkinOptions::LoadVariants(std::wstring skinName) {
    Settings *settings = Settings::Instance();
    _variant->Clear();
    LanguageTranslator *translator = settings->Translator();
    std::wstring noVariant = L"<" + translator->Translate(DefaultSettings::Variant) + L">";
    _variant->AddItem(noVariant);
    /* Determine which variants are available */
    std::wstring variantDir = Settings::SkinDir() + L"\\" + 
        skinName + L"\\" + DefaultSettings::VariantDirName;
    if (variantDir.empty()) {
        return false;
    }

    std::list<std::wstring> variants = FindVariants(variantDir.c_str());
    for (std::wstring variant : variants) {
        _variant->AddItem(variant);
    }

    /* Update the combo box with the current variant or none at all */
    std::wstring current = settings->CurrentVariant();
    int idx = _variant->Select(current);
    if (idx == CB_ERR) {
        _variant->Select(0);
    }
    LoadVariantInfo(current);
    return true;
}

std::list<std::wstring> SkinOptions::FindVariants(std::wstring dir) {
    std::list<std::wstring> variants;
    WIN32_FIND_DATA ffd;
    HANDLE hFind;

    CLOG(L"Finding variants in: %s", dir.c_str());
    std::wstring variantPath = dir;
    dir += L"\\*";
    hFind = FindFirstFile(dir.c_str(), &ffd);
    if (hFind == INVALID_HANDLE_VALUE) {
        CLOG(L"FindFirstFile() failed");
        return variants;
    }
    do {
        std::wstring fName(ffd.cFileName);
        if (fName.at(0) == L'.') {
            continue;
        }
        if (ffd.dwFileAttributes & ~FILE_ATTRIBUTE_DIRECTORY) {
            continue;
        }
        std::wstring variantXML = variantPath + L"\\" + fName + L"\\" + DefaultSettings::VariantFileName;
        if (FindFirstFile(variantXML.c_str(), &ffd) == INVALID_HANDLE_VALUE) {
            QCLOG(L"XML not found in folder %s, skipping", fName.c_str());
            continue;
        }
        QCLOG(L"%s", fName.c_str());
        variants.push_back(fName);
    } while (FindNextFile(hFind, &ffd));
    FindClose(hFind);

    return variants;
}

void SkinOptions::LoadVariantInfo(std::wstring variantName) {
    if (_variant->SelectionIndex() == 0) {
        _variantAuthor->Text(L"");
        return;
    }
    std::wstring variantXML = Settings::Instance()->VariantXML(_skinName, variantName);
    VariantInfo v(variantXML);

    std::wstring transAuthor
        = Settings::Instance()->Translator()->TranslateAndReplace(
        L"Author: {1}", v.Author());
    _variantAuthor->Text(transAuthor);
}