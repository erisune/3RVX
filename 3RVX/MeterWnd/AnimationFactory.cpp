// Copyright (c) 2015, Matthew Malensek.
// Copyright (c) 2026, erisune.
// Distributed under the GPL-3.0 License (see COPYING for details)

#include "AnimationFactory.h"

Animation *AnimationFactory::Create(
        AnimationTypes::HideAnimation anim, int speed) {

    Animation *animation;

    switch (anim) {
    case AnimationTypes::Fade:
        animation = new FadeOut(speed);
        break;

    case AnimationTypes::Slide:
        animation = new Slide(speed);
        break;

    case AnimationTypes::None:
    default:
        animation = NULL;
    }

    return animation;
}