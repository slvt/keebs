#include QMK_KEYBOARD_H

#include "trackboy.h"

#ifdef VIA_ENABLE
#    include "via.h"
#endif

/* VIA / Vial layout options are packed into the two bytes reserved by
 * VIA_EEPROM_LAYOUT_OPTIONS_SIZE. The order of the fields here has to match the
 * order of the "labels" array in keymaps/vial/vial.json. */
#define TRACKBOY_DRAG_DPI_SHIFT        0
#define TRACKBOY_DRAG_DPI_MASK         (0x0Fu << TRACKBOY_DRAG_DPI_SHIFT)
#define TRACKBOY_DRAG_DPI_ENABLE_SHIFT 4
#define TRACKBOY_DRAG_DPI_ENABLE_MASK  (0x01u << TRACKBOY_DRAG_DPI_ENABLE_SHIFT)
#define TRACKBOY_SCROLL_SHIFT          5
#define TRACKBOY_SCROLL_MASK           (0x0Fu << TRACKBOY_SCROLL_SHIFT)
#define TRACKBOY_DPI_SHIFT             9
#define TRACKBOY_DPI_MASK              (0x0Fu << TRACKBOY_DPI_SHIFT)

#define TRACKBOY_DRAG_DPI_DEFAULT_IDX 4
#define TRACKBOY_DPI_DEFAULT_IDX      7
#define TRACKBOY_SCROLL_DEFAULT_IDX   5

/* How long a trackball button has to be held before it counts as a hold rather
 * than a tap. A keymap can override either of them. */
#ifndef TB_LEFT_HOLD_TERM
#    define TB_LEFT_HOLD_TERM TAPPING_TERM
#endif
#ifndef TB_RIGHT_HOLD_TERM
#    define TB_RIGHT_HOLD_TERM TAPPING_TERM
#endif

static const uint16_t dpi_table[]    = {100, 200, 300, 400, 500, 600, 800, 1000, 1200, 1600, 2000, 2500, 3200, 4000, 5000};
static const int32_t  scroll_table[] = {8, 16, 24, 32, 40, 48, 56, 64, 72, 80};

static uint8_t dpi_index        = TRACKBOY_DPI_DEFAULT_IDX;
static uint8_t drag_dpi_index   = TRACKBOY_DRAG_DPI_DEFAULT_IDX;
static uint8_t scroll_index     = TRACKBOY_SCROLL_DEFAULT_IDX;
static bool    drag_dpi_enabled = true;

static bool     left_button_held        = false;
static bool     scroll_mode_active      = false;
static bool     left_button_pressed     = false;
static bool     right_button_pressed    = false;
static bool     left_button_pending     = false;
static bool     left_button_active      = false;
static bool     right_scroll_active     = false;
static bool     right_button_suppressed = false;
static uint16_t left_button_timer       = 0;
static uint16_t right_button_timer      = 0;

static uint8_t clamp_dpi_index(uint8_t index) {
    return MIN(index, ARRAY_SIZE(dpi_table) - 1);
}

static uint8_t clamp_scroll_index(uint8_t index) {
    return MIN(index, ARRAY_SIZE(scroll_table) - 1);
}

static void refresh_cpi(void) {
    uint8_t active_dpi_index = dpi_index;

    if (drag_dpi_enabled && left_button_held) {
        active_dpi_index = drag_dpi_index;
    }

    pointing_device_set_cpi(dpi_table[clamp_dpi_index(active_dpi_index)]);
}

static void set_left_button_held(bool held) {
    left_button_held = held;
    refresh_cpi();
}

static void apply_layout_options(uint32_t raw) {
    drag_dpi_enabled = (raw & TRACKBOY_DRAG_DPI_ENABLE_MASK) != 0;
    drag_dpi_index   = clamp_dpi_index((raw & TRACKBOY_DRAG_DPI_MASK) >> TRACKBOY_DRAG_DPI_SHIFT);
    dpi_index        = clamp_dpi_index((raw & TRACKBOY_DPI_MASK) >> TRACKBOY_DPI_SHIFT);
    scroll_index     = clamp_scroll_index((raw & TRACKBOY_SCROLL_MASK) >> TRACKBOY_SCROLL_SHIFT);
    refresh_cpi();
}

void via_set_layout_options_kb(uint32_t raw) {
    apply_layout_options(raw);
}

void pointing_device_init_kb(void) {
#ifdef VIA_ENABLE
    apply_layout_options(via_get_layout_options());
#else
    apply_layout_options(VIA_EEPROM_LAYOUT_OPTIONS_DEFAULT);
#endif

    pointing_device_init_user();
}

report_mouse_t pointing_device_task_kb(report_mouse_t report) {
    static int32_t accumulated_scroll = 0;

    if (scroll_mode_active) {
        accumulated_scroll += report.y;
        int32_t scroll_divisor = scroll_table[clamp_scroll_index(scroll_index)];
        int32_t shift          = accumulated_scroll / scroll_divisor;
        accumulated_scroll -= shift * scroll_divisor;

        report.x = 0;
        report.y = 0;
        report.h = 0;
        report.v = shift;
    } else {
        accumulated_scroll = 0;
    }

    return pointing_device_task_user(report);
}

void matrix_scan_kb(void) {
    if (left_button_pressed && left_button_pending && timer_elapsed(left_button_timer) >= TB_LEFT_HOLD_TERM) {
        register_code16(MS_BTN1);
        set_left_button_held(true);
        left_button_active  = true;
        left_button_pending = false;
    }

    if (right_button_pressed && !left_button_pressed && !right_button_suppressed && !scroll_mode_active &&
        !right_scroll_active && timer_elapsed(right_button_timer) >= TB_RIGHT_HOLD_TERM) {
        scroll_mode_active  = true;
        right_scroll_active = true;
    }

    matrix_scan_user();
}

bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case TB_LEFT_BTN:
            if (record->event.pressed) {
                left_button_pressed = true;
                left_button_timer   = timer_read();
                left_button_pending = true;
                if (right_button_pressed) {
                    right_button_suppressed = true;
                }
            } else {
                left_button_pressed = false;

                if (left_button_pending) {
                    tap_code16(MS_BTN1);
                    left_button_pending = false;
                } else if (left_button_active) {
                    set_left_button_held(false);
                    unregister_code16(MS_BTN1);
                    left_button_active = false;
                }
            }
            return false;

        case TB_RIGHT_BTN:
            if (record->event.pressed) {
                right_button_pressed    = true;
                right_button_timer      = timer_read();
                right_scroll_active     = false;
                right_button_suppressed = left_button_pressed;
            } else {
                right_button_pressed = false;

                if (right_scroll_active) {
                    scroll_mode_active = false;
                } else if (!right_button_suppressed) {
                    tap_code16(MS_BTN2);
                }

                right_scroll_active     = false;
                right_button_suppressed = false;
            }
            return false;
    }

    return process_record_user(keycode, record);
}
