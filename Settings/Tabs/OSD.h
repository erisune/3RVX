// Copyright (c) 2015, Matthew Malensek.
// Copyright (c) 2026, erisune.
// Distributed under the GPL-3.0 License (see COPYING for details)

#pragma once

#include "../../3RVX/Controllers/Volume/VolumeController.h"
#include "SettingsTab.h"

class OSD : public SettingsTab {
public:
    OSD() :
    SettingsTab(NULL, MAKEINTRESOURCE(IDD_OSD), L"OSD") {

    }

    virtual void SaveSettings();

protected:
    virtual void Initialize();
    virtual void LoadSettings();

private:
    std::vector<VolumeController::DeviceInfo> _audioDevices;
    std::vector<VolumeController::DeviceInfo> _captureDevices;
    std::unordered_map<int, std::wstring> _taperLevels;
    void ShowGroup(int group);

private:
    /* Control Event Handlers */
    void OnOSDListItemChange(NMLISTVIEW *lv);

private:
    /* Controls: */
    ListView *_osdList;

    GroupBox *_volumeGroup;
    Checkbox *_volumeIcon;
    Checkbox *_volumeShow;
    Checkbox *_subscribeVolEvents;
    Label *_audioDeviceLabel;
    ComboBox *_audioDevice;
    Label *_audioTaperLabel;
    EditBox *_audioTaperEdit;
    ComboBox *_audioTaper;
    Label *_limitLabel;
    Slider *_limitSlider;
    Label *_limitValue;
    Checkbox *_forceLimit;
    Checkbox *_muteLock;

    GroupBox *_microphoneGroup;
    Checkbox *_microphoneIcon;
    Checkbox *_microphoneShow;
    Checkbox *_subscribeMicEvents;
    Label *_captureDeviceLabel;
    ComboBox *_captureDevice;
    Checkbox *_micMuteLock;

    GroupBox *_ejectGroup;
    Checkbox *_ejectIcon;
    Checkbox *_ejectShow;
    Checkbox *_subscribeEjectEvents;

    GroupBox *_brightnessGroup;
    Checkbox *_brightnessIcon;
    Checkbox *_brightnessShow;
    Label *_displayDeviceLabel;
    ComboBox *_displayDevice;

    GroupBox *_keyboardGroup;
    Checkbox *_keyboardIcon;
    Checkbox *_keyboardShow;
    Checkbox *_caps;
    Checkbox *_scroll;
    Checkbox *_num;
    Checkbox *_media;
    Checkbox *_run;

    std::vector<GroupBox *> _groups;

private:
    /* Strings: */
    std::wstring _osdStr = L"OSD";
    std::wstring _volumeStr = L"Volume";
    std::wstring _microphoneStr = L"Microphone";
    std::wstring _brightnessStr = L"Brightness";
    std::wstring _ejectStr = L"Eject";
    std::wstring _keyboardStr = L"Keyboard";
};
