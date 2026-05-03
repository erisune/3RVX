// Copyright (c) 2015, Matthew Malensek.
// Copyright (c) 2026, erisune.
// Distributed under the GPL-3.0 License (see COPYING for details)

#include "MultiSkin.h"

OSDComponent *MultiSkin::VolumeOSD() {
    return nullptr;
}

OSDComponent *MultiSkin::MuteOSD() {
    return nullptr;
}

OSDComponent *MultiSkin::EjectOSD() {
    return nullptr;
}

std::vector<HICON> MultiSkin::VolumeIconset() {
    return std::vector<HICON>();
}

SliderComponent *MultiSkin::VolumeSlider() {
    return nullptr;
}
