# Track Ball Wireless

ZMK config for the wireless version of the [Track Ball](..), a handwired trackball with a PMW3389 sensor and three buttons.

* Keyboard Maintainer: [Salavat Abdullin](https://github.com/slvt)
* Hardware Supported: nice!nano v2 with a PixArt optical sensor module
* Shield name: `trackball_wireless` (right handed) or `trackball_wireless_left` (left handed)

No matrix at all. Three buttons wired straight to GPIO, read by `zmk,kscan-gpio-direct`. Movement comes from a `zmk,input-listener` on the sensor node, so orientation, DPI and scroll behaviour all live in the overlay rather than the keymap.

The sensor is a PMW3389 on SPI (the module is sold as PMW3360/3389, the chip on it decides). It is driven by [`drivers/input/input_pmw33xx.c`](drivers/input/input_pmw33xx.c), a modified copy of [`george-norton/zmk-driver-pmw3360`](https://github.com/george-norton/zmk-driver-pmw3360), MIT licensed, the original notice is kept in the file headers and in `drivers/input/LICENSE-zmk-driver-pmw3360`. Changes against the original:

* PMW3389 support, selected with `compatible = "pixart,pmw3389"`. The CPI goes into a different pair of registers and the step is 50 instead of 100. `pixart,pmw3360` still works
* report rate limit, `report-interval-ms` (default 8). The original sends one HID report per sensor read, hundreds a second, which a Bluetooth link drops, and the cursor stutters. Now the motion interrupt is held off for that long after each report and the sensor adds the movement up in between
* polling mode removed, it cast a plain `k_work` to `k_work_delayable` and corrupted memory

The driver does not load the sensor SROM firmware and no PixArt firmware is shipped in this repository. The sensor runs on its built in firmware, which is enough for tracking.

Its DPI property in devicetree is `cpi`, not `resolution`.

## Buttons

Three buttons, nothing else. Positions in `zmk,kscan-gpio-direct` order:

| Position | Button | Pin |
| --- | --- | --- |
| 0 | left | D7, P0.11 |
| 1 | middle | D8, P1.04 |
| 2 | right | D9, P1.06 |

All three are active low to GND with the internal pull-up, no external resistors.

## What each press does

| Press | Result |
| --- | --- |
| left tap | left click (right click in the left handed build) |
| right tap | right click (left click in the left handed build) |
| middle tap | middle click |
| middle hold | scroll mode, held. Movement becomes vertical scroll, buttons below apply |
| middle hold, then left | DPI down one step |
| middle hold, then right | DPI up one step |
| left held while plugging USB in | bootloader, see Bootloader below |

The middle button is a hold tap. Tap gives a middle click, holding it enters scroll for as long as you keep it down.

## Changing hand

There are two builds from one keymap: `trackball_wireless` is right handed, `trackball_wireless_left` is left handed, with left and right swapped. Flash the one for your hand. To change hand later, flash the other build. There is no combo to switch it on the fly, that is what keeps every click instant.

The left build is [`trackball_wireless_left.keymap`](boards/shields/trackball_wireless_left/trackball_wireless_left.keymap), two lines that define `LEFT_HANDED` and include the shared keymap, so there is nothing to keep in sync. Only the base layer differs. DPI steps in scroll mode are not mirrored, middle hold, then left is DPI down in both builds.

## Changing DPI

Hold the middle button, then tap left to go down a step or right to go up. Five steps, 100 through 500, starting on 300:

| Step | Effective CPI |
| --- | --- |
| 0 | 100 |
| 1 | 200 |
| 2 | 300, the default |
| 3 | 400 |
| 4 | 500 |

Steps come out at the ends. On step 0, pressing left does nothing, and on step 4 pressing right does nothing.

One caveat worth knowing: the sensor itself is fixed at 400 CPI in the overlay and every step is a fraction of that, produced by the `zip_xy_scaler` input processor, not by the sensor. That means the numbers are effective movement, and the sensor still reports 400. This is deliberate, it keeps a single CPI value in devicetree and the steps in one place. If you want real hardware CPI switching instead, set `cpi` higher and drop the scaler.

## Remembering settings

The DPI step is a keymap layer, which ZMK keeps in RAM. A small module, `drivers/persist/trackball_persist.c`, watches the layers, writes the active DPI step to flash 2 seconds after the last change, and restores them at boot. After a power cycle the trackball comes back with the step you left it on. The 2 second delay is `CONFIG_TRACKBALL_PERSIST_SAVE_DELAY_MS`: switching off sooner than that loses the last change. Scroll mode is never saved, it is only active while the middle button is held. Saved values belong to one build: every build is stamped with the commit it was made from, so after flashing a `.uf2` from a new commit the trackball starts from the defaults in the keymap, and from then on remembers again. Flashing the same build again keeps the saved values. The module is on by default for `trackball_wireless` and `trackball_wireless_left`.

## Changing the speeds in the firmware

All in `boards/shields/trackball_wireless/trackball_wireless.overlay`. Speed is a fraction, 3 4 is 3/4.

| What | Where | Now |
| --- | --- | --- |
| Default cursor speed | `input-processors = <&zip_xy_scaler 3 4>;` right under the layer 0 comment in `trackball_listener` | 3/4 |
| The other four DPI steps | the `dpi_step0`, `dpi_step1`, `dpi_step3`, `dpi_step4` blocks, each a `zip_xy_scaler` of two numbers | 1/4, 1/2, 1, 5/4 |
| Scroll speed | the second number of `zip_scroll_scaler 1 40` in each of the five `scroll_step0` to `scroll_step4` blocks, change all five, bigger is slower | 40 |
| Sensor resolution | `trackball` node, `cpi`, multiples of 50, every speed above is a fraction of it | 400 |

If you change the default step, also change which layer the middle button's hold tap in `default_layer` of the keymap points to, so that scroll mode picks the matching step.

## Layers

| Layer | Purpose |
| --- | --- |
| 0 | base, step 2, the default |
| 1 to 5 | one per DPI step, addressable so the default step can come back |
| 6 to 10 | scroll mode combined with each DPI step |

Scroll overrides come before the DPI overrides in the overlay, so holding the middle button switches to scrolling even while a DPI layer is active.

Scrolling is vertical only. The ZMK built in `zip_xy_to_scroll_mapper` also sends X to the horizontal wheel, which would make the ball scroll sideways as well, so the overlay uses its own `y_to_scroll` code mapper that touches Y alone, followed by an X scaler of `0 1` to drop the horizontal axis completely. A side effect is that while scroll mode is held the ball no longer nudges the cursor sideways either.

Scroll speed is the `zip_scroll_scaler` of `1 40` on every scroll layer: one wheel step per forty sensor counts. A bigger second number is slower. The macOS scroll speed slider did not change anything for this device in testing, so tune it here.

## Latency

There are no combos, so left and right clicks go out at once. The middle button is a 150 ms hold tap: released sooner it is a middle click, held longer it is scroll. If middle clicks turn into scroll, raise `tapping-term-ms` of `mkp_lt` in the keymap.

## Bootloader

**Switch the trackball off, hold the left button, plug the USB cable in.** The `NICENANO` drive appears. Copy the `.uf2` onto it and the board reboots by itself.

The power switch has to be off, or the board must not be running yet. The check happens once, in the first moments after power up. If the trackball is already on and you plug the cable in, the board keeps running, and the left button is just a left click. In that case switch it off first, then hold the left button and plug the cable in.

A small module, `drivers/boot_button/trackball_boot_button.c`, reads the left button right after boot, and if it has been held for 100 ms the board reboots into the UF2 bootloader. It reads the first pin of `trackball_buttons` in the overlay and is on by default for both trackball builds (`CONFIG_TRACKBALL_BOOT_BUTTON`). It runs from the firmware, so it does nothing if the firmware cannot start. In that case, or with the reset button reachable, a double tap on the reset button of the nice!nano enters the same bootloader.

## Sensor orientation

If the pointer moves the wrong way after you fix the sensor in place, do not change the keymap. `invert-y` and `invert-x` on the sensor node flip a single axis, `rotate-90` turns the axes a quarter turn and `angle-tune` nudges the angle for a mounting that is not quite square. All of it is software only, no rework.

`invert-y` is already set, because on this mounting the pointer moved down when the ball went up. Because it sits on the sensor node rather than in the input processors, the scroll layers follow along for free and the DPI scaling is untouched.

## Building

Ready to flash binaries are in [`../firmware`](../firmware): `trackball_wireless.uf2` for the right handed build and `trackball_wireless_left.uf2` for the left handed one. You only need to build if you want to change something.

ZMK builds through GitHub Actions rather than on your own machine. This folder is a complete ZMK config with its own drivers, so you do not need anything else from this repository:

1. Create a new repository on GitHub and copy the contents of this folder into its root.
2. Push. The workflow in [`.github/workflows/build.yml`](.github/workflows/build.yml) runs on every push and builds both shields listed in [`build.yaml`](build.yaml).
3. Open the **Actions** tab, pick the finished run, and download the `firmware` artifact. The `.uf2` files are inside.

The trackball is built without the ZMK Studio snippet on purpose. Studio rewrites the keymap at runtime and the layer layout with its DPI and scroll layers is not something you want handed to a generic editor. Orientation, DPI and scroll behaviour cannot be changed from Studio anyway, they are compiled in.

Debug logging is off in the regular build, it slows the firmware down. To read the sensor product ID, make a temporary build with `-DCONFIG_INPUT_LOG_LEVEL_DBG=y` and USB logging.

## Wiring

Sensor signal to the pro micro pin. Do not guess, solder by this table:

| Signal | Pin | Port |
| --- | --- | --- |
| CS | D1 | P0.06 |
| MISO | D0 | P0.08 |
| MOSI | D2 | P0.17 |
| SCLK | D3 | P0.20 |
| MOT | D4 | P0.22, interrupt, active low, pull-up |

VCC is 3.3 V and GND is GND. Power the sensor from the pro micro, never from 5 V, the nice!nano v2 pins are 3.3 V only. Put a 100 nF ceramic between VCC and GND right at the module, many breakouts already have one.

See [`boards/shields/trackball_wireless/trackball_wireless.keymap`](boards/shields/trackball_wireless/trackball_wireless.keymap) and [`boards/shields/trackball_wireless/trackball_wireless.overlay`](boards/shields/trackball_wireless/trackball_wireless.overlay) for the full mapping.
