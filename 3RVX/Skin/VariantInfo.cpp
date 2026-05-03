// Copyright (c) 2026, erisune.
// Distributed under the GPL-3.0 License (see COPYING for details)

#include "VariantInfo.h"

#include "../DefaultSettings.h"
#include "../Error.h"
#include "../Logger.h"
#include "../StringUtils.h"

VariantInfo::VariantInfo(std::wstring variantFile, bool reportErrors) :
_variantFile(variantFile) {
    CLOG(L"Loading variant from XML: %s", _variantFile.c_str());
    if (variantFile.empty()) {
        _validVariant = false;
        return;
    }
    std::wstring xmlName = std::wstring(DefaultSettings::VariantFileName);
    _variantDir = _variantFile.substr(0, _variantFile.length() - (xmlName.length() + 1));

    FILE *fp;
    _wfopen_s(&fp, _variantFile.c_str(), L"rb");
    if (fp == NULL) {
        if (reportErrors) {
            Error::ErrorMessage(Error::SKINERR_INVALID_SKIN, _variantFile.c_str());
        }
        return;
    }

    tinyxml2::XMLError result = _xml.LoadFile(fp);
    fclose(fp);
    if (result != tinyxml2::XMLError::XML_SUCCESS) {
        if (reportErrors) {
            Error::ErrorMessage(Error::SKINERR_XMLPARSE, _variantFile.c_str());
        }
        return;
    }

    _root = _xml.GetDocument()->FirstChildElement("variant");
    if (_root == NULL) {
        if (reportErrors) {
            Error::ErrorMessage(Error::SKINERR_MISSING_XMLROOT);
        }
        return;
    }
    _validVariant = true;
}

std::wstring VariantInfo::Author() {
    tinyxml2::XMLHandle xmlHandle(_root);
    tinyxml2::XMLElement *author = xmlHandle
        .FirstChildElement("info")
        .FirstChildElement("author")
        .ToElement();

    const char *authorText;
    if (author == NULL || (authorText = author->GetText()) == NULL) {
        return L"";
    }

    return std::wstring(StringUtils::Widen(authorText));
}

std::wstring VariantInfo::URL() {
    tinyxml2::XMLHandle xmlHandle(_root);
    tinyxml2::XMLElement *site = xmlHandle
        .FirstChildElement("info")
        .FirstChildElement("website")
        .ToElement();

    const char *text;
    if (site == NULL || (text = site->GetText()) == NULL) {
        return L"";
    }
    return std::wstring(StringUtils::Widen(text));
}