// Copyright (c) 2015, Matthew Malensek.
// Copyright (c) 2026, erisune.
// Distributed under the GPL-3.0 License (see COPYING for details)

#pragma once

#include "Skin.h"
#include "SkinInfo.h"

class Meter;
class SliderKnob;
class SoundPlayer;

struct MeterComponent;

using tinyxml2::XMLElement;

/// <summary>
/// This skin implementation serves as a last resort in the skin loader
/// pipeline. If no suitable resources have been found in any of the other skins
/// in the pipeline, then the ErrorSkin displays an error message to let the
/// user know something went wrong.
/// </summary>
class ErrorSkin : public Skin {
public:
    virtual OSDComponent *VolumeOSD();
    virtual OSDComponent *MuteOSD();
    virtual OSDComponent *EjectOSD();
    virtual OSDComponent *BrightnessOSD();
    virtual OSDComponent *MicrophoneOSD();
    virtual OSDComponent *MicrophoneMuteOSD();

    virtual HICON EjectIcon();
    virtual std::vector<HICON> VolumeIconset();
    virtual std::vector<HICON> MicrophoneIconset();

    virtual SliderComponent *VolumeSlider();
};