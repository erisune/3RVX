// Copyright (c) 2015, Matthew Malensek.
// Distributed under the BSD 2-Clause License (see LICENSE.txt for details)

#pragma once

#include <Windows.h>

#include <string>
#include <vector>

class Skin;
struct OSDComponent;
struct SliderComponent;

class SkinManager {
public:
    static SkinManager *Instance();
    void LoadSkin(std::wstring skinXML, std::wstring variantXML);

    OSDComponent *VolumeOSD();
    std::vector<HICON> &VolumeIconset();
    SliderComponent *VolumeSlider();

    OSDComponent *MuteOSD();

    OSDComponent *EjectOSD();
    HICON &EjectIcon();

    OSDComponent *KeyboardOSD();
    HICON &CapsLockIcon();
    HICON &NumLockIcon();
    HICON &ScrollLockIcon();

    OSDComponent *BrightnessOSD();

    OSDComponent *MicrophoneOSD();
    std::vector<HICON> &MicrophoneIconset();

    OSDComponent *MicrophoneMuteOSD();

private:
    OSDComponent *_volumeOSD;
    std::vector<HICON> _volumeIconset;
    SliderComponent *_volumeSlider;
    OSDComponent *_muteOSD;

    OSDComponent *_ejectOSD;
    HICON _ejectIcon;

    OSDComponent *_keyboardOSD;
    HICON _capsIcon;
    HICON _numIcon;
    HICON _scrollIcon;

    OSDComponent *_brightnessOSD;

    OSDComponent *_microphoneOSD;
    std::vector<HICON> _microphoneIconset;
    OSDComponent *_microphoneMuteOSD;

    ~SkinManager();
    void DisposeComponents();

private:
    static SkinManager *instance;

};