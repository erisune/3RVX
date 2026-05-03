// Copyright (c) 2015, Matthew Malensek.
// Copyright (c) 2026, erisune.
// Distributed under the GPL-3.0 License (see COPYING for details)

#pragma once

#include "Control.h"
#include "../../3RVX/Logger.h"

class Label : public Control {
public:
    Label(int id, Dialog &parent, bool translate = true) :
    Control(id, parent, translate) {

    }
};