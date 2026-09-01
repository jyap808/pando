/* Copyright 2026 Julian Yap (@jyap808)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#define MATRIX_ROWS  5
#define MATRIX_COLS  12
#define MATRIX_ROWS_PER_SIDE MATRIX_ROWS
#define MATRIX_COLS_PER_SIDE (MATRIX_COLS / 2)

#define MATRIX_ROW_PINS_L { B0, B1, B2, A8, A9 }
#define MATRIX_COL_PINS_L { A7, A6, A2, A3, A4, A5 }

#define MATRIX_ROW_PINS_R { 0b00000001, 0b00000010, 0b00000100, 0b00001000, 0b00010000 }
#define MATRIX_COL_PINS_R { 0b00000001, 0b00000010, 0b00000100, 0b00001000, 0b00010000, 0b00100000 }

// RIGHT_HALF PCA9555 address (A0=A1=A2=GND → 7-bit 0x20)
#define RIGHT_HALF 0x20

#define RIGHT_COLS_PORT PCA9555_PORT0
#define RIGHT_ROWS_PORT PCA9555_PORT1

// I2C for STM32G0B1KBT6 (I2C1 on PB6/PB7 = AF6)
#define I2C_DRIVER I2CD1
#define I2C1_SCL_PIN B6
#define I2C1_SDA_PIN B7
#define I2C1_SCL_PAL_MODE 6
#define I2C1_SDA_PAL_MODE 6

// I2C timing for STM32G0B1 (I2C1 clock = PCLK = 64MHz, ~400kHz Fast mode)
// f_SCL = 64MHz / ((SCLL+1 + SCLH+1) * (PRESC+1))
// PRESC=0, SCLH=38, SCLL=129 → (39+130) = 169 clocks → 378kHz
// SCLDEL=7 → (7+1) x 15.625ns = 125ns data setup (meets FM 100ns minimum)
#define I2C1_TIMINGR_PRESC  0x00U
#define I2C1_TIMINGR_SCLDEL 0x07U
#define I2C1_TIMINGR_SDADEL 0x00U
#define I2C1_TIMINGR_SCLH   0x26U  // 38
#define I2C1_TIMINGR_SCLL   0x81U  // 129
