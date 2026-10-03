/*
 * SPDX-License-Identifier: MIT
 *
 * Saves the DPI step and left handed mode of the trackball_wireless keymap to
 * flash and puts them back at boot. Both live in keymap layers, which ZMK keeps
 * in RAM only, so without this every power cycle returns to 300 DPI, right handed.
 *
 * Layer numbers match trackball_wireless.keymap:
 *   1..5   DPI steps 0..4, 3 is the default 300
 *   6      left handed
 *   7..11  scroll mode, transient while the middle button is held
 */

#include <zephyr/kernel.h>
#include <zephyr/settings/settings.h>
#include <zephyr/logging/log.h>

#include <zmk/event_manager.h>
#include <zmk/events/layer_state_changed.h>
#include <zmk/keymap.h>

LOG_MODULE_REGISTER(trackball_persist, CONFIG_ZMK_LOG_LEVEL);

#define DPI_LAYER_FIRST 1
#define DPI_LAYER_LAST 5
#define DPI_LAYER_DEFAULT 3
#define LEFT_HANDED_LAYER 6
#define SCROLL_LAYER_FIRST 7
#define SCROLL_LAYER_LAST 11

#define SETTINGS_KEY "trackball/state"

struct persist_state {
    uint32_t build_id;
    uint8_t dpi_layer;
    uint8_t left_handed;
};

// FNV-1a over the commit hash the build was made from, set in CMakeLists.txt,
// or over the compile time stamp when git was not available. A new firmware gets
// a new id, so values saved by an older build are ignored and every flash of a
// new build starts from the defaults.
#ifdef TRACKBALL_BUILD_STAMP
#define BUILD_STAMP TRACKBALL_BUILD_STAMP
#else
#define BUILD_STAMP __DATE__ " " __TIME__
#endif

static uint32_t build_id(void) {
    static const char stamp[] = BUILD_STAMP;
    uint32_t h = 2166136261u;

    for (size_t i = 0; i < sizeof(stamp) - 1; i++) {
        h = (h ^ (uint8_t)stamp[i]) * 16777619u;
    }
    return h;
}

static struct persist_state saved = {.dpi_layer = DPI_LAYER_DEFAULT, .left_handed = 0};
static bool restoring;

static struct persist_state current_state(bool *scroll_active) {
    zmk_keymap_layers_state_t state = zmk_keymap_layer_state();
    struct persist_state s = {
        .build_id = build_id(), .dpi_layer = DPI_LAYER_DEFAULT, .left_handed = 0};

    *scroll_active = false;
    for (int l = SCROLL_LAYER_FIRST; l <= SCROLL_LAYER_LAST; l++) {
        if (state & BIT(l)) {
            *scroll_active = true;
        }
    }
    for (int l = DPI_LAYER_LAST; l >= DPI_LAYER_FIRST; l--) {
        if (state & BIT(l)) {
            s.dpi_layer = l;
            break;
        }
    }
    s.left_handed = (state & BIT(LEFT_HANDED_LAYER)) ? 1 : 0;
    return s;
}

static void save_work_cb(struct k_work *work) {
    bool scroll_active;
    struct persist_state s = current_state(&scroll_active);

    // Scroll mode is a transient overlay. The &to that ends a DPI change leaves
    // it, and the following layer event schedules the save again.
    if (scroll_active || (s.dpi_layer == saved.dpi_layer && s.left_handed == saved.left_handed)) {
        return;
    }

    int err = settings_save_one(SETTINGS_KEY, &s, sizeof(s));
    if (err) {
        LOG_ERR("Failed to save trackball state (err %d)", err);
        return;
    }
    saved = s;
    LOG_DBG("Saved DPI layer %d, left handed %d", s.dpi_layer, s.left_handed);
}

static K_WORK_DELAYABLE_DEFINE(save_work, save_work_cb);

static int layer_changed_listener(const zmk_event_t *eh) {
    if (restoring) {
        return ZMK_EV_EVENT_BUBBLE;
    }
    k_work_reschedule(&save_work, K_MSEC(CONFIG_TRACKBALL_PERSIST_SAVE_DELAY_MS));
    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(trackball_persist, layer_changed_listener);
ZMK_SUBSCRIPTION(trackball_persist, zmk_layer_state_changed);

static int persist_set(const char *name, size_t len, settings_read_cb read_cb, void *cb_arg) {
    const char *next;

    if (settings_name_steq(name, "state", &next) && !next) {
        if (len != sizeof(saved)) {
            return -EINVAL;
        }
        struct persist_state loaded;
        int err = read_cb(cb_arg, &loaded, sizeof(loaded));

        if (err < 0) {
            return err;
        }
        // Saved by another build, keep the defaults.
        if (loaded.build_id == build_id()) {
            saved = loaded;
        }
        return 0;
    }
    return -ENOENT;
}

static int persist_commit(void) {
    restoring = true;

    if (saved.dpi_layer >= DPI_LAYER_FIRST && saved.dpi_layer <= DPI_LAYER_LAST &&
        saved.dpi_layer != DPI_LAYER_DEFAULT) {
        zmk_keymap_layer_to(saved.dpi_layer);
    }
    if (saved.left_handed) {
        zmk_keymap_layer_activate(LEFT_HANDED_LAYER);
    }

    restoring = false;
    LOG_DBG("Restored DPI layer %d, left handed %d", saved.dpi_layer, saved.left_handed);
    return 0;
}

SETTINGS_STATIC_HANDLER_DEFINE(trackball, "trackball", NULL, persist_set, persist_commit, NULL);
