// Copyright 2026 Salavat Abdullin (@slvt)
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

enum atari800_layers {
  _QWERTY,
  _SHIFT,
  _CONTROL
};

// Keycodes are mapped for the Atari800 emulator
// https://github.com/atari800/atari800/blob/master/DOC/USAGE

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

// QWERTY
// ┌───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┐
// │ESC    │   1   │   2   │   3   │   4   │   5   │   6   │   7   │   8   │   9   │   0   │   <   │   >   │Back S │Break  │SysRst │
// ├───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┤
// │TAB    │   Q   │   W   │   E   │   R   │   T   │   Y   │   U   │   I   │   O   │   P   │   -   │   =   │Return │       │Option │
// ├───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┤
// │CONTROL│   A   │   S   │   D   │   F   │   G   │   H   │   J   │   K   │   L   │   ;   │   +   │   *   │CapsLwr│       │Select │
// ├───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┤
// │SHIFT  │   Z   │   X   │   C   │   V   │   B   │Space  │   N   │   M   │   ,   │   .   │   /   │Inverse│Shift  │F1     │Start  │
// └───────┴───────┴───────┴───────┴───────┴───────┴───────┴───────┴───────┴───────┴───────┴───────┴───────┴───────┴───────┴───────┘

// Ctrl+Up              -
// Ctrl+Down            =
// Ctrl+Left            +
// Ctrl+Right           *

[_QWERTY] = LAYOUT(
    KC_ESC,        KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,   KC_7,    KC_8,    KC_9,    KC_0,    S(KC_LT),   S(KC_GT),    KC_BSPC,       KC_F7, KC_F5,
    KC_TAB,        KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,   KC_U,    KC_I,    KC_O,    KC_P,    C(KC_UP),   C(KC_DOWN),  KC_ENT,               KC_F2,
    MO(_CONTROL),  KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,   KC_J,    KC_K,    KC_L,    KC_SCLN, C(KC_LEFT), C(KC_RIGHT), KC_CAPS,              KC_F3,
    MO(_SHIFT),    KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_SPC, KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,    KC_TILD,     MO(_SHIFT),    KC_F1, KC_F4
),

// SHIFT
// ┌───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┐
// │       │   !   │   "   │   #   │   $   │   %   │   &   │   /   │   @   │   (   │   )   │Clear  │InsLine│Delete │       │       │
// ├───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┤
// │SET    │       │       │       │       │       │       │       │       │       │       │   _   │   |   │       │       │       │
// ├───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┤
// │       │       │       │       │       │       │       │       │       │       │   :   │   \   │   ^   │       │       │       │
// ├───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┤
// │       │       │       │       │       │       │       │       │       │   [   │   ]   │   ?   │       │       │       │       │
// └───────┴───────┴───────┴───────┴───────┴───────┴───────┴───────┴───────┴───────┴───────┴───────┴───────┴───────┴───────┴───────┘

// Shift+Up             _ (Atari Shift+'-')
// Shift+Down           | (Atari Shift+'=')
// Shift+Left           \ (Atari Shift+'+')
// Shift+Right          ^ (Atari Shift+'*')

// Shift+Insert         Insert line (Atari Shift+'>')
// Home                 Clear (Atari Shift+'<')
// Shift+Delete         Delete line (Atari Shift+Backspace)
// Shift+Backspace      Delete line (Atari Shift+Backspace)

[_SHIFT] = LAYOUT(
    XXXXXXX,   S(KC_1), S(KC_QUOT), S(KC_3), S(KC_4), S(KC_5), S(KC_7), KC_SLSH, S(KC_2), S(KC_9), S(KC_0),    KC_HOME,    S(KC_INS),   S(KC_DEL), XXXXXXX, XXXXXXX,
    S(KC_TAB), XXXXXXX, XXXXXXX,    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,    S(KC_UP),   S(KC_DOWN),  XXXXXXX,            XXXXXXX,
    XXXXXXX,   XXXXXXX, XXXXXXX,    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, S(KC_SCLN), S(KC_LEFT), S(KC_RIGHT), XXXXXXX,            XXXXXXX,
    XXXXXXX,   XXXXXXX, XXXXXXX,    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_LBRC, KC_RBRC,    KC_QUES,    XXXXXXX,     XXXXXXX,   XXXXXXX, XXXXXXX
),

// CONTROL
// ┌───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┐
// │       │       │       │       │       │       │       │       │       │       │       │       │InsChr │DelChr │       │       │
// ├───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┤
// │CLR    │       │       │       │       │       │       │       │       │       │       │   ↑   │   ↓   │       │       │       │
// ├───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┤
// │       │       │       │       │       │       │       │       │       │       │       │   ←   │   →   │       │       │       │
// ├───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┤
// │       │       │       │       │       │       │       │       │       │       │       │       │       │       │       │       │
// └───────┴───────┴───────┴───────┴───────┴───────┴───────┴───────┴───────┴───────┴───────┴───────┴───────┴───────┴───────┴───────┘

// Up                   Up (Atari Ctrl+'-')
// Down                 Down (Atari Ctrl+'=')
// Left                 Left (Atari Ctrl+'+')
// Right                Right (Atari Ctrl+'*')

// Insert               Insert character (Atari Ctrl+'>')

// Delete               Delete character (Atari Ctrl+Backspace)
// Ctrl+Backspace       Delete character (Atari Ctrl+Backspace)

[_CONTROL] = LAYOUT(
    XXXXXXX,   XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,   KC_INS,    KC_DEL,  XXXXXXX, XXXXXXX,
    C(KC_TAB), XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_UP,     KC_DOWN,   XXXXXXX,          XXXXXXX,
    XXXXXXX,   XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_LEFT,   KC_RIGHT,  XXXXXXX,          XXXXXXX,
    XXXXXXX,   XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,   XXXXXXX,   XXXXXXX, XXXXXXX, XXXXXXX
)

};
