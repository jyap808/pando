// Copyright 2026 Julian Yap (@jyap808)
// SPDX-License-Identifier: GPL-2.0-or-later

#include "quantum.h"
#include "matrix.h"
#include "pca9555.h"
#include "i2c_master.h"
#include "timer.h"

static const pin_t left_cols[MATRIX_COLS_PER_SIDE] = MATRIX_COL_PINS_L;
static const pin_t left_rows[MATRIX_ROWS_PER_SIDE] = MATRIX_ROW_PINS_L;
static const uint8_t right_cols[MATRIX_COLS_PER_SIDE] = MATRIX_COL_PINS_R;
static const uint8_t right_rows[MATRIX_ROWS_PER_SIDE] = MATRIX_ROW_PINS_R;

// If the right half is missing, re-probe it this often so a disconnected
// half never slows down the scan loop and reconnects promptly.
#define RIGHT_HALF_RETRY_MS 2000

static bool     right_connected = false;
static uint32_t right_retry_at  = 0;

static void right_half_init(void) {
    pca9555_set_config(RIGHT_HALF, RIGHT_COLS_PORT, ALL_INPUT);
    pca9555_set_config(RIGHT_HALF, RIGHT_ROWS_PORT, ALL_OUTPUT);
    pca9555_set_output(RIGHT_HALF, RIGHT_ROWS_PORT, ALL_HIGH);
}

// On STM32G0B1, PA8 is UCPD1_CC1 and its internal dead-battery pull-down is
// enabled by default after reset. This fights GPIO usage of the pin, so the
// ST-recommended fix for using PA8 as a normal GPIO is to set the UCPD1 strobe
// bit in SYSCFG_CFGR1, which disconnects the internal pull-down. UCPD2's pins
// (PD0/PD2) aren't exposed on the 32-pin package but the strobe bit is set
// anyway for completeness. The SYSCFG clock is already enabled by ChibiOS HAL
// init at this point. UCPD is not used by this keyboard.
void keyboard_pre_init_kb(void) {
    SYSCFG->CFGR1 |= SYSCFG_CFGR1_UCPD1_STROBE | SYSCFG_CFGR1_UCPD2_STROBE;
    keyboard_pre_init_user();
}

void matrix_init_custom(void) {
    // Probe the right half; if present, configure the expander ports. Left
    // half GPIOs are always initialised so the board works standalone.
    pca9555_init(RIGHT_HALF);

    uint8_t test = 0xFF;
    right_connected = pca9555_read_pins(RIGHT_HALF, RIGHT_COLS_PORT, &test);
    uprintf("right half %s\n", right_connected ? "connected" : "NOT FOUND");

    if (right_connected) {
        right_half_init();
    }

    for (uint8_t c = 0; c < MATRIX_COLS_PER_SIDE; c++) {
        gpio_set_pin_input_high(left_cols[c]);
    }
    for (uint8_t r = 0; r < MATRIX_ROWS_PER_SIDE; r++) {
        gpio_set_pin_output(left_rows[r]);
        gpio_write_pin_high(left_rows[r]);
    }
}

bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    bool changed = false;

    for (uint8_t r = 0; r < MATRIX_ROWS; r++) {
        matrix_row_t row = 0;

        gpio_write_pin_low(left_rows[r]);
        matrix_io_delay();

        for (uint8_t c = 0; c < MATRIX_COLS_PER_SIDE; c++) {
            if (!gpio_read_pin(left_cols[c])) {
                row |= (MATRIX_ROW_SHIFTER << c);
            }
        }

        gpio_write_pin_high(left_rows[r]);

        // Right half I2C is only touched while connected, so a missing half
        // doesn't add I2C timeouts to the scan loop.
        if (right_connected) {
            pca9555_set_output(RIGHT_HALF, RIGHT_ROWS_PORT, ~right_rows[r]);

            uint8_t input = 0xFF;
            if (pca9555_read_pins(RIGHT_HALF, RIGHT_COLS_PORT, &input)) {
                for (uint8_t c = 0; c < MATRIX_COLS_PER_SIDE; c++) {
                    // Right-half columns are wired in reverse order (PCB
                    // mirroring) and sit in matrix cols 6-11.
                    if (!(input & right_cols[c])) {
                        row |= (MATRIX_ROW_SHIFTER << (MATRIX_COLS_PER_SIDE - 1 - c + MATRIX_COLS_PER_SIDE));
                    }
                }
            } else {
                right_connected = false;
                uprintf("right half disconnected\n");
                break;
            }

            pca9555_set_output(RIGHT_HALF, RIGHT_ROWS_PORT, ALL_HIGH);
        }

        if (current_matrix[r] != row) {
            current_matrix[r] = row;
            changed = true;
        }
    }

    if (!right_connected) {
        // Expander is gone: clear right-half bits (cols 6-11) so no keys stick.
        for (uint8_t r = 0; r < MATRIX_ROWS; r++) {
            current_matrix[r] &= (MATRIX_ROW_SHIFTER << MATRIX_COLS_PER_SIDE) - 1;
        }

        if (timer_elapsed32(right_retry_at) > RIGHT_HALF_RETRY_MS) {
            right_retry_at = timer_read32();
            if (i2c_ping_address(RIGHT_HALF << 1, 100) == I2C_STATUS_SUCCESS) {
                right_half_init();
                right_connected = true;
                uprintf("right half connected\n");
            }
        }
    }

    return changed;
}
