#pragma once

#include "../Controls/Dialog.h"
#include "../resource.h"

#include <string>

class Button;
class ComboBox;
class EditBox;
class GroupBox;
class Label;
class Spinner;
class LanguageTranslator;

class SkinOptions : public Dialog {
public:
    SkinOptions(HWND parent, std::wstring skinName);
    
private:
    /* Controls */
    GroupBox *_variantGroup;
    ComboBox *_variant;
    Label *_variantAuthor;

    GroupBox *_propertiesGroup;
    Label *_opacityLabel;
    EditBox *_opacity;
    Spinner *_opacitySpinner;
    Label *_sizeLabel;
    EditBox *_size;
    Spinner *_sizeSpinner;

    Button *_ok;
    Button *_cancel;

    std::wstring _skinName;
    void Initialize();
    void LoadSettings();
    void SaveSettings();
    bool LoadVariants(std::wstring skinName);
    void LoadVariantInfo(std::wstring variantName);
    std::list<std::wstring> FindVariants(std::wstring dir);
};