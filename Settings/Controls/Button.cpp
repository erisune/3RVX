// Copyright (c) 2015, Matthew Malensek.
// Copyright (c) 2026, erisune.
// Distributed under the GPL-3.0 License (see COPYING for details)

#include "Button.h"

BOOL CALLBACK Button::Command(unsigned short nCode) {
    switch (nCode) {
    case BN_CLICKED:
        if (OnClick) {
            return OnClick();
        }
    }

    return FALSE;
}
