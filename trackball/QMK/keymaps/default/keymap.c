#include QMK_KEYBOARD_H

#include "trackball.h"

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT(TB_LEFT_BTN, TB_MID_BTN, TB_RIGHT_BTN),
    [1] = LAYOUT(KC_TRNS, KC_TRNS, KC_TRNS),
    [2] = LAYOUT(KC_TRNS, KC_TRNS, KC_TRNS),
    [3] = LAYOUT(KC_TRNS, KC_TRNS, KC_TRNS)
};
