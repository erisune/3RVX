// Copyright (c) 2015, Matthew Malensek.
// Copyright (c) 2026, erisune.
// Distributed under the GPL-3.0 License (see COPYING for details)

#include "EditBox.h"

void EditBox::Clear() {
    Text(L"");
}

BOOL CALLBACK EditBox::Command(unsigned short nCode) {
    switch (nCode) {
    case EN_CHANGE:
        if (OnTextChange) {
            return OnTextChange();
        }
    }

    return FALSE;
}
