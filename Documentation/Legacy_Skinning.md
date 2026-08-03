> [!NOTE]
> This guide is kept here only for legacy reasons like understanding and modifying skins
> in the V2 format. To fully take advantage of 3RVX features, users should make new skins in the [V3
> format](Skinning.md) whenever possible.

3RVX Skinning Guide (V2)
------------------------

A skin consists of a skin.xml file and several png images.  Skins are arranged
with the following structure:

> **Skin Name/** \
> ├─ Control/ \
> ├─ Notification Icons/ \
> ├─ OSD/ \
> └─ skin.xml

In my opinion, the best way to learn 3RVX skinning is to go check out the
included skins to see how they work.  The xml format may seem daunting at
first, but it's basically just a bunch of settings you'll need to edit.

Let's look at each folder and what they contain.

OSD
----
* **`back.png`** - the OSD's main background
* **`meter.png`** - shows the volume level.
* **`eject.png`** - shown when a drive ejects
* **`mute.png`** - shown when the volume is muted
* **`glassMask.png`** (optional) - where Vista glass should be drawn (black = glass)

Notification Icons
----
* `0.ico` - shown when the volume is muted
* `X.ico` - where X is a number starting with 1 and continuing for as many icons
          needed.  1.ico represents 0% volume and the highest number represents
          100% volume.

Control
----
* `back.png` - the control meter's background
* `knob.png` - the control knob that can be dragged to change the volume
* `glassMask.png` (optional) - where Vista glass should be drawn (black = glass)

skin.xml format
----
skin.xml tells 3RVX how to draw your images on screen.

**`<info>`**
* `<author>`        - the author of the skin
* `<website>`       - the author's website. *Must* start with `http://`
* `<skinVersion>`   - the skin's version number (can be whatever you want)
* `<formatVersion>` - lets 3RVX know what skin format to expect (currently "2")

**`<osd>`**
* `<meterOrientation>` [`horizontal`|`vertical`|`bitstrip`] 
    * how the skin's meter (volume bar) should be drawn.  Types:
      - `horizontal` : gets larger from left to right
      - `vertical`   : gets larger from bottom to top
      - `bitstrip`   : allows full control -- you draw each step of the meter.
* `<meterPosition>`
    * `<X>` - meter's x-coordinate
    * `<Y>` - meter's y-coordinate
* `<meterMax>` - how many segments your meter will be split into
* `<fadeStepping>` - used in older versions to control the hide delay. This is now controlled in the program's settings, so this tag is ignored.
* `<drawPercentage>` [`true`|`false`]
    * when set to true, text will be drawn with the current volume percentage.
* `<font>` - used if drawPercentage is turned on.
    * `<fontName>` font face name
    * `<fontStyle>` [`normal`|`bold`|`italic`]
    * `<fontColor>` (alpha, red, green, blue)
    * `<fontSize>` number
    * `<fontLocation>`
      - `<X>` - font's x-coordinate
      - `<Y>` - font's y-coordinate

**`<control>`**
* `<orientation>` [`horizontal`|`vertical`]
    * which way the meter slides
* `<sliderPosition>`
    * `<Location>`
      - `<X>` - slider's x-coordinate
      - `<Y>` - slider's y-coordinate
    * `<Size>`
      - `<Width>`  - slider's width
      - `<Height>` - slider's height

*******************************************************************************
(c) 2008 Matthew Malensek

http://matt.malensek.net/
*******************************************************************************