/*
 * SPDX-License-Identifier: MIT
 *
 * Boot button for trackball_wireless: hold the left button while the board is
 * powered up and it reboots into the UF2 bootloader, like holding the left
 * button on plug-in does for the QMK build. The pin is the first entry of the
 * trackball_buttons kscan, so it follows the overlay.
 */

#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/reboot.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(trackball_boot_button, CONFIG_ZMK_LOG_LEVEL);

// Same magic value as RST_UF2 in dt-bindings/zmk/reset.h, which is what &bootloader uses.
#define BOOT_UF2_MAGIC 0x57

// The button has to read as pressed for this long in a row, so a glitch on the pin
// during power up does not send the board to the bootloader.
#define BOOT_BUTTON_HOLD_MS 100
#define BOOT_BUTTON_POLL_MS 10

static const struct gpio_dt_spec boot_button =
    GPIO_DT_SPEC_GET_BY_IDX(DT_NODELABEL(trackball_buttons), input_gpios, 0);

static int trackball_boot_button_init(void) {
    if (!gpio_is_ready_dt(&boot_button)) {
        LOG_ERR("Boot button GPIO is not ready");
        return -ENODEV;
    }

    // The flags from the overlay set the pull up and the active low level.
    int err = gpio_pin_configure_dt(&boot_button, GPIO_INPUT);
    if (err) {
        LOG_ERR("Failed to configure the boot button (err %d)", err);
        return err;
    }

    // Let the pull up settle before the first read.
    k_busy_wait(2000);

    for (int waited = 0; waited < BOOT_BUTTON_HOLD_MS; waited += BOOT_BUTTON_POLL_MS) {
        if (gpio_pin_get_dt(&boot_button) != 1) {
            return 0;
        }
        k_busy_wait(BOOT_BUTTON_POLL_MS * 1000);
    }

    LOG_INF("Left button held at boot, entering the bootloader");
    sys_reboot(BOOT_UF2_MAGIC);
    return 0;
}

// Application level, so the GPIO driver is certain to be up. No host is connected yet
// at this point, so the held button is never sent as a click.
SYS_INIT(trackball_boot_button_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
