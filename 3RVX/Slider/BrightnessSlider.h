#pragma once

#include "SliderWnd.h"

class BrightnessController;
class Settings;
class SliderKnob;

class BrightnessSlider : public SliderWnd {
public:
    BrightnessSlider(BrightnessController &volumeCtrl);

    virtual void Show();
    bool Visible();
    void MeterLevels(float level);

protected:
    void SliderChanged();
    void ScrollUp();
    void ScrollDown();
    void KeyPress(unsigned int vk);

    void IncreaseBrightness();
    void DecreaseBrightness();

private:
    SliderKnob *_knob;
    float _level;
    BrightnessController &_brightnessCtrl;

    static constexpr const float SCROLL_INCREMENT = 0.05f;
};
