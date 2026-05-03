// Copyright (c) 2015, Matthew Malensek.
// Copyright (c) 2026, erisune.
// Distributed under the GPL-3.0 License (see COPYING for details)

#pragma once

#include "Button.h"

class Checkbox : public Button {
public:
    Checkbox(int id, Dialog &parent, bool translate = true) :
    Button(id, parent, translate) {

    }

    bool Checked();
    bool Checked(bool checked);
};