// Copyright (c) 2015, Matthew Malensek.
// Copyright (c) 2026, erisune.
// Distributed under the GPL-3.0 License (see COPYING for details)

#pragma once

#include "MeterComponent.h"

#include "../MeterWnd/Meters/SliderKnob.h"

struct SliderComponent : public MeterComponent {
    SliderKnob *knob;

    virtual ~SliderComponent() {
        delete knob;
    }
};