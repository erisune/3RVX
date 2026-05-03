// Copyright (c) 2015, Matthew Malensek.
// Copyright (c) 2026, erisune.
// Distributed under the GPL-3.0 License (see COPYING for details)

#pragma once

#include "Control.h"

class Button : public Control {
public:
    Button(int id, Dialog &parent, bool translate = true) :
    Control(id, parent, translate) {

    }

    virtual BOOL CALLBACK Command(unsigned short nCode);

public:
    /* Event Handlers */
    std::function<bool()> OnClick;

};