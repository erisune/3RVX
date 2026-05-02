// Copyright (c) 2015, Matthew Malensek.
// Distributed under the BSD 2-Clause License (see LICENSE.txt for details)

#include "SkinManager.h"

#include "Shlwapi.h"

#include "../Error.h"
#include "../Settings.h"

#include "ErrorSkin.h"
#include "MeterComponent.h"
#include "OSDComponent.h"
#include "Skin.h"
#include "SkinV2.h"
#include "SkinV3.h"
#include "SliderComponent.h"

SkinManager *SkinManager::instance;

SkinManager *SkinManager::Instance() {
    if (instance == NULL) {
        instance = new SkinManager();
    }
    return instance;
}

void SkinManager::LoadSkin(std::wstring skinXML, std::wstring variantXML) {
    DisposeComponents();

    /* First, make sure the skin directory exists. */
    Settings *settings = Settings::Instance();
    std::wstring skinDir = settings->SkinDir();
    if (PathFileExists(skinDir.c_str()) == FALSE) {
        Error::ErrorMessageDie(Error::SKINERR_SKINDIR, skinDir);
    }

    Skin *skin;
    SkinInfo info(skinXML, false);
    if (info.FormatVersion() == 2) {
        skin = new SkinV2(skinXML);
    } else {
        if (variantXML.empty()) {
            skin = new SkinV3(skinXML);
        } else {
            skin = new SkinV3(skinXML, variantXML);
        }
    }

    std::vector<Skin *> skins;
    skins.push_back(skin);

    for (Skin *skin : skins) {
        if (_volumeOSD == nullptr) {
            _volumeOSD = skin->VolumeOSD();
        }

        if (_volumeIconset.size() == 0) {
            _volumeIconset = skin->VolumeIconset();
        }

        if (_volumeSlider == nullptr) {
            _volumeSlider = skin->VolumeSlider();
        }

        if (_muteOSD == nullptr) {
            _muteOSD = skin->MuteOSD();
        }

        if (_ejectOSD == nullptr) {
            _ejectOSD = skin->EjectOSD();
        }

        if (_ejectIcon == nullptr) {
            _ejectIcon = skin->EjectIcon();
        }

        if (_keyboardOSD == nullptr) {
            _keyboardOSD = skin->KeyboardOSD();
        }

        if (_keyboardSet.size() == 0) {
            _keyboardSet = skin->KeyboardSet();
        }

        if (_capsIcon == nullptr) {
            _capsIcon = skin->CapsLockIcon();
        }

        if (_numIcon == nullptr) {
            _numIcon = skin->NumLockIcon();
        }

        if (_scrollIcon == nullptr) {
            _scrollIcon = skin->ScrollLockIcon();
        }

        if (_brightnessOSD == nullptr) {
            _brightnessOSD = skin->BrightnessOSD();
        }

        if (_brightnessIconset.size() == 0) {
            _brightnessIconset = skin->BrightnessIconset();
        }

        if (_brightnessSlider == nullptr) {
            _brightnessSlider = skin->BrightnessSlider();
        }

        if (_microphoneOSD == nullptr) {
            _microphoneOSD = skin->MicrophoneOSD();
        }

        if (_microphoneIconset.size() == 0) {
            _microphoneIconset = skin->MicrophoneIconset();
        }

        if (_microphoneMuteOSD == nullptr) {
            _microphoneMuteOSD = skin->MicrophoneMuteOSD();
        }

    }

    for (Skin *skin : skins) {
        delete skin;
    }
}

OSDComponent *SkinManager::VolumeOSD() {
    return _volumeOSD;
}

std::vector<HICON> &SkinManager::VolumeIconset() {
    return _volumeIconset;
}

SliderComponent *SkinManager::VolumeSlider() {
    return _volumeSlider;
}

OSDComponent *SkinManager::MuteOSD() {
    return _muteOSD;
}

OSDComponent *SkinManager::EjectOSD() {
    return _ejectOSD;
}

HICON &SkinManager::EjectIcon() {
    return _ejectIcon;
}

OSDComponent *SkinManager::KeyboardOSD() {
    return _keyboardOSD;
}

HICON &SkinManager::CapsLockIcon() {
    return _capsIcon;
}

HICON &SkinManager::NumLockIcon() {
    return _numIcon;
}

HICON &SkinManager::ScrollLockIcon() {
    return _scrollIcon;
}

std::vector<MeterComponent*> &SkinManager::KeyboardSet() {
    return _keyboardSet;
}

OSDComponent *SkinManager::BrightnessOSD() {
    return _brightnessOSD;
}

std::vector<HICON> &SkinManager::BrightnessIconset() {
    return _brightnessIconset;
}

SliderComponent *SkinManager::BrightnessSlider() {
    return _brightnessSlider;
}

OSDComponent *SkinManager::MicrophoneOSD() {
    return _microphoneOSD;
}

std::vector<HICON> &SkinManager::MicrophoneIconset() {
    return _microphoneIconset;
}

OSDComponent *SkinManager::MicrophoneMuteOSD() {
    return _microphoneMuteOSD;
}

SkinManager::~SkinManager() {
    DisposeComponents();
}

void SkinManager::DisposeComponents() {
    delete _volumeOSD;
    _volumeOSD = nullptr;
    for (HICON icon : _volumeIconset) {
        DestroyIcon(icon);
    }
    _volumeIconset.clear();
    delete _volumeSlider;
    _volumeSlider = nullptr;

    delete _muteOSD;
    _muteOSD = nullptr;

    delete _ejectOSD;
    _ejectOSD = nullptr;
    DestroyIcon(_ejectIcon);
    _ejectIcon = nullptr;

    delete _keyboardOSD;
    _keyboardOSD = nullptr;
    DestroyIcon(_capsIcon);
    DestroyIcon(_numIcon);
    DestroyIcon(_scrollIcon);
    _capsIcon = nullptr;
    _numIcon = nullptr;
    _scrollIcon = nullptr;
    for (MeterComponent *key : _keyboardSet) {
        delete key;
        key = nullptr;
    }
    _keyboardSet.clear();

    delete _brightnessOSD;
    _brightnessOSD = nullptr;
    for (HICON icon : _brightnessIconset) {
        DestroyIcon(icon);
    }
    _brightnessIconset.clear();
    _brightnessSlider = nullptr;

    delete _microphoneOSD;
    _microphoneOSD = nullptr;
    for (HICON icon : _microphoneIconset) {
        DestroyIcon(icon);
    }
    _microphoneIconset.clear();

    delete _microphoneMuteOSD;
    _microphoneMuteOSD = nullptr;
}