# Small Boy

ZMK config for the [Small Boy](..), a 51 key wireless handwired keyboard.

* Keyboard Maintainer: [Salavat Abdullin](https://github.com/slvt)
* Hardware Supported: nice!nano v2
* Shield name: `smallboy`

51 keys on a 4×14 matrix, `col2row`. No number row, the layout is built around MT3 Operator keycaps.

## Layers

| Layer | Name | Held with | Contents |
| --- | --- | --- | --- |
| 0 | default | base | |
| 1 | layer_1 | `Fn`, the bottom right key of the third row | number row and Caps Lock |
| 2 | layer_2 | `Fn` and `Up` together | F1 to F12 |
| 3 | layer_3 | `Tab` | Bluetooth profiles and bootloader |

`Tab` is a hold tap: tap it for a tab, hold it for layer 3.

See [`boards/shields/smallboy/smallboy.keymap`](boards/shields/smallboy/smallboy.keymap) for the full mapping.

## Building

A ready to flash binary for the default keymap is in [`../Firmware`](../Firmware), so you only need to build if you want to change something.

ZMK builds through GitHub Actions rather than on your own machine. This folder is a complete ZMK config, so you do not need anything else from this repository:

1. Create a new repository on GitHub and copy the contents of this folder into its root.
2. Push. The workflow in [`.github/workflows/build.yml`](.github/workflows/build.yml) runs on every push.
3. Open the **Actions** tab, pick the finished run, and download the `firmware` artifact. The `.uf2` is inside.

To change the layout, edit [`boards/shields/smallboy/smallboy.keymap`](boards/shields/smallboy/smallboy.keymap) and push again. The board and shield combination is set in [`build.yaml`](build.yaml). See the [ZMK documentation](https://zmk.dev/docs) for the available behaviors.

## Bootloader

Enter the bootloader in one of two ways:

* **Reset button**: double tap the reset button on the nice!nano
* **Keymap**: hold `Tab` and press `B`

The nice!nano appears as a USB drive named `NICENANO`. Copy the `.uf2` onto it and the keyboard reboots on its own.

## ZMK Studio

The firmware is built with [ZMK Studio](https://zmk.dev/docs/features/studio) support, so the keymap can be changed at runtime without flashing anything.

Connect the keyboard over USB, open [zmk.studio](https://zmk.studio/), and hold `Tab` and `Z` to unlock it for editing.

Once you start managing the keymap through Studio, further edits to `smallboy.keymap` no longer take effect. The layout lives in the keyboard's own memory from that point on. To go back to the file, use **Restore Stock Settings** in the Studio interface.

## Bluetooth

Hold `Tab` for the profile controls: the five profiles on `Q` through `T`, and `BT_CLR` on `Bksp`.
