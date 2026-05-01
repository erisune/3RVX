#include "BrightnessSlider.h"

#include "../Controllers/Brightness/BrightnessController.h"
#include "../Error.h"
#include "../Settings.h"
#include "../Skin/Skin.h"
#include "../Skin/SkinManager.h"
#include "../Skin/SliderComponent.h"

BrightnessSlider::BrightnessSlider(BrightnessController &brightnessCtrl) :
SliderWnd(L"3RVX-BrightnessSlider", L"3RVX Brightness Slider"),
_brightnessCtrl(brightnessCtrl) {

    SkinManager *skin = SkinManager::Instance();
    SliderComponent *brightnessSlider = skin->BrightnessSlider();
    if (brightnessSlider == nullptr) {
        return;
    }
    BackgroundImage(brightnessSlider->background);
    EnableGlass(brightnessSlider->mask);

    _knob = brightnessSlider->knob;
    _vertical = _knob->Vertical();

    for (Meter *m : brightnessSlider->meters) {
        AddMeter(m);
    }

    Knob(_knob);
}

void BrightnessSlider::SliderChanged() {
    _brightnessCtrl.Brightness(_knob->Value());
}

void BrightnessSlider::ScrollUp() {
    IncreaseBrightness();
}

void BrightnessSlider::ScrollDown() {
    DecreaseBrightness();
}

void BrightnessSlider::KeyPress(unsigned int vk) {
    if (_visible == false) {
        return;
    }

    switch (vk) {
    case VK_ESCAPE:
    case VK_RBUTTON:
        Hide();
        break;

    case VK_UP:
        if (_vertical) {
            IncreaseBrightness();
        }
        break;

    case VK_DOWN:
        if (_vertical) {
            IncreaseBrightness();
        }
        break;

    case VK_LEFT:
        if (!_vertical) {
            DecreaseBrightness();
        }
        break;

    case VK_RIGHT:
        if (!_vertical) {
            DecreaseBrightness();
        }
        break;
    }
}

void BrightnessSlider::IncreaseBrightness() {
    float level = _brightnessCtrl.Brightness() + SCROLL_INCREMENT;
    _brightnessCtrl.Brightness(level);
}

void BrightnessSlider::DecreaseBrightness() {
    float level = _brightnessCtrl.Brightness() - SCROLL_INCREMENT;
    _brightnessCtrl.Brightness(level);
}

void BrightnessSlider::MeterLevels(float level) {
    if (Visible() && _dragging == false) {
        MeterWnd::MeterLevels(level);
        Update();
    }
    _level = level;
}

void BrightnessSlider::Show() {
    MeterWnd::MeterLevels(_level);
    Update();
    SliderWnd::Show();
    SetActiveWindow(Window::Handle());
    SetForegroundWindow(Window::Handle());
}

bool BrightnessSlider::Visible() {
    return _visible;
}