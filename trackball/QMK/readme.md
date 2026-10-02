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

Buttons connect the pin to GND. With 2 buttons, the left and right ones are used and the middle one is left out.

## Trackball Controls

Two buttons:

* Left button: left mouse click
* Left button held: pointer switches to the drag DPI value, if drag DPI is enabled
* Right button tap: right mouse click
* Right button hold: scroll mode, trackball movement is converted to vertical scrolling

Three buttons:

* Left button: left mouse click, drag DPI applies while it is held, if enabled
* Right button: right mouse click
* Middle button tap: middle click
* Middle button hold: scroll mode, or press any other button while holding it
* Left and right together: toggles left handed mode, which swaps left and right

Left and right wait 50 ms for each other so the chord can be told from two clicks. Change `TB_CHORD_TERM` in `config.h` to tune it.

## Layout Options

Exposed through VIA and Vial, stored in EEPROM. Field order matches the `labels` array in [`keymaps/vial/vial.json`](keymaps/vial/vial.json).

| Option | Values | Default |
| --- | --- | --- |
| DPI | 100 to 5000 | 1000 |
| Scroll speed | 1/8 to 1/80 | 1/48 |
| Enable drag DPI | on, off | off |
| Drag DPI | 100 to 5000 | 500 |
| Buttons | 2, 3 | 3 |
| Left handed | on, off | off |

## Building

Copy this folder into your firmware tree as `keyboards/slvtkeebs/trackball`. The `vial` keymap needs [Vial-QMK](https://github.com/vial-kb/vial-qmk), a fork of QMK:

    git clone https://github.com/vial-kb/vial-qmk.git
    cd vial-qmk
    make git-submodule
    cp -r <this folder> keyboards/slvtkeebs/trackball
    qmk compile -kb slvtkeebs/trackball -km vial

The `default` keymap targets plain [QMK](https://github.com/qmk/qmk_firmware) with VIA and does not build in Vial-QMK.

Both keymaps share the same layout: `keymaps/vial/keymap.c` includes `keymaps/default/keymap.c`. The `vial` keymap adds `vial.json`, the keyboard UID and the unlock combo.

The resulting `.uf2` is written to the root of the tree you built in.

## Unlocking Vial

Hold the left and right buttons (matrix positions `[0, 0]` and `[0, 2]`) when Vial asks for it.

## Bootloader

* **Bootmagic reset**: hold the left button (matrix position `[0, 0]`) and plug the board in
* **BOOT button**: hold the `BOOT` button on the RP2040 Zero and plug it in

The RP2040 appears as a USB drive named `RPI-RP2`. Copy the `.uf2` onto it and the board reboots on its own.

## Credits

The firmware started from the QMK code of the [HPD keyboard](https://github.com/ergohaven/hpd) by Ergohaven, was modified and stays under GPL-2.0.
