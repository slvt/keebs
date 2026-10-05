#include QMK_KEYBOARD_H

#include "trackball.h"

#ifdef VIA_ENABLE
#    include "via.h"
#endif

/* VIA / Vial layout options are packed into the two bytes reserved by
 * VIA_EEPROM_LAYOUT_OPTIONS_SIZE. The order of the fields here has to match the
 * order of the "labels" array in keymaps/vial/vial.json, the last label is the
 * least significant. */
#define TRACKBALL_LEFT_HANDED_SHIFT     0
#define TRACKBALL_LEFT_HANDED_MASK      (0x01u << TRACKBALL_LEFT_HANDED_SHIFT)
#define TRACKBALL_BUTTONS_SHIFT         1
#define TRACKBALL_BUTTONS_MASK          (0x03u << TRACKBALL_BUTTONS_SHIFT)
#define TRACKBALL_DRAG_DPI_SHIFT        3
#define TRACKBALL_DRAG_DPI_MASK         (0x0Fu << TRACKBALL_DRAG_DPI_SHIFT)
#define TRACKBALL_DRAG_DPI_ENABLE_SHIFT 7
#define TRACKBALL_DRAG_DPI_ENABLE_MASK  (0x01u << TRACKBALL_DRAG_DPI_ENABLE_SHIFT)
#define TRACKBALL_SCROLL_SHIFT          8
#define TRACKBALL_SCROLL_MASK           (0x0Fu << TRACKBALL_SCROLL_SHIFT)
#define TRACKBALL_DPI_SHIFT             12
#define TRACKBALL_DPI_MASK              (0x0Fu << TRACKBALL_DPI_SHIFT)

/* Values of the Buttons field, the same order as the Buttons label in vial.json. */
#define TRACKBALL_ONE_BUTTON    0
#define TRACKBALL_TWO_BUTTONS   1
#define TRACKBALL_THREE_BUTTONS 2

#define TRACKBALL_DRAG_DPI_DEFAULT_IDX 4
#define TRACKBALL_DPI_DEFAULT_IDX      7
#define TRACKBALL_SCROLL_DEFAULT_IDX   5

/* How long a button has to be held before it counts as a hold rather than a
 * tap. A keymap can override each of them. */
#ifndef TB_LEFT_HOLD_TERM
#    define TB_LEFT_HOLD_TERM TAPPING_TERM
#endif
#ifndef TB_RIGHT_HOLD_TERM
#    define TB_RIGHT_HOLD_TERM TAPPING_TERM
#endif
#ifndef TB_MID_HOLD_TERM
#    define TB_MID_HOLD_TERM TAPPING_TERM
#endif

#ifndef VIA_ENABLE
/* Without VIA or Vial the DPI is changed by a gesture: hold the middle button
 * and press right for the next DPI step or left for the previous one. The step
 * is stored in the keyboard EEPROM word together with an id of this build, so a
 * newly flashed build starts from the default DPI again. */
#    define TB_DPI_GESTURE
#endif

static const uint16_t dpi_table[]    = {100, 200, 300, 400, 500, 600, 800, 1000, 1200, 1600, 2000, 2500, 3200, 4000, 5000};
static const int32_t  scroll_table[] = {8, 16, 24, 32, 40, 48, 56, 64, 72, 80};

static uint8_t dpi_index        = TRACKBALL_DPI_DEFAULT_IDX;
static uint8_t drag_dpi_index   = TRACKBALL_DRAG_DPI_DEFAULT_IDX;
static uint8_t scroll_index     = TRACKBALL_SCROLL_DEFAULT_IDX;
static bool    drag_dpi_enabled = true;
static bool    three_buttons    = true;
static bool    one_button       = false;
static bool    left_handed      = false;

static bool     left_button_held        = false;
static bool     scroll_mode_active      = false;
static bool     left_button_pressed     = false;
static bool     right_button_pressed    = false;
static bool     left_button_pending     = false;
static bool     left_button_active      = false;
static bool     right_button_active     = false;
static bool     mid_button_pressed      = false;
static bool     mid_button_pending      = false;
static bool     right_scroll_active     = false;
static bool     right_button_suppressed = false;
static uint16_t left_button_timer       = 0;
static uint16_t right_button_timer      = 0;
static uint16_t mid_button_timer        = 0;

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

#ifdef TB_DPI_GESTURE
/* A 16 bit hash of the compile date and time, it changes with every build. */
static uint16_t build_id(void) {
    const char *text = __DATE__ __TIME__;
    uint16_t    hash = 5381;

    while (*text) {
        hash = (uint16_t)((hash * 33u) ^ (uint8_t)*text++);
    }

    return hash ? hash : 1;
}

static void save_dpi_index(uint8_t index) {
    eeconfig_update_kb(((uint32_t)build_id() << 16) | index);
}

static void step_dpi(int8_t direction) {
    uint8_t next = dpi_index;

    if (direction > 0 && next < ARRAY_SIZE(dpi_table) - 1) {
        next++;
    } else if (direction < 0 && next > 0) {
        next--;
    }

    if (next != dpi_index) {
        dpi_index = next;
        refresh_cpi();
        save_dpi_index(dpi_index);
    }
}
#endif

static void apply_layout_options(uint32_t raw) {
    drag_dpi_enabled = (raw & TRACKBALL_DRAG_DPI_ENABLE_MASK) != 0;
    drag_dpi_index   = clamp_dpi_index((raw & TRACKBALL_DRAG_DPI_MASK) >> TRACKBALL_DRAG_DPI_SHIFT);
    dpi_index        = clamp_dpi_index((raw & TRACKBALL_DPI_MASK) >> TRACKBALL_DPI_SHIFT);
    scroll_index     = clamp_scroll_index((raw & TRACKBALL_SCROLL_MASK) >> TRACKBALL_SCROLL_SHIFT);
    uint8_t buttons  = (raw & TRACKBALL_BUTTONS_MASK) >> TRACKBALL_BUTTONS_SHIFT;
    three_buttons    = buttons == TRACKBALL_THREE_BUTTONS;
    one_button       = buttons == TRACKBALL_ONE_BUTTON;
    left_handed      = (raw & TRACKBALL_LEFT_HANDED_MASK) != 0;
    refresh_cpi();
}

void via_set_layout_options_kb(uint32_t raw) {
    apply_layout_options(raw);
}

void pointing_device_init_kb(void) {
#ifdef VIA_ENABLE
    apply_layout_options(via_get_layout_options());
#else
    uint32_t raw    = VIA_EEPROM_LAYOUT_OPTIONS_DEFAULT;
    uint32_t stored = eeconfig_read_kb();

    if ((stored >> 16) == build_id()) {
        raw = (raw & ~TRACKBALL_DPI_MASK) | ((stored & 0x0Fu) << TRACKBALL_DPI_SHIFT);
    } else {
        save_dpi_index(TRACKBALL_DPI_DEFAULT_IDX);
    }
    apply_layout_options(raw);
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
    if (three_buttons) {
        if (mid_button_pressed && mid_button_pending && !scroll_mode_active && timer_elapsed(mid_button_timer) >= TB_MID_HOLD_TERM) {
            scroll_mode_active  = true;
            mid_button_pending  = false;
        }
    } else {
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
    }

    matrix_scan_user();
}

/* Two buttons: left is a click that switches to the drag DPI while held, right
 * is a tap for right click and a hold for scroll. */
static void process_two_buttons(uint16_t keycode, bool pressed) {
    switch (keycode) {
        case TB_LEFT_BTN:
            if (pressed) {
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
            break;

        case TB_RIGHT_BTN:
            if (pressed) {
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
            break;

        case TB_MID_BTN:
            if (pressed) {
                register_code16(MS_BTN3);
            } else {
                unregister_code16(MS_BTN3);
            }
            break;
    }
}

/* One button: only a left button, wired to the left pin. Pressing it holds the
 * left mouse button, so a tap is a click and holding it while moving the ball
 * selects or drags, like on the one button Macs. Nothing is delayed. The drag
 * DPI applies while it is held, if enabled. Left handed mode has no effect. */
static void process_one_button(bool pressed) {
    if (pressed) {
        register_code16(MS_BTN1);
        set_left_button_held(true);
    } else {
        set_left_button_held(false);
        unregister_code16(MS_BTN1);
    }
}

/* Three buttons: left and right are plain clicks, middle is a tap for middle
 * click and a hold for scroll. Nothing is delayed. Without VIA the middle
 * button also works as a modifier, see TB_DPI_GESTURE: left and right then change
 * the DPI instead of clicking, and the middle button gives no click. */
static void process_three_buttons(uint16_t keycode, bool pressed) {
    switch (keycode) {
        case TB_LEFT_BTN:
            if (pressed) {
#ifdef TB_DPI_GESTURE
                if (mid_button_pressed) {
                    mid_button_pending = false;
                    step_dpi(-1);
                    break;
                }
#endif
                register_code16(MS_BTN1);
                set_left_button_held(true);
                left_button_active = true;
            } else if (left_button_active) {
                set_left_button_held(false);
                unregister_code16(MS_BTN1);
                left_button_active = false;
            }
            break;

        case TB_RIGHT_BTN:
            if (pressed) {
#ifdef TB_DPI_GESTURE
                if (mid_button_pressed) {
                    mid_button_pending = false;
                    step_dpi(1);
                    break;
                }
#endif
                register_code16(MS_BTN2);
                right_button_active = true;
            } else if (right_button_active) {
                unregister_code16(MS_BTN2);
                right_button_active = false;
            }
            break;

        case TB_MID_BTN:
            if (pressed) {
                mid_button_pressed = true;
                mid_button_timer   = timer_read();
                mid_button_pending = true;
            } else {
                mid_button_pressed = false;

                if (mid_button_pending) {
                    tap_code16(MS_BTN3);
                    mid_button_pending = false;
                }
                scroll_mode_active = false;
            }
            break;
    }
}

bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case TB_LEFT_BTN:
        case TB_RIGHT_BTN:
        case TB_MID_BTN:
            if (one_button) {
                if (keycode == TB_LEFT_BTN) {
                    process_one_button(record->event.pressed);
                }
                return false;
            }

            /* Left handed mode swaps the roles of the outer buttons. */
            if (left_handed && keycode != TB_MID_BTN) {
                keycode = (keycode == TB_LEFT_BTN) ? TB_RIGHT_BTN : TB_LEFT_BTN;
            }

            if (three_buttons) {
                process_three_buttons(keycode, record->event.pressed);
            } else {
                process_two_buttons(keycode, record->event.pressed);
            }
            return false;
    }

    return process_record_user(keycode, record);
}
