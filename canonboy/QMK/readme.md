# Canon Boy

A handwired replica of the Canon Cat keyboard.

* Keyboard Maintainer: [Salavat Abdullin](https://github.com/slvt)
* Hardware Supported: RP2040 Pro Micro

59 keys on a 5×14 matrix. The keymap is configured for the [Canon Cat emulator](https://archive.org/details/canoncat). See [`keymaps/default/keymap.c`](keymaps/default/keymap.c) for the full mapping and [the MAME driver](https://github.com/mamedev/mame/blob/master/src/mame/canon/cat.cpp) for how the original handles its keys.

## Building

Copy this folder into your firmware tree as `keyboards/slvtkeebs/canonboy`.

The `default` keymap builds against plain [QMK](https://github.com/qmk/qmk_firmware):

    qmk compile -kb slvtkeebs/canonboy -km default

The `vial` keymap needs [Vial-QMK](https://github.com/vial-kb/vial-qmk), a fork of QMK:

    git clone https://github.com/vial-kb/vial-qmk.git
    cd vial-qmk
    make git-submodule
    cp -r <this folder> keyboards/slvtkeebs/canonboy
    qmk compile -kb slvtkeebs/canonboy -km vial

Both keymaps share the same layout: `keymaps/vial/keymap.c` includes `keymaps/default/keymap.c`, so the layout is edited in one place.

The resulting `.uf2` is written to the root of the tree you built in.

See the [build environment setup](https://docs.qmk.fm/#/getting_started_build_tools) and the [make instructions](https://docs.qmk.fm/#/getting_started_make_guide) for more information. Brand new to QMK? Start with the [Complete Newbs Guide](https://docs.qmk.fm/#/newbs).

## Unlocking Vial

Hold the top left two keys (matrix positions `[0, 0]` and `[0, 1]`) when Vial asks for physical confirmation.

## Bootloader

Enter the bootloader in one of two ways:

* **Bootmagic reset**: hold the top left key (matrix position `[0, 0]`) and plug the keyboard in
* **BOOT button**: hold the physical `BOOT` button on the RP2040 Pro Micro and plug the keyboard in

Either way the RP2040 appears as a USB drive named `RPI-RP2`. Copy the `.uf2` onto it and the keyboard reboots on its own.

The `BOOT` button is the reliable fallback: it works even if the firmware is broken or the matrix is miswired, because it is handled by the chip itself rather than by QMK. Reaching it means opening the case.
