// Copyright (c) 2015, Matthew Malensek.
// Distributed under the BSD 2-Clause License (see LICENSE.txt for details)

#include "SkinV3.h"

#include <algorithm>
#include <memory>
#include <vector>
#include <Shlwapi.h>

#include "../CommCtl.h"
#include "../Error.h"
#include "../MeterWnd/Meters/MeterTypes.h"
#include "../StringUtils.h"
#include "../SoundPlayer.h"
#include "AccentColor.h"
#include "OSDComponent.h"
#include "MeterComponent.h"
#include "SliderComponent.h"

SkinV3::SkinV3(std::wstring skinXML) :
SkinInfo(skinXML), VariantInfo(NULL) {

}

SkinV3::SkinV3(std::wstring skinXML, std::wstring variantXML) :
SkinInfo(skinXML), VariantInfo(variantXML) {

}

SkinV3::~SkinV3() {

}

OSDComponent *SkinV3::VolumeOSD() {
    return CreateOSDComponent("volume");
}

OSDComponent *SkinV3::MuteOSD() {
    return CreateOSDComponent("mute");
}

OSDComponent *SkinV3::EjectOSD() {
    return CreateOSDComponent("eject");
}

OSDComponent *SkinV3::KeyboardOSD() {
    return CreateOSDComponent("keyboard");
}

OSDComponent *SkinV3::BrightnessOSD() {
    return CreateOSDComponent("brightness");
}

std::vector<HICON> SkinV3::BrightnessIconset() {
    std::vector<HICON> iconList;

    XMLElement* brightnessIconElem = SubElement("osds", "brightness");
    if (brightnessIconElem == NULL) {
        return iconList;
    }

    return Iconset(brightnessIconElem);
}

HICON SkinV3::EjectIcon() {
    XMLElement *eject = SubElement("osds", "eject");
    if (eject == nullptr) {
        return NULL;
    }
    return Icon(eject);
}

HICON SkinV3::CapsLockIcon() {
    XMLElement *keyboard = SubElement("osds", "keyboard");
    if (keyboard == nullptr) {
        return NULL;
    }
    XMLElement *capsLock = keyboard->FirstChildElement("capsLock");
    if (capsLock == nullptr) {
        return NULL;
    }
    return Icon(capsLock);
}

HICON SkinV3::NumLockIcon() {
    XMLElement *keyboard = SubElement("osds", "keyboard");
    if (keyboard == nullptr) {
        return NULL;
    }
    XMLElement *numLock = keyboard->FirstChildElement("numLock");
    if (numLock == nullptr) {
        return NULL;
    }
    return Icon(numLock);
}

HICON SkinV3::ScrollLockIcon() {
    XMLElement *keyboard = SubElement("osds", "keyboard");
    if (keyboard == nullptr) {
        return NULL;
    }
    XMLElement *scrollLock = keyboard->FirstChildElement("scrollLock");
    if (scrollLock == nullptr) {
        return NULL;
    }
    return Icon(scrollLock);
}

std::vector<HICON> SkinV3::VolumeIconset() {
    std::vector<HICON> iconList;

    XMLElement *volumeIconElem = SubElement("osds", "volume");
    if (volumeIconElem == NULL) {
        return iconList;
    }

    return Iconset(volumeIconElem);
}

SliderComponent *SkinV3::VolumeSlider() {
    return CreateSliderComponent("volume");
}

SliderComponent *SkinV3::BrightnessSlider() {
    return CreateSliderComponent("brightness");
}

OSDComponent *SkinV3::MicrophoneOSD() {
    return CreateOSDComponent("mic");
}

OSDComponent *SkinV3::MicrophoneMuteOSD() {
    return CreateOSDComponent("micmute");
}

std::vector<HICON> SkinV3::MicrophoneIconset() {
    std::vector<HICON> iconList;

    XMLElement* micIconElem = SubElement("osds", "mic");
    if (micIconElem == NULL) {
        return iconList;
    }

    return Iconset(micIconElem);
}

OSDComponent *SkinV3::CreateOSDComponent(char *osdName) {
    XMLElement *osdElem = SubElement("osds", osdName);
    if (osdElem == NULL) {
        return NULL;
    }

    OSDComponent *osd = new OSDComponent;
    if (PopulateMeterComponent(osd, osdElem) == false) {
        /* Could not fully populate the OSD */
        delete osd;
        return NULL;
    }
    osd->defaultUnits = DefaultUnits(osdElem);

    return osd;
}

SliderComponent *SkinV3::CreateSliderComponent(char *sliderName) {
    XMLElement *sliderElem = SubElement("sliders", sliderName);
    if (sliderElem == NULL) {
        return NULL;
    }

    SliderComponent *slider = new SliderComponent;
    if (PopulateMeterComponent(slider, sliderElem) == false) {
        /* Could not fully populate the slider */
        delete slider;
        return NULL;
    }
    slider->knob = Knob(sliderElem);

    return slider;
}

bool SkinV3::PopulateMeterComponent(
        MeterComponent *component, XMLElement *elem) {
    component->background = Image(elem, "background");
    component->mask = Image(elem, "mask");
    component->meters = Meters(elem);
    component->sound = Sound(elem);

    if (component->background == NULL) {
        /* A meter skin component has to have a background image */
        return false;
    }

    return true;
}

int SkinV3::DefaultUnits(XMLElement *elem) {
    int defaultUnits = DEFAULT_UNITS;
    elem->QueryIntAttribute("defaultUnits", &defaultUnits);
    return defaultUnits;
}

Gdiplus::Bitmap *SkinV3::Image(XMLElement *elem, char *attName) {
    const char *imgFile = elem->Attribute(attName);
    if (imgFile == NULL) {
        std::wstring aName = StringUtils::Widen(attName);
        CLOG(L"Could not find XML attribute: %s", aName.c_str());
        return NULL;
    }

    std::wstring wImgFile;
    if (_validVariant) {
        wImgFile = _variantDir + L"\\" + StringUtils::Widen(imgFile);
        if (!PathFileExists(wImgFile.c_str())) {
            /* Check for parent resource */
            wImgFile = _skinDir + L"\\" + StringUtils::Widen(imgFile);
        }
    } else {
        wImgFile = _skinDir + L"\\" + StringUtils::Widen(imgFile);
    }
    if (!PathFileExists(wImgFile.c_str())) {
        Error::ErrorMessageDie(Error::GENERR_NOTFOUND, wImgFile);
    }

    Gdiplus::Bitmap *bg = Gdiplus::Bitmap::FromFile(wImgFile.c_str());
    return bg;
}

std::wstring SkinV3::ImageName(XMLElement *meterXMLElement) {
    const char *imgName = meterXMLElement->Attribute("image");
    if (imgName == NULL) {
        return NULL;
    }
    std::wstring imgPath;
    if (_validVariant) {
        imgPath = _variantDir + L"\\" + StringUtils::Widen(imgName);
        if (PathFileExists(imgPath.c_str())) {
            return imgPath;
        }
    }
    imgPath = _skinDir + L"\\" + StringUtils::Widen(imgName);
    return imgPath;
}

HICON SkinV3::Icon(XMLElement *elem) {
    XMLElement *icon = elem->FirstChildElement("icon");
    if (icon == nullptr) {
        return nullptr;
    }

    const char *file = icon->Attribute("file");
    if (file == nullptr) {
        return nullptr;
    }

    std::wstring iconPath;
    if (_validVariant) {
        iconPath = _variantDir + L"\\" + StringUtils::Widen(file);
        if (PathFileExists(iconPath.c_str())) {
            return Skin::ReadIcon(iconPath);
        }
    }
    iconPath = _skinDir + L"\\" + StringUtils::Widen(file);
    return Skin::ReadIcon(iconPath);
}

std::vector<HICON> SkinV3::Iconset(XMLElement *elem) {
    XMLElement *set = elem->FirstChildElement("iconset");
    if (set == NULL) {
        return std::vector<HICON>();
    }

    const char *loc = set->Attribute("location");
    if (loc == NULL) {
        CLOG(L"Unknown iconset location");
        return std::vector<HICON>();
    }

    std::wstring iconDir;
    if (_validVariant) {
        iconDir = _variantDir + L"\\" + StringUtils::Widen(loc) + L"\\";
        std::vector<HICON> icons = Skin::ReadIconDirectory(iconDir);
        if (!icons.empty()) {
            return icons;
        }
    }
    iconDir = _skinDir + L"\\" + StringUtils::Widen(loc) + L"\\";
    return Skin::ReadIconDirectory(iconDir);
}

SoundPlayer *SkinV3::Sound(XMLElement *elem) {
    XMLElement *sound = elem->FirstChildElement("sound");
    if (sound == NULL) {
        return NULL;
    }

    const char *fileName = sound->Attribute("file");
    if (fileName == NULL) {
        CLOG(L"OSD has <sound> tag but no file specified");
        return NULL;
    }

    std::wstring wFileName;
    if (_validVariant) {
        wFileName = _variantDir + L"\\" + StringUtils::Widen(fileName);
        if (!PathFileExists(wFileName.c_str())) {
            wFileName = _skinDir + L"\\" + StringUtils::Widen(fileName);
        }
    } else {
        wFileName = _skinDir + L"\\" + StringUtils::Widen(fileName);
    }
    
    if (PathFileExists(wFileName.c_str()) == FALSE) {
        Error::ErrorMessage(Error::GENERR_NOTFOUND, wFileName);
    }

    SoundPlayer *player = new SoundPlayer(wFileName);
    if (player->Ready() == false) {
        CLOG(L"Failed to initialize sound player");
        delete player;
        return NULL;
    }

    return player;
}

std::vector<Meter *> SkinV3::Meters(XMLElement *parentElement) {
    std::vector<Meter*> meters;

    XMLElement *meter = parentElement->FirstChildElement("meter");
    for (; meter != NULL; meter = meter->NextSiblingElement("meter")) {
        Meter *m = LoadMeter(meter);
        if (m != NULL) {
            meters.push_back(m);
        }
    }

    return meters;
}

Meter *SkinV3::LoadMeter(XMLElement *meterXMLElement) {
    const char *meterType = meterXMLElement->Attribute("type");
    if (meterType == NULL) {
        /* If we dont' know the meter type, we can't proceed. */
        CLOG(L"Unknown meter type!");
        return NULL;
    }

    std::string type(meterType);
    std::transform(type.begin(), type.end(), type.begin(), ::tolower);

    /* IntAttribute() returns 0 on error, the default position. */
    int x = meterXMLElement->IntAttribute("x");
    int y = meterXMLElement->IntAttribute("y");

    int units = 10;
    meterXMLElement->QueryIntAttribute("units", &units);

    if (units > 100) {
        units = 100;
    }
    if (units < 1) {
        units = 1;
    }

    /* Several of the meters can be inverted (flipped), so we check for this
     * flag now. */
    bool inverted = false;
    meterXMLElement->QueryBoolAttribute("inverted", &inverted);

    /* Check for meter background image. 'text' is the only meter
     * that does not require an image. */
    std::wstring img;
    if (type != "text") {
        img = ImageName(meterXMLElement);
        if (PathFileExists(img.c_str()) == FALSE) {
            Error::ErrorMessageDie(Error::GENERR_NOTFOUND, img);
        }
    }

    Meter *m = NULL;
    if (type == "bitstrip") {
        m = new Bitstrip(img, x, y, units);
    } else if (type == "horizontalbar") {
        m = new HorizontalBar(img, x, y, units);
    } else if (type == "horizontalendcap") {
        m = new HorizontalEndcap(img, x, y, units);
    } else if (type == "horizontalsliderknob") {
        int width = meterXMLElement->IntAttribute("width");
        int height = meterXMLElement->IntAttribute("height");
        m = new HorizontalSliderKnob(img, x, y, width, height);
    } else if (type == "horizontaltile") {
        m = new HorizontalTile(img, x, y, units, inverted);
    } else if (type == "image") {
        m = new StaticImage(img, x, y);
    } else if (type == "numberstrip") {
        Gdiplus::StringAlignment align = Alignment(meterXMLElement);
        m = new NumberStrip(img, x, y, units, align);
    } else if (type == "text") {
        int width = meterXMLElement->IntAttribute("width");
        int height = meterXMLElement->IntAttribute("height");

        Gdiplus::Font *font = Font(meterXMLElement);
        Gdiplus::StringAlignment align = Alignment(meterXMLElement);

        const char *fontColor = meterXMLElement->Attribute("color");
        std::wstring color(L"FFFFFF");
        if (fontColor != NULL) {
            color = std::wstring(StringUtils::Widen(fontColor));
        }
        int itrans = 255;
        meterXMLElement->QueryIntAttribute("transparency", &itrans);
        byte transparency = (byte) itrans;

        int iglow = 0;
        meterXMLElement->QueryIntAttribute("glow", &iglow);
        byte glow = (byte) iglow;

        const char* stringFormat = meterXMLElement->Attribute("format");
        std::wstring format(L"[[PERC]]%");
        if (stringFormat != NULL) {
            format = std::wstring(StringUtils::Widen(stringFormat));
        }

        m = new Text(x, y, width, height, font, align, color,
            transparency, format, glow);

        delete font;

    } else if (type == "verticalbar") {
        m = new VerticalBar(img, x, y, units, inverted);
    } else if (type == "verticalsliderknob") {
        int width = meterXMLElement->IntAttribute("width");
        int height = meterXMLElement->IntAttribute("height");
        m = new VerticalSliderKnob(img, x, y, width, height);
    } else {
        CLOG(L"Unknown meter type: %s", StringUtils::Widen(type).c_str());
        return NULL;
    }

    const char *colorTransform = meterXMLElement->Attribute("colorTransform");
    if (colorTransform != NULL) {
        unsigned long searchColor = strtoul(colorTransform, NULL, 16);
        /* Always use alpha of 255 for the search color */
        searchColor |= 0xFF000000;

        UINT32 accentColor = AccentColor::Instance()->Color();

        int trans = 0;
        meterXMLElement->QueryIntAttribute(
            "colorTransformTransparency", &trans);
        trans &= 0xFF;

        /* Check if the color transformation should use a color matrix */
        bool matrix = false;
        meterXMLElement->QueryBoolAttribute(
            "colorTransformMatrix", &matrix);

        if (matrix) {
            m->ApplyColorTransformMatrix(searchColor, accentColor, trans);
        } else {
            m->ApplyColorTransform(searchColor, accentColor, trans);
        }
    }

    CLOG(L"Created meter [%s]:\n%s",
        StringUtils::Widen(type).c_str(), m->ToString().c_str());

    return m;
}

Gdiplus::Font *SkinV3::Font(XMLElement *meterXMLElement) {
    const char *fontName = meterXMLElement->Attribute("font");
    std::wstring name(L"Arial");
    if (fontName != NULL) {
        name = std::wstring(StringUtils::Widen(fontName));
    }

    float size = 10;
    meterXMLElement->QueryFloatAttribute("size", &size);

    int styleFlags = 0;
    const char *fontStyle = meterXMLElement->Attribute("style");
    if (fontStyle != NULL) {
        std::wstring style(StringUtils::Widen(fontStyle));
        std::transform(style.begin(), style.end(), style.begin(), ::tolower);

        std::wstring::size_type sz;
        sz = style.find(L"bold", 0);
        if (sz != std::wstring::npos) {
            styleFlags |= Gdiplus::FontStyleBold;
        }

        sz = style.find(L"italic", 0);
        if (sz != std::wstring::npos) {
            styleFlags |= Gdiplus::FontStyleItalic;
        }

        sz = style.find(L"underline", 0);
        if (sz != std::wstring::npos) {
            styleFlags |= Gdiplus::FontStyleUnderline;
        }

        sz = style.find(L"strike", 0);
        if (sz != std::wstring::npos) {
            styleFlags |= Gdiplus::FontStyleStrikeout;
        }
    }

    Gdiplus::Font *font = new Gdiplus::Font(name.c_str(), size, styleFlags);
    return font;
}

Gdiplus::StringAlignment
SkinV3::Alignment(XMLElement *meterXMLElement) {
    const char *align = meterXMLElement->Attribute("align");
    if (align == NULL) {
        align = "left";
    }

    std::string alignStr(align);
    std::transform(alignStr.begin(), alignStr.end(),
        alignStr.begin(), ::tolower);

    Gdiplus::StringAlignment alignment = Gdiplus::StringAlignmentNear;
    if (alignStr == "right") {
        alignment = Gdiplus::StringAlignmentFar;
    } else if (alignStr == "center") {
        alignment = Gdiplus::StringAlignmentCenter;
    }

    return alignment;
}

SliderKnob *SkinV3::Knob(XMLElement *elem) {
    XMLElement *slider = elem->FirstChildElement("slider");
    if (slider == nullptr) {
        Error::ErrorMessageDie(Error::SKINERR_MISSING_XML, L"<slider>");
        return nullptr;
    }

    std::wstring img = ImageName(slider);
    if (PathFileExists(img.c_str()) == FALSE) {
        Error::ErrorMessageDie(Error::GENERR_NOTFOUND, img);
    }

    const char *type = slider->Attribute("type");
    if (type == nullptr) {
        type = "vertical";
    }

    std::string typeStr(type);
    std::transform(typeStr.begin(), typeStr.end(),
        typeStr.begin(), ::tolower);

    if (typeStr != "vertical" && typeStr != "horizontal") {
        Error::ErrorMessageDie(
            Error::SKINERR_INVALID_SLIDERTYPE,
            StringUtils::Widen(typeStr));
    }

    bool vertical = true;
    if (typeStr == "horizontal") {
        vertical = false;
    }

    int x = slider->IntAttribute("x");
    int y = slider->IntAttribute("y");
    int w = slider->IntAttribute("width");
    int h = slider->IntAttribute("height");

    SliderKnob *knob;
    if (vertical == true) {
        knob = new VerticalSliderKnob(img, x, y, w, h);
    } else {
        knob = new HorizontalSliderKnob(img, x, y, w, h);
    }
    return knob;
}