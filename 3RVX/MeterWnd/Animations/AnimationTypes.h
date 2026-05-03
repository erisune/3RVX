// Copyright (c) 2015, Matthew Malensek.
// Copyright (c) 2026, erisune.
// Distributed under the GPL-3.0 License (see COPYING for details)

#pragma once

#include <vector>
#include <string>

#include "FadeOut.h"
#include "Slide.h"

class AnimationTypes {
public:
    enum HideAnimation {
        None,
        Fade,
        Slide
    };
    static std::vector<std::wstring> HideAnimationNames;
};