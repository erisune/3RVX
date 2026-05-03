// Copyright (c) 2015, Matthew Malensek.
// Copyright (c) 2026, erisune.
// Distributed under the GPL-3.0 License (see COPYING for details)

#pragma once

class HotkeyInfo;

/// <summary>
/// Interface for classes that are able to handle and process hotkey events.
/// </summary>
class HotkeyProcessor {
    virtual void ProcessHotkeys(HotkeyInfo &hki) = 0;
};