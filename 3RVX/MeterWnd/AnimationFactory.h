// Copyright (c) 2015, Matthew Malensek.
// Copyright (c) 2026, erisune.
// Distributed under the GPL-3.0 License (see COPYING for details)

#pragma once

#include "Animations\AnimationTypes.h"
class Animation;

class AnimationFactory {
public:
    static Animation *Create(AnimationTypes::HideAnimation anim, int speed);
};