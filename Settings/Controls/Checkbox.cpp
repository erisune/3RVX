// Copyright (c) 2015, Matthew Malensek.
// Copyright (c) 2026, erisune.
// Distributed under the GPL-3.0 License (see COPYING for details)

#include "Checkbox.h"

bool Checkbox::Checked() {
    return IsDlgButtonChecked(_parent, _id) == BST_CHECKED;
}

bool Checkbox::Checked(bool checked) {
    return CheckDlgButton(
        _parent,_id, checked ? BST_CHECKED : BST_UNCHECKED) == TRUE;
}

