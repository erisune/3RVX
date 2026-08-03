Skinning 3RVX
=============

3RVX allows for its interface to be changed through customizable skins. Each part of the user interface can be skinned, including:
* The on-screen displays (OSDs): Volume, Microphone, Brightness, etc.
* Control sliders
* The system notification icons (system tray)
* Sound effects

XML Format
----------

Directory Structure
-------------------
Unlike the V2 format, the directory structure of V3 skins is a little more flexible. The only reserved files and folders are:

**Skin Name/** \
├─ Variants/ \
└─ skin.xml

Having a `skin.xml` file present is absolutely necessary for the folder to be considered a 3RVX skin, and any variants relative to the skin should always be included inside the `Variants` folder.

**`<skin>`** - Needed for skin definition
* `<info>`
  * `<author>` - The skin author's name
  * `<website>` - A link to the skin author's website

**Example**
```xml
<skin>
  <info>
    <author>Skin Author</author>
    <website>https://example.com</website>
  </info>
  <osds>
    <!-- ... -->
  </osds>
  <sliders>
    <!-- ... -->
  </sliders>
</skin>
```

About Variants
--------------

**Variant Name/** \
└─ variant.xml

Variants are meant for recolors or similar versions of a skin that share a single `skin.xml`. For a folder to be considered a Variant, it should have a `variant.xml` file, with only the following tags:

**`<variant>`** - Needed for variant definition
* `<info>`
  * `<author>` - The variant author's name
  * `<website>` - A link to the variant author's website

**Example**
```xml
<variant>
  <info>
    <author>Variant Author</author>
    <website>https://example.com</website>
  </info>
</variant>
```

The file structure of variants should be laid out exactly the same as the original skin they're overriding. For example, if a skin has its volume OSD background located in `OSD/back.png`, the variant should have its background in `Variant Name/OSD/back.png`.

OSD
---
The OSD (on-screen display) is the visual indicator that shows the status or value of its corresponding setting. OSDs can be of the following types:

* `<volume>` - Playback volume OSD
* `<mute>` - Mute playback OSD, displayed when volume is 0% or muted
* `<mic>` - Microphone volume OSD
* `<micmute>` - Mute microphone OSD, displayed when microphone volume is 0% or muted
* `<brightness>` - Brightness OSD
* `<eject>` - Eject OSD, displayed on device eject or media removal
* `<keyboard>` - Keyboard OSD

All of these should be nested inside of the `<osds>` tag.

**Example**
```xml
<osds>
  <volume background="OSD\canvas.png"
          mask="OSD\glass.png" defaultUnits="57">
    <meter type="Image" image="OSD\Images\volume.png" x="20" y="16"/>
    <meter type="Image" image="OSD\bar.png" x="63" y="30"/>
    <meter type="HorizontalBar" image="OSD\meter.png"
          x="65" y="32" units="57"
          colorTransform="01D428"
          colorTransformMatrix="true"/>
  </volume>
</osds>
```

OSD Attributes
--------------
* `background` - the OSD's main background, consider this as the canvas on top of which all the meters will be drawn.
* `mask` - a black `#000000` and white `#ffffff` image, where all the black pixels will be converted into the glass region of the OSD.
* `defaultUnits` - the default number of units that the meters will have. This value affects the default increments or decrements of the corresponding controls.

Meters
------
Meters are images drawn on top of the OSD background, most of them hold information about the status of the corresponding setting.

They can be of the following types:
`Bitstrip`, `HorizontalBar`, `HorizontalEndCap`, `HorizontalSliderKnob`, `HorizontalTile`, `Image`, `NumberStrip`, `Text`, `VerticalBar`, `VerticalSliderKnob`, `VerticalTile`.

Meter Attributes
----------------
* `image` - The image path relative to the skin folder
* `x` - The x coordinate relative to the OSD background
* `y` - The y coordinate relative to the OSD background
* `units` - [0-100, default 10] The amount of segments that the image resource will be split into. Should be a multiple of `defaultUnits` depending on the meter type.
* `width` - Full width of the meter, ignored by most meter types (Works in `HorizontalSliderKnob`, `VerticalSliderKnob` and `Text`)
* `height` - Full height of the meter, ignored by most meter types (Ditto)
* `colorTransform` - The color that will be converted into the user's accent color (e.g. "FF0000")
* `colorTransformMatrix` - [`true`|`false`] Use a color matrix to also transform similar colors. Useful if you want to transform a gradient, for example.
* `useSystemScheme` - [`true`|`false`] Allows replacing colors with the system color scheme.
  * Colors that can be replaced by their corresponding system color are: `sysColorAppWorkspace`, `sysColorHighlight`, `sysColorWindow`, `sysColorWindowFrame`, `sysColor3DFace`, `sysColor3DHighlight`, `sysColor3DLight`, `sysColor3DDkShadow`, `sysColor3DShadow`.
  * The value assigned to these color attributes should be in hex format, as seen above.

`Text` only:
* `format` - Text content. Text meters can display the value of the OSD by using the `[[PERC]]` string (e.g. `volume: [[PERC]]%`)
* `color` - Font color in hex format
* `font` - Font face name (e.g. "Arial")
* `size` - Font size in pixels
* `align` - Text alignment, can be `left`, `right` or `center`
* `transparency` - Alpha value for the text (0-255)
* `glow` - Glow value for the text, small values recommended
* `style` - Text styles, can include `bold`, `italic`, `underline` and `strike`, separated by whitespaces
* `useSystemScheme` - [`true`|`false`] Use the system scheme's font face and color. Will override `font` and `color`.

Slider
------
Sliders are controls that allow the user to change their corresponding setting (volume/brightness) and are triggered by clicking on the notification icon. Not all skins use sliders, as they're not mandatory for a skin to work properly.

There are two types of sliders: `volume` and `brightness`, and two orientation types: `horizontal` and `vertical`.

Slider Attributes
-----------------
### Slider
* `background` - The image of the slider background, relative to the skin folder.
* `mask` - a black `#000000` and white `#ffffff` image, where all the black pixels will be converted into the glass region of the slider.

### Knobs
* `image` - The image of the slider knob, relative to the skin folder
* `type` - [`horizontal`|`vertical`] The orientation of the slider
* `x` - The X coordinate of the slider knob relative to the slider background
* `y` - The Y coordinate of the slider knob relative to the slider background
* `width` - The full width of the slider
* `height` - The full height of the slider

**Example**
```xml
<sliders>
  <volume background="Slider\slider.png" mask="Slider\glass.png">
    <slider type="horizontal" image="Slider\knob.png"
            x="10" y="8" width="76" height="13"/>
  </volume>
</sliders>
```

Notification Icons
------------------
Each OSD has a corresponding notification area icon (tray icon), which can be enabled or disabled at the user's leisure. These icons can be customized with the `iconset` tag and `location` attribute, or `icon` tag and `file` attribute for OSDs that require single icons.

**Example**
```xml
<iconset location="Icons\Volume"/>
```

```xml
<icon file="Icons\Keyboard\Caps.ico"/>
```

Sound Effects
-------------
Sound effects are used as feedback to inform the user on a setting change, if they have them enabled. They are usually named `sound.wav`, but 3RVX allows the skin author to use different names and different sounds for each OSD if they prefer.

The OSDs that currently support sound effects are: `volume`, `mic` and `brightness`

**Example**
```xml
<!-- ... -->
    <sound file="sounds/ding.wav"/>
  </volume>
</osds>
```
