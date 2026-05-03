// Copyright (c) 2015, Matthew Malensek.
// Copyright (c) 2026, erisune.
// Distributed under the GPL-3.0 License (see COPYING for details)

#pragma once

#include "Control.h"

class ListBox : public Control {
public:
    ListBox(int id, Dialog &parent) :
    Control(id, parent, false) {

    }

    int AddItem(std::wstring item);
    int InsertItem(int index, std::wstring item);

    virtual BOOL CALLBACK Command(unsigned short nCode);

public:
    /* Event Handlers */
    std::function<bool()> OnSelectionChange;
};