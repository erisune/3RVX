#pragma once

#include <string>
#include "../TinyXml2/tinyxml2.h"

class VariantInfo {
public:
    VariantInfo(std::wstring skinName, bool reportErrors = false);

    std::wstring Author();
    std::wstring URL();

protected:
    bool _validVariant;
    std::wstring _variantFile;
    std::wstring _variantDir;
    tinyxml2::XMLDocument _xml;
    tinyxml2::XMLElement *_root;
};