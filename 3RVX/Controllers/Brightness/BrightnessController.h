// Copyright (c) 2017, Matthew Malensek.
// Copyright (c) 2026, erisune.
// Distributed under the GPL-3.0 License (see COPYING for details)

#pragma once

class BrightnessController {
public:
    virtual float Brightness() = 0;
    virtual void Brightness(float level) = 0;
    virtual bool SupportsBrightnessAPI() = 0;

    static const int MSG_BRI_CHNG = WM_APP + 2080;
};