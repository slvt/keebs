# Tiny Boy

A handwired 3x10 ortholinear keyboard, inspired by the [ScottoLong](https://github.com/joe-scotto/scottokeebs/tree/main/ScottoLong).

* Keyboard Maintainer: [Salavat Abdullin](https://github.com/slvt)
* Hardware Supported: RP2040

## Layers

Four layers. Holding the space bar switches to `_FN`: numbers on the top row, function keys and punctuation on the two rows below. From `_FN`, holding the left key (tap: Cmd) switches to `_LOWER`, and holding the right key (tap: Opt) switches to `_RAISE`. Both `_LOWER` and `_RAISE` are empty, ready to be configured through Vial.

## Combos

The `default` keymap has two combos built in:

* `Q` and `W` together send `ESC`
* `O` and `P` together send `BACKSPACE`

The `vial` keymap ships with no combos: Vial owns the combo table for live editing, which conflicts with a compiled-in one. Add the same two (or any others) through the Combo tab in Vial after flashing.

## Building

Copy this folder into your firmware tree as `keyboards/slvtkeebs/tinyboy`.

The `default` keymap builds against plain [QMK](https://github.com/qmk/qmk_firmware) and gives you VIA support:

    qmk compile -kb slvtkeebs/tinyboy -km default

The `vial` keymap needs [Vial-QMK](https://github.com/vial-kb/vial-qmk), a fork of QMK, and gives you Vial support on top of VIA:

    git clone https://github.com/vial-kb/vial-qmk.git
    cd vial-qmk
    make git-submodule
    cp -r <this folder> keyboards/slvtkeebs/tinyboy
    qmk compile -kb slvtkeebs/tinyboy -km vial

Both keymaps share the same layout: `keymaps/vial/keymap.c` includes `keymaps/default/keymap.c`, so the layout is edited in one place. The `vial` keymap adds `vial.json`, the keyboard UID and the unlock combo.

The resulting `.uf2` is written to the root of the tree you built in.

See the [build environment setup](https://docs.qmk.fm/#/getting_started_build_tools) and the [make instructions](https://docs.qmk.fm/#/getting_started_make_guide) for more information. Brand new to QMK? Start with the [Complete Newbs Guide](https://docs.qmk.fm/#/newbs).

## Unlocking Vial

Vial keeps the security-sensitive settings locked until you prove physical access to the keyboard. Hold `Q` and `W` (matrix positions `[0, 0]` and `[0, 1]`) when Vial asks for it.

## Bootloader

Enter the bootloader in one of two ways:

* **Bootmagic reset**: hold `Q` (matrix position `[0, 0]`) and plug the keyboard in
* **BOOT button**: hold the physical `BOOT` button on the RP2040 and plug the keyboard in

Either way the RP2040 appears as a USB drive named `RPI-RP2`. Copy the `.uf2` onto it and the keyboard reboots on its own.

The `BOOT` button is the reliable fallback: it works even if the firmware is broken or the matrix is miswired, because it is handled by the chip itself rather than by QMK. Reaching it means opening the case.

## Credits

The layout is inspired by the [ScottoLong](https://github.com/joe-scotto/scottokeebs/tree/main/ScottoLong) by Joe Scotto.
