/*
 * Copyright (c) 2025 George Norton
 *
 * SPDX-License-Identifier: MIT
 *
 * Modified: PMW3389 support, report rate limiting, polling mode removed.
 * Based on https://github.com/george-norton/zmk-driver-pmw3360
 */

#pragma once

#include <zephyr/device.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>

#ifdef __cplusplus
extern "C" {
#endif

// Indicates the direction of a SPI transaction
#define PMW33XX_SPI_READ 0
#define PMW33XX_SPI_WRITE 0x80

// Timing values from the datasheet
#define PMW33XX_T_SRAD 160
#define PMW33XX_T_SCLK_NCS_READ 120
#define PMW33XX_T_SCLK_NCS_WRITE 35
#define PMW33XX_T_SRR 20  // Same as T_SRW
#define PMW33XX_T_SWR 180 // Same as T_SWW
#define PMW33XX_T_SRAD_MOTBR 35

// Select register addresses and values
#define PMW33XX_REG_PRODUCT_ID          0x00
#define PMW33XX_REG_REVISION_ID         0x01
#define PMW33XX_REG_MOTION              0x02
#   define PMW33XX_MOTION_MOT           0x80
#define PMW33XX_REG_DELTA_X_L           0x03
#define PMW33XX_REG_DELTA_X_H           0x04
#define PMW33XX_REG_DELTA_Y_L           0x05
#define PMW33XX_REG_DELTA_Y_H           0x06
#define PMW33XX_REG_CONTROL             0x0D
#   define PMW33XX_CONTROL_ROTATE_90    0xC0
#   define PMW33XX_CONTROL_ROTATE_180   0x60
#   define PMW33XX_CONTROL_ROTATE_270   0xA0
#define PMW33XX_REG_CONFIG_1            0x0F
// PMW3389 only: the 16 bit resolution is split over two registers, and the
// high byte sits at the same address the PMW3360 uses for CONFIG_1.
#define PMW3389_REG_RESOLUTION_L        0x0E
#define PMW3389_REG_RESOLUTION_H        0x0F
#define PMW33XX_REG_CONFIG_2            0x10
#   define PMW33XX_CONFIG_2_RTP_MOD     0x04
#   define PMW33XX_CONFIG_2_REST_EN     0x20

#define PMW33XX_REG_ANGLE_TUNE          0x11
#define PMW33XX_REG_POWER_UP            0x3A
#define PMW33XX_REG_MOTION_BURST        0x50
#define PMW33XX_REG_LIFT_CONFIG         0x63
#   define PMW33XX_LIFT_CONFIG_2MM      0x02
#   define PMW33XX_LIFT_CONFIG_3MM      0x03

// Which chip an instance drives, it decides how the CPI registers are written
enum pmw33xx_chip {
    PMW33XX_CHIP_3360,
    PMW33XX_CHIP_3389,
};

// Runtime configurable attributes
enum pmw33xx_attributes {
    PMW33XX_ATTR_CPI
};

// The motion burst report, this can be exteneded to read more
// data, but we dont use it, so we dont bother reading it.
struct motion_burst {
    uint8_t motion;
    uint8_t observation;
    uint8_t delta_x_l;
    uint8_t delta_x_h;
    uint8_t delta_y_l;
    uint8_t delta_y_h;
/*  uint8_t squal;
    uint8_t raw_data_sum;
    uint8_t maximum_raw_data;
    uint8_t minimum_raw_data;
    uint8_t shutter_upper;
    uint8_t shutter_lower; */
} __packed;

// The drivers private data
struct pmw33xx_data {
    const struct device *dev;
    struct k_mutex mutex; // Prevent set_attr from changing things while we are initializing
    bool ready;
    uint16_t cpi;
    bool motion_burst_active;
#if defined(CONFIG_INPUT_PIXART_PMW33XX_USE_OWN_THREAD)
    struct k_work_q driver_work_queue;
#endif
    struct k_work motion_work;
    struct k_work_delayable rearm_work; // re-enables the motion interrupt after the report interval
    struct k_work_delayable init_work;
    struct gpio_callback irq_gpio_cb; // motion pin irq callback
};

// The drivers configuration struture - populated by the device tree
struct pmw33xx_config {
    struct spi_dt_spec spi;
    struct gpio_dt_spec cs_gpio;
    struct gpio_dt_spec irq_gpio;
    struct gpio_dt_spec reset_gpio;
    uint16_t cpi;
    bool rotate_90;
    bool rotate_180;
    bool rotate_270;
    int8_t angle_tune;
    bool lift_height_3mm;
    uint32_t report_interval_ms;
    enum pmw33xx_chip chip;
    bool invert_x;
    bool invert_y;
};

#ifdef __cplusplus
}
#endif