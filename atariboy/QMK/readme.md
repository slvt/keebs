# Atari Boy

A handwired replica of the Atari 800 keyboard.

* Keyboard Maintainer: [Salavat Abdullin](https://github.com/slvt)
* Hardware Supported: RP2040 Pro Micro

The keycodes are mapped to what the [Atari800](https://atari800.github.io/) emulator expects rather than to the labels on the caps. `SHIFT` and `CONTROL` are momentary layers rather than plain modifiers. See [`keymaps/default/keymap.c`](keymaps/default/keymap.c) for the full mapping.

## Building

Copy this folder into your QMK tree as `keyboards/slvtkeebs/atariboy`.

Make example for this keyboard (after setting up your QMK environment):

    qmk compile -kb slvtkeebs/atariboy -km default

Flashing example for this keyboard:

    qmk flash -kb slvtkeebs/atariboy -km default

See the [build environment setup](https://docs.qmk.fm/#/getting_started_build_tools) and the [make instructions](https://docs.qmk.fm/#/getting_started_make_guide) for more information. Brand new to QMK? Start with the [Complete Newbs Guide](https://docs.qmk.fm/#/newbs).

## Bootloader

Enter the bootloader in one of two ways:

* **Bootmagic reset**: hold `ESC` (matrix position `[0, 0]`) and plug the keyboard in
* **BOOT button**: hold the physical `BOOT` button on the RP2040 Pro Micro and plug the keyboard in

Either way the RP2040 appears as a USB drive named `RPI-RP2`. Copy the `.uf2` onto it and the keyboard reboots on its own.

The `BOOT` button is the reliable fallback: it works even if the firmware is broken or the matrix is miswired, because it is handled by the chip itself rather than by QMK. Reaching it means opening the case.
