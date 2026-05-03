// Copyright (c) 2015, Matthew Malensek.
// Copyright (c) 2026, erisune.
// Distributed under the GPL-3.0 License (see COPYING for details)

#pragma once

class MeterCallbackReceiver {
public:
    virtual void MeterChangeCallback(int units) = 0;
};