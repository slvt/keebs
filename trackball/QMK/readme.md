# Track Ball

A wired trackball with a PMW3389 sensor.

* Keyboard Maintainer: [Salavat Abdullin](https://github.com/slvt)
* Hardware Supported: RP2040 Zero, PMW3389 sensor over SPI

The keymap has a single 1x3 matrix of direct pins, the buttons are handled in [`trackball.c`](trackball.c) rather than in the keymap.

## Wiring

| Part | RP2040 Zero pin |
| --- | --- |
| Sensor SCLK | GP26 (SPI1 SCK) |
| Sensor MOSI | GP27 (SPI1 TX) |
| Sensor MISO | GP28 (SPI1 RX) |
| Sensor NCS | GP29 |
| Sensor VDD | 3V3 |
| Left button | GP6 |
| Middle button | GP7 |
| Right button | GP8 |

Buttons connect the pin to GND. With 2 buttons, the left and right ones are used and the middle one is left out. With 1 button, only the left one is used. Solder only the buttons you have and pick the same number in the Buttons option in Vial. In the Vial layout the buttons are drawn 3U wide for 1 button, 1.5U for 2 and 1U for 3.

Holding the left button while plugging in the USB cable enters the bootloader, so this works with any number of buttons. Vial Unlock (matrix tester only) needs the left and right buttons together, so it is not available with 1 button.

## Trackball Controls

Three buttons:

* Left button: left mouse click, drag DPI applies while it is held, if enabled
* Right button: right mouse click
* Middle button tap: middle click
* Middle button hold: scroll mode
* Middle button held, then the right button on the case (`default` keymap only): next DPI step
* Middle button held, then the left button on the case (`default` keymap only): previous DPI step. The gesture follows the physical position of the buttons, also with swapped keys. The middle button gives no click after a DPI step

Two buttons:

* Left button: left mouse click
* Left button held: pointer switches to the drag DPI value, if drag DPI is enabled
* Right button tap: right mouse click
* Right button hold: scroll mode, trackball movement is converted to vertical scrolling

One button (experimental, left pin only):

* Tap: left mouse click
* Hold while moving the ball: select or drag, drag DPI applies while it is held, if enabled
* No right click, no scroll and no middle click. Left handed mode has no effect


### Keymaps

| | `vial` | `default` |
| --- | --- | --- |
| Settings | in Vial, stored by Vial in EEPROM, survive flashing | fixed in the build, see below |
| Number of buttons | Buttons option in Vial, 1, 2 or 3 | `TB_BUTTONS` in [`keymaps/default/config.h`](keymaps/default/config.h), 3 by default |
| Left handed | Left handed checkbox | swap the first and last key of layer 0 in [`keymaps/default/keymap.c`](keymaps/default/keymap.c), `LAYOUT(TB_RIGHT_BTN, TB_MID_BTN, TB_LEFT_BTN)`, and build it yourself, or flash the prebuilt [`default-left-hand.uf2`](../firmware/default-left-hand.uf2) |
| DPI | DPI option | gesture with 3 buttons, stored in EEPROM |
| Scroll speed and drag DPI | options | fixed, 1/48 and drag DPI off |

The `default` keymap needs no Vial, so settings that Vial would offer are fixed in the build. The DPI step it stores sits in the keyboard EEPROM word together with an id of the build: it survives power off, but a freshly flashed build starts from the default DPI of 1000 again. There is no chord for left handed mode. The gesture follows the physical buttons, so it stays right with swapped keys.

## Layout Options

Options of the `vial` keymap, stored in EEPROM by Vial. Field order matches the `labels` array in [`keymaps/vial/vial.json`](keymaps/vial/vial.json).

| Option | Values | Default |
| --- | --- | --- |
| Buttons | 1, 2, 3 | 3 |
| DPI | 100 to 5000 | 1000 |
| Scroll speed | 1/8 to 1/80 | 1/48 |
| Enable drag DPI | on, off | off |
| Drag DPI | 100 to 5000 | 500 |
| Left handed | on, off | off |

## Building

Copy this folder into your firmware tree as `keyboards/slvtkeebs/trackball`. The `vial` keymap needs [Vial-QMK](https://github.com/vial-kb/vial-qmk), a fork of QMK:

    git clone https://github.com/vial-kb/vial-qmk.git
    cd vial-qmk
    make git-submodule
    cp -r <this folder> keyboards/slvtkeebs/trackball
    qmk compile -kb slvtkeebs/trackball -km vial

The `default` keymap targets plain [QMK](https://github.com/qmk/qmk_firmware) and does not build in Vial-QMK. It reads no options from VIA: the number of buttons is `TB_BUTTONS` in its `config.h`, the DPI is changed by the gesture described in Trackball Controls.

The keymaps share the same layout: `keymaps/vial/keymap.c` includes `keymaps/default/keymap.c`. The `vial` keymap adds `vial.json`, the keyboard UID and the unlock combo.

The resulting `.uf2` is written to the root of the tree you built in.

## Unlocking Vial

Hold the left and right buttons (matrix positions `[0, 0]` and `[0, 2]`) when Vial asks for it.

## Bootloader

* **Bootmagic reset**: hold the left button (matrix position `[0, 0]`) and plug the board in
* **BOOT button**: hold the `BOOT` button on the RP2040 Zero and plug it in

The RP2040 appears as a USB drive named `RPI-RP2`. Copy the `.uf2` onto it and the board reboots on its own.

## Credits

The firmware started from the QMK code of the [HPD keyboard](https://github.com/ergohaven/hpd) by Ergohaven, was modified and stays under GPL-2.0.
