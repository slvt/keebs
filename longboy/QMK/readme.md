# Long Boy

A handwired 64-key keyboard based on the Planck, with extra columns for a full Cyrillic layout and a numpad.

* Keyboard Maintainer: [Salavat Abdullin](https://github.com/slvt)
* Hardware Supported: RP2040

## Layers

Two layers. Holding the `Fn` key (next to the arrow cluster) switches to `_FN`: function keys, media and brightness controls, `` ` `` and Caps Lock.

## Combos

Combo support is enabled but nothing is pre-configured. Add any through `key_combos[]` in `keymap.c` for the `default` keymap, or live through the Combo tab in Vial for the `vial` keymap.

## Building

Copy this folder into your firmware tree as `keyboards/slvtkeebs/longboy`.

The `default` keymap builds against plain [QMK](https://github.com/qmk/qmk_firmware) and gives you VIA support:

    qmk compile -kb slvtkeebs/longboy -km default

The `vial` keymap needs [Vial-QMK](https://github.com/vial-kb/vial-qmk), a fork of QMK, and gives you Vial support on top of VIA:

    git clone https://github.com/vial-kb/vial-qmk.git
    cd vial-qmk
    make git-submodule
    cp -r <this folder> keyboards/slvtkeebs/longboy
    qmk compile -kb slvtkeebs/longboy -km vial

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
