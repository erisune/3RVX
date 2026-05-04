// Copyright (c) 2015, Matthew Malensek.
// Copyright (c) 2026, erisune.
// Distributed under the GPL-3.0 License (see COPYING for details)s

#include "MeterWnd.h"
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")
#include <VersionHelpers.h>
#include <sstream>

#include "../DisplayManager.h"
#include "../Skin/AccentColor.h"
#include "Animation.h"
#include "AnimationFactory.h"

MeterWnd::MeterWnd(LPCWSTR className, LPCWSTR title, HINSTANCE hInstance) :
LayeredWnd(className, title, hInstance, NULL, WINDOW_STYLES) {

}

MeterWnd::~MeterWnd() {
    delete _hideAnimation;
    delete _composite;
    DeleteClones();
}

void MeterWnd::Update(bool forceUpdate) {
    CLOG(L"Updating meter window");
    using namespace Gdiplus;

    bool dirty = (_composite == NULL);

    if (forceUpdate) {
        dirty = true;
    }

    for (Meter *meter : _meters) {
        if (meter->Dirty() == true) {
            dirty = true;
            break;
        }
    }

    if (dirty) {
        QCLOG(L"Contents have changed; redrawing");

        if (_composite) {
            delete _composite;
        }

        Rect bgRect(0, 0, _background->GetWidth(), _background->GetHeight());
        _composite = _background->Clone(bgRect, PixelFormat32bppARGB);
        Graphics graphics(_composite);

        for (Meter *meter : _meters) {
            CLOG(L"Drawing meter:\n%s", meter->ToString().c_str());
            meter->Draw(_composite, &graphics);
        }

        float size = static_cast<float>(_settings->Size()) / 100.0f;
        if (size != 1.0f) {
            unsigned int width = static_cast<UINT>(_composite->GetWidth() * size);
            unsigned int height = static_cast<UINT>(_composite->GetHeight() * size);
            Gdiplus::Bitmap *resized = static_cast<Gdiplus::Bitmap*>(_composite->GetThumbnailImage(width, height));
            delete _composite;
            _composite = resized;
        }
    }

    Bitmap(_composite);
    UpdateClones();
}

void MeterWnd::SetMeters(std::list<Meter*> meters) {
    _meters.assign(meters.begin(), meters.end());
}

void MeterWnd::AddMeter(Meter *meter) {
    _meters.push_back(meter);
}

const std::list<Meter*>& MeterWnd::Meters() {
    return _meters;
}

void MeterWnd::MeterLevels(float value) {
    for (Meter *meter : _meters) {
        meter->Value(value);
    }
}

void MeterWnd::HideAnimation(AnimationTypes::HideAnimation anim, int speed) {
    delete _hideAnimation;
    _hideAnimation = AnimationFactory::Create(anim, speed);
}

void MeterWnd::VisibleDuration(int duration) {
    _visibleDuration = duration;
}

void MeterWnd::BackgroundImage(Gdiplus::Bitmap *background) {
    _background = background;
}

bool MeterWnd::EnableGlass(Gdiplus::Bitmap *mask) {
    bool result = LayeredWnd::EnableGlass(mask);
    ApplyClonesGlass();
    if (!_settings->GlassEffectsEnabled()) {
        result = DrawBackdrop(_glassMask);
    }
    return result;
}

bool MeterWnd::DrawBackdrop(Gdiplus::Bitmap *mask) {
    if (mask == NULL) {
        return false;
    }
    /* Saving in case of accent color updates */
    if (_buffer == NULL) {
        _buffer = _background;
    } else {
        delete _background;
        _background = _buffer;
    }
    HRGN maskRegion = GetMaskRegion(mask);
    Gdiplus::Region region(maskRegion);
    DeleteObject(maskRegion);
    Gdiplus::Rect rect(0, 0, mask->GetWidth(), mask->GetHeight());
    Gdiplus::Bitmap *backdrop = mask->Clone(rect, PixelFormat32bppARGB);
    Gdiplus::Graphics g(backdrop);
    g.Clear(Gdiplus::Color::Transparent);

    UINT32 accentColor;
    if (_settings->UseAccentColor()) {
        /* Get accent color for backdrop */
        AccentColor::Instance()->Refresh();
        accentColor = AccentColor::Instance()->Color();
    } else {
        accentColor = 0xFF0A0A0A;
    }
    UINT8 alpha = _settings->Opacity();

    /* Modify alpha based on user settings */
    alpha &= 0x80;
    accentColor = (accentColor & 0x00FFFFFF) | (alpha << 24);
        
    Gdiplus::Color color = Gdiplus::ARGB(accentColor);
    Gdiplus::SolidBrush brush(accentColor);
    g.FillRegion(&brush, &region);
    g.DrawImage(_background, rect);
    _background = backdrop->Clone(rect, PixelFormat32bppARGB);
    delete backdrop;
    return true;
}

void MeterWnd::Show(bool animate) {
    if (_visible == false) {
        UpdateWindowPosition();

        bool disabled = false;
        if (_disableFullscreen
                && DisplayManager::IsFullscreen(Window::Handle())) {
            CLOG(L"not showing (fs)");
            disabled = true;
        }

        if (_disableDirectX
                && _d3dDevice->Occluded()) {
            CLOG(L"not showing (occluded)");
            disabled = true;
        }


        if (disabled == false) {
            ShowWindow(Window::Handle(), SW_SHOW);
            _visible = true;
        }
    }

    ShowClones();

    if (_visibleDuration > 0) {
        SetTimer(Window::Handle(), TIMER_HIDE, _visibleDuration, NULL);
        KillTimer(Window::Handle(), TIMER_OUT);

        if (_hideAnimation) {
            _hideAnimation->Reset(this);
        }
    }
}

void MeterWnd::Hide(bool animate) {
    if (_visible == false) {
        return;
    }

    if (animate && _hideAnimation) {
        SetTimer(Window::Handle(),
            TIMER_OUT, _hideAnimation->UpdateInterval(), NULL);
    } else {
        ShowWindow(Window::Handle(), SW_HIDE);
        _visible = false;
        HideClones();
    }
}

void MeterWnd::AnimateOut() {
    bool animOver = _hideAnimation->Animate(this);
    if (animOver) {
        CLOG(L"Finished hide animation.");
        KillTimer(Window::Handle(), TIMER_OUT);
        ShowWindow(Window::Handle(), SW_HIDE);
        _visible = false;
        HideClones();
    }
}

byte MeterWnd::Transparency() {
    return LayeredWnd::Transparency();
}

void MeterWnd::Transparency(byte transparency) {
    LayeredWnd::Transparency(transparency);
    UpdateClonesTransparency(transparency);
}

LayeredWnd *MeterWnd::Clone() {
    size_t numClones = _clones.size() + 1;
    std::wstringstream cloneClass;
    cloneClass << Window::ClassName() << L":" << numClones;
    std::wstringstream cloneTitle;
    cloneTitle << Window::Title() << L":" << numClones;

    LayeredWnd *clone = new LayeredWnd(
        cloneClass.str().c_str(),
        cloneTitle.str().c_str(),
        Window::InstanceHandle(),
        _composite,
        GetWindowLong(Window::Handle(), GWL_EXSTYLE));

    if (_glassMask) {
        clone->EnableGlass(_glassMask);
    }

    _clones.push_back(clone);
    CLOG(L"Created meter window clone: %s/%s",
        cloneClass.str().c_str(), cloneTitle.str().c_str());
    return clone;
}

std::vector<LayeredWnd *> MeterWnd::Clones() {
    return _clones;
}

void MeterWnd::DeleteClones() {
    for (LayeredWnd *clone : _clones) {
        delete clone;
    }
    _clones.clear();
}

void MeterWnd::UpdateClones() {
    for (LayeredWnd *clone : _clones) {
        clone->Bitmap(_composite);
    }
}

void MeterWnd::UpdateClonesTransparency(byte transparency) {
    for (LayeredWnd *clone : _clones) {
        clone->Transparency(transparency);
    }
}

void MeterWnd::ShowClones() {
    for (LayeredWnd *clone : _clones) {
        if (_disableFullscreen
                && DisplayManager::IsFullscreen(clone->Handle())) {
            continue;
        }
        if (_disableDirectX
                && DisplayManager::Direct3DOccluded(clone->Handle())) {
            continue;
        }

        clone->Show();
    }
}

void MeterWnd::HideClones() {
    for (LayeredWnd *clone : _clones) {
        clone->Hide();
    }
}

void MeterWnd::ApplyClonesGlass() {
    if (_glassMask == NULL) {
        return;
    }

    for (LayeredWnd *clone : _clones) {
        clone->EnableGlass(_glassMask);
    }
}

LRESULT MeterWnd::WndProc(
        HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

    if (message == WM_TIMER) {
        switch (wParam) {
        case TIMER_HIDE:
            CLOG(L"Display duration has elapsed. Hiding window.");
            Hide();
            KillTimer(hWnd, TIMER_HIDE);
            break;

        case TIMER_OUT:
            AnimateOut();
            break;
        }
    } else if (message == WM_DWMCOLORIZATIONCOLORCHANGED) {
        if (_settings->UseAccentColor()) {
            CLOG(L"updating meter color maps");
            AccentColor::Instance()->Refresh();
            UINT32 color = AccentColor::Instance()->Color();
            if (!_settings->GlassEffectsEnabled()) {
                DrawBackdrop(_glassMask);
            }
            for (Meter *m : _meters) {
                if (m->HasColorTransformMatrix()) {
                    m->UpdateColorTransformMatrix(color);
                }
                else if (m->HasColorTransform()) {
                    m->UpdateColorTransform(color);
                }
            }
        }
    }

    return LayeredWnd::WndProc(hWnd, message, wParam, lParam);
}

