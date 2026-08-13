# Track Boy

A handwired TKL keyboard with an integrated PMW3389 trackball.

* Keyboard Maintainer: [Salavat Abdullin](https://github.com/slvt)
* Hardware Supported: RP2040 Pro Micro, PMW3389 sensor over SPI

Two layers: base and `FN`. The trackball has two dedicated buttons next to the arrow cluster, wired as ordinary matrix positions and handled in [`trackboy.c`](trackboy.c) rather than in the keymap, so editing the keymap cannot break them.

## Trackball Controls

* Left button: left mouse click
* Left button held: pointer switches to the drag DPI value, if drag DPI is enabled
* Right button tap: right mouse click
* Right button hold: scroll mode, trackball movement is converted to vertical scrolling

## Layout Options

Exposed through VIA and Vial, stored in EEPROM. Field order matches the `labels` array in [`keymaps/vial/vial.json`](keymaps/vial/vial.json).

| Option | Values | Default |
| --- | --- | --- |
| DPI | 100 to 5000 | 1000 |
| Scroll speed | 1/8 to 1/80 | 1/48 |
| Enable drag DPI | on, off | off |
| Drag DPI | 100 to 5000 | 500 |

## Building

Copy this folder into your firmware tree as `keyboards/slvtkeebs/trackboy`.

The `default` keymap builds against plain [QMK](https://github.com/qmk/qmk_firmware) and gives you VIA support:

    qmk compile -kb slvtkeebs/trackboy -km default

The `vial` keymap needs [Vial-QMK](https://github.com/vial-kb/vial-qmk), a fork of QMK, and gives you Vial support on top of VIA:

    git clone https://github.com/vial-kb/vial-qmk.git
    cd vial-qmk
    make git-submodule
    cp -r <this folder> keyboards/slvtkeebs/trackboy
    qmk compile -kb slvtkeebs/trackboy -km vial

Both keymaps share the same layout: `keymaps/vial/keymap.c` includes `keymaps/default/keymap.c`, so the layout is edited in one place. The `vial` keymap adds `vial.json`, the keyboard UID and the unlock combo.

The resulting `.uf2` is written to the root of the tree you built in.

See the [build environment setup](https://docs.qmk.fm/#/getting_started_build_tools) and the [make instructions](https://docs.qmk.fm/#/getting_started_make_guide) for more information. Brand new to QMK? Start with the [Complete Newbs Guide](https://docs.qmk.fm/#/newbs).

## Unlocking Vial

Vial keeps the security-sensitive settings locked until you prove physical access to the keyboard. Hold `ESC` and `1` (matrix positions `[0, 0]` and `[0, 1]`) when Vial asks for it.

## Bootloader

Enter the bootloader in one of two ways:

* **Bootmagic reset**: hold `ESC` (matrix position `[0, 0]`) and plug the keyboard in
* **BOOT button**: hold the physical `BOOT` button on the RP2040 Pro Micro and plug the keyboard in

Either way the RP2040 appears as a USB drive named `RPI-RP2`. Copy the `.uf2` onto it and the keyboard reboots on its own.

The `BOOT` button is the reliable fallback: it works even if the firmware is broken or the matrix is miswired, because it is handled by the chip itself rather than by QMK. Reaching it means opening the case.

## Credits

This trackball housing and firmware was inspired by and initially prototyped from the [HPD keyboard](https://github.com/ergohaven/hpd). Thanks to Ergohaven for publishing their work as open source!

The firmware started from their QMK code, was modified and stays under GPL-2.0.
