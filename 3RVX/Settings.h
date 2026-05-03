// Copyright (c) 2015, Matthew Malensek.
// Copyright (c) 2026, erisune.
// Distributed under the GPL-3.0 License (see COPYING for details)

#pragma once

#include <unordered_map>
#include <string>

#include "TinyXml2\tinyxml2.h"
#include "MeterWnd\Animations\AnimationTypes.h"

class HotkeyInfo;
class LanguageTranslator;
class Skin;

class Settings {
public:
    enum OSDPos {
        Top,
        Left,
        Right,
        Bottom,
        Center,
        TopLeft,
        TopRight,
        BottomLeft,
        BottomRight,
        CustomPosition,
    };
    static std::vector<std::wstring> OSDPosNames;

public:
    static Settings *Instance();

    void Load();
    void LoadEmptySettings();
    int Save();

    /* General Settings */

    bool SoundEffectsEnabled();
    void SoundEffectsEnabled(bool enable);
    bool GlassEffectsEnabled();
    void GlassEffectsEnabled(bool enable);

    /* Skin */
    std::wstring CurrentSkin();
    bool CurrentSkin(std::wstring skinName);
    std::wstring SkinXML();
    std::wstring SkinXML(std::wstring skinName);

    std::wstring CurrentVariant();
    bool CurrentVariant(std::wstring variantName);
    std::wstring VariantXML();
    std::wstring VariantXML(std::wstring skinName, std::wstring variantName);

    int Opacity();
    void Opacity(int opacity);
    int Size();
    void Size(int size);

    /* Language */
    LanguageTranslator *Translator();
    std::wstring LanguageName();
    void LanguageName(std::wstring name);

    /* Animation */
    AnimationTypes::HideAnimation HideAnim();
    void HideAnim(AnimationTypes::HideAnimation anim);
    int HideDelay();
    void HideDelay(int delay);
    int HideSpeed();
    void HideSpeed(int speed);

    /* Display */
    bool AlwaysOnTop();
    void AlwaysOnTop(bool enable);
    bool HideFullscreen();
    void HideFullscreen(bool enable);
    bool HideDirectX();
    void HideDirectX(bool enable);
    std::wstring Monitor();
    void Monitor(std::wstring monitorName);
    OSDPos OSDPosition();
    void OSDPosition(OSDPos pos);
    int OSDEdgeOffset();
    void OSDEdgeOffset(int offset);
    int OSDX();
    void OSDX(int x);
    int OSDY();
    void OSDY(int y);

    /* OSDs */
    bool BrightnessOSDEnabled();
    void BrightnessOSDEnabled(bool enable);
    bool EjectOSDEnabled();
    void EjectOSDEnabled(bool enable);
    bool KeyboardOSDEnabled();
    void KeyboardOSDEnabled(bool enable);
    bool VolumeOSDEnabled();
    void VolumeOSDEnabled(bool enable);
    bool MicrophoneOSDEnabled();
    void MicrophoneOSDEnabled(bool enable);

    bool ShowBrightnessOSD();
    void ShowBrightnessOSD(bool enable);
    bool ShowEjectOSD();
    void ShowEjectOSD(bool enable);
    bool ShowKeyboardOSD();
    void ShowKeyboardOSD(bool enable);
    bool ShowVolumeOSD();
    void ShowVolumeOSD(bool enable);
    bool ShowMicrophoneOSD();
    void ShowMicrophoneOSD(bool enable);

    /* Volume */
    void AudioDeviceID(std::wstring id);
    std::wstring AudioDeviceID();
    bool MuteOnLock();
    void MuteOnLock(bool enable);
    bool SubscribeVolumeEvents();
    void SubscribeVolumeEvents(bool enable);
    int VolumeCurveAdjustment();
    void VolumeCurveAdjustment(int value);
    float VolumeLimiter();
    void VolumeLimiter(float limit);
    bool VolumeIconEnabled();
    void VolumeIconEnabled(bool enable);

    /* Microphone */
    void CaptureDeviceID(std::wstring id);
    std::wstring CaptureDeviceID();
    bool MicMuteOnLock();
    void MicMuteOnLock(bool enable);
    bool SubscribeMicrophoneEvents();
    void SubscribeMicrophoneEvents(bool enable);
    bool MicrophoneIconEnabled();
    void MicrophoneIconEnabled(bool enable);

    /* Eject */
    bool EjectIconEnabled();
    void EjectIconEnabled(bool enable);
    bool SubscribeEjectEvents();
    void SubscribeEjectEvents(bool enable);

    /* Keyboard */
    bool KeyboardIconsEnabled();
    void KeyboardIconsEnabled(bool enable);
    bool CapsLockEnabled();
    void CapsLockEnabled(bool enable);
    bool NumLockEnabled();
    void NumLockEnabled(bool enable);
    bool ScrollLockEnabled();
    void ScrollLockEnabled(bool enable);
    bool MediaKeysEnabled();
    void MediaKeysEnabled(bool enable);
    bool ShowRunHotkey();
    void ShowRunHotkey(bool enable);

    /* Brightness */
    bool BrightnessIconEnabled();
    void BrightnessIconEnabled(bool enable);

    /* Hotkeys */
    std::unordered_map<int, HotkeyInfo> Hotkeys();
    void Hotkeys(std::vector<HotkeyInfo> hotkeys);

    bool AutomaticUpdates();
    void AutomaticUpdates(bool enabled);
    void LastUpdateCheckNow();
    void LastUpdateCheck(long long time);
    long long LastUpdateCheck();
    std::wstring IgnoreUpdate();
    void IgnoreUpdate(std::wstring versionString);

    bool ShowOnStartup();
    void ShowOnStartup(bool show);

public:
    /* Static settings methods */

    /// <summary>
    /// Retrieves the location of the current executable. Both the main program
    /// and settings application should be in the same directory, but this
    /// method could return different directories if the executables are moved.
    /// </summary>
    static std::wstring AppDir();

    /// <summary>
    /// Determines whether this installation is portable or not.
    /// </summary>
    static bool Portable();

    /// <summary>
    /// Determines where program settings are stored. If this is an installed
    /// version of the program, then the settings are in
    /// %APPDATA%\3RVX\
    /// <p>
    /// If this is a portable installation of the program, the settings are
    /// stored in the same directory as the program executable.
    /// </summary>
    static std::wstring SettingsDir();

    /// <summary>
    /// Ensures the settings directory has been created. If the directory
    /// already exists, calling this function has no effect.
    /// </summary>
    static void CreateSettingsDir();

    static std::wstring SettingsFile();
    static std::wstring SkinDir();
    static std::wstring LanguagesDir();
    static std::wstring MainApp();
    static std::wstring SettingsApp();
    static void LaunchSettingsApp();


private:
    Settings() {
        /* Don't allow instantiation for this Singleton class; see the
         * Instance() method to retrieve an instance instead. */
    }

    static Settings *instance;
    static std::wstring _appDir;

    std::wstring _file;
    tinyxml2::XMLDocument _xml;
    tinyxml2::XMLElement *_root;

    LanguageTranslator *_translator;

    bool HasSetting(std::string elementName);
    bool GetEnabled(std::string elementName, const bool defaultSetting);
    void SetEnabled(std::string elementName, bool enabled);

    std::wstring GetText(std::string elementName);
    void SetText(std::string elementName, std::string text);
    int GetInt(std::string elementName, const int defaultValue = 0);
    float GetFloat(std::string elementName, const float defaultValue = 0.0f);

    template<typename T> void SetElementValue(
            std::string elementName, T value) {
        tinyxml2::XMLElement *el = GetOrCreateElement(elementName);
        el->SetText(value);
    }

    tinyxml2::XMLElement *GetOrCreateElement(std::string elementName);

private:
    /* XML tag names */
    static constexpr const char *XML_AUDIODEV = "audioDeviceID";
    static constexpr const char *XML_BRIGHTNESSICON = "brightnessIcon";
    static constexpr const char *XML_CAPSLOCK = "capsLock";
    static constexpr const char *XML_CAPTUREDEV = "captureDeviceID";
    static constexpr const char *XML_CURVE_ADJUST = "curveAdjust";
    static constexpr const char *XML_EJECTICON = "ejectIcon";
    static constexpr const char *XML_ENABLE_BOSD = "brightnessOSDEnabled";
    static constexpr const char *XML_ENABLE_EOSD = "ejectOSDEnabled";
    static constexpr const char *XML_ENABLE_KOSD = "keyboardOSDEnabled";
    static constexpr const char *XML_ENABLE_MOSD = "micOSDEnabled";
    static constexpr const char *XML_ENABLE_VOSD = "volumeOSDEnabled";
    static constexpr const char *XML_HIDE_WHENFULL = "hideFullscreen";
    static constexpr const char *XML_HIDE_DIRECTX = "hideDirectX";
    static constexpr const char *XML_HIDEANIM = "hideAnimation";
    static constexpr const char *XML_HIDETIME = "hideDelay";
    static constexpr const char *XML_HIDESPEED = "hideSpeed";
    static constexpr const char *XML_IGNOREUPDATE = "ignoreUpdateVersion";
    static constexpr const char *XML_GLASS = "glassEffects";
    static constexpr const char *XML_KEYBOARDICONS = "keyboardIcons";
    static constexpr const char *XML_LANGUAGE = "language";
    static constexpr const char *XML_MONITOR = "monitor";
    static constexpr const char *XML_MEDIAKEYS = "mediaKeys";
    static constexpr const char *XML_MICICON = "micIcon";
    static constexpr const char *XML_MICMUTELOCK = "micMuteDuringLock";
    static constexpr const char *XML_MUTELOCK = "muteDuringLock";
    static constexpr const char *XML_NUMLOCK = "numLock";
    /* Note: the XML_VOLUMEICON tag name is 'notifyIcon' to ensure backwards
     * compatibility with previous versions of 3RVX that didn't support multiple
     * icon types. */
    static constexpr const char *XML_VOLUMEICON = "notifyIcon";
    static constexpr const char *XML_ONTOP = "onTop";
    static constexpr const char *XML_OPACITY = "opacity";
    static constexpr const char *XML_OSD_OFFSET = "osdEdgeOffset";
    static constexpr const char *XML_OSD_POS = "osdPosition";
    static constexpr const char *XML_OSD_X = "osdX";
    static constexpr const char *XML_OSD_Y = "osdY";
    static constexpr const char *XML_SCROLLLOCK = "scrollLock";
    static constexpr const char *XML_SHOWONSTART = "showOnStartup";
    static constexpr const char *XML_SHOWRUN = "showRun";
    static constexpr const char *XML_SHOW_BOSD = "showBrightnessOSD";
    static constexpr const char *XML_SHOW_EOSD = "showEjectOSD";
    static constexpr const char *XML_SHOW_KOSD = "showKeyboardOSD";
    static constexpr const char *XML_SHOW_MOSD = "showMicOSD";
    static constexpr const char *XML_SHOW_VOSD = "showVolumeOSD";
    static constexpr const char *XML_SKIN = "skin";
    static constexpr const char *XML_SIZE = "size";
    static constexpr const char *XML_SOUNDS = "soundEffects";
    static constexpr const char *XML_SUBSCRIBE_EJECT = "subscribeEjectEvents";
    static constexpr const char *XML_SUBSCRIBE_MIC = "subscribeMicEvents";
    static constexpr const char *XML_SUBSCRIBE_VOL = "subscribeVolumeEvents";
    static constexpr const char *XML_UPDATEAUTO = "automaticUpdates";
    static constexpr const char *XML_UPDATECHECKTIME = "lastUpdateCheck";
    static constexpr const char *XML_VARIANT = "variant";
    static constexpr const char *XML_VOLUME_LIMITER = "volumeLimiter";
};
