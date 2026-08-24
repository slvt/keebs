// Copyright 2026 Salavat Abdullin (@slvt)
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

enum canonboy_layers {
  _QWERTY
};

// Keycodes are mapped for the Canon Cat emulator.
// Emulator: https://archive.org/details/canoncat
// Key handling in MAME: https://github.com/mamedev/mame/blob/master/src/mame/canon/cat.cpp

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

// QWERTY
// ┌────┬────┬────┬────┬────┬────┬────┬────┬────┬────┬────┬────┬────┬────────┐
// │±   │1   │2   │3   │4   │5   │6   │7   │8   │9   │0   │-   │+   │    UNDO│
// ├────┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──────┤
// │TAB   │Q   │W   │E   │R   │T   │Y   │U   │I   │O   │P   │¼   │[]  │ ERASE│
// ├──────┴┬───┴┬───┴┬───┴┬───┴┬───┴┬───┴┬───┴┬───┴┬───┴┬───┴┬───┴┬───┴──────┤
// │LOCK   │A   │S   │D   │F   │G   │H   │J   │K   │L   │;   │+   │    RETURN│
// ├───────┴──┬─┴──┬─┴──┬─┴──┬─┴──┬─┴──┬─┴──┬─┴──┬─┴──┬─┴──┬─┴──┬─┴──────────┤
// │SHIFT     │Z   │X   │C   │V   │B   │N   │M   │,   │.   │/   │       SHIFT│
// ├────────┬─┴────┴─┬──┴────┴────┴────┴────┴────┴┬───┴────┼────┴───┬────────┤
// │FRONT   │LEAP    │                            │LEAP    │FRONT   │DOCUMENT│
// └────────┴────────┴────────────────────────────┴────────┴────────┴────────┘

[_QWERTY] = LAYOUT(
    KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSLS,
    KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSPC,
    KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT, KC_ENT,
    KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT,
    KC_LCTL, KC_LALT, KC_SPC,  KC_RALT, KC_RCTL, KC_PGUP
)

};
