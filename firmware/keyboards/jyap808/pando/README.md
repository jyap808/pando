# Pando

Split keyboard with STM32G0B1KBT6 master communicating with a PCA9555 I/O
expander slave over I2C via USB-C interconnect. Works as a full 58-key split,
or the left half can be used standalone.

## Matrix Configuration

The keyboard is a 58-key columnar split (mirroring the Pando58 layout) with a
single STM32G0B1 master driving both halves:

- **Left half**: rows B0-B2/A8-A9 and cols A7/A6/A2-A5 are scanned directly
  from the MCU GPIOs.
- **Right half**: rows and cols go through the PCA9555 over I2C. `MATRIX_ROWS`
  is 5, `MATRIX_COLS` is 12 (6 per side), with the right half occupying matrix
  cols 6-11.

See `config.h` for `MATRIX_ROW_PINS_L/R` and `MATRIX_COL_PINS_L/R`. The custom
matrix driver in `matrix.c` drives one row low at a time (GPIO on the left,
PCA9555 output on the right) and reads the columns, so both halves are scanned
without any split-transport logic.

### Standalone use

The **left half works standalone**: at init the driver probes the PCA9555 and
only touches the right half over I2C while it is present. If the expander is
disconnected mid-session, the right-half columns are cleared so no keys stick,
and the driver re-probes every 2 seconds for the right half to come back (see
`RIGHT_HALF_RETRY_MS` in `matrix.c`).

### Testing

Build and flash:
```
make jyap808/pando:vial
```

Connect with `qmk console` — the firmware initialises the PCA9555, reads its
pins to verify connectivity, and configures its ports in `matrix_init_custom()`.
Check the console output for device detection and error messages.

## I2C Configuration

The STM32G0B1 uses the I2Cv2 peripheral which requires explicit timing register
configuration via `I2C1_TIMINGR_*` defines in `config.h`.

The SCL frequency formula is:
```
f_SCL = f_I2CCLK / ((SCLL+1 + SCLH+1) * (PRESC+1))
```

### Clock source

The GENERIC_STM32_G0B1XB board config (used by QMK for STM32G0B1) sets the I2C1
clock source to PCLK (64MHz) via `STM32_I2C1SEL = STM32_I2C1SEL_PCLK`.

This keyboard does not need to override the clock source — PCLK = SYSCLK
= 64MHz.

### Timing values

QMK's ChibiOS I2C driver takes `I2C1_TIMINGR_*` defines from `config.h` and
falls back to built-in defaults (in `platforms/chibios/drivers/i2c_master.c`,
commented as "assuming a 72MHz clock") if they are not set. The values in
`config.h` are those same defaults, spelled out explicitly so the timing does
not depend on QMK's fallback values:

- `I2C1_TIMINGR_PRESC = 0`
- `I2C1_TIMINGR_SCLDEL = 7` (8 × 15.625ns = 125ns data setup, meets FM 100ns minimum)
- `I2C1_TIMINGR_SDADEL = 0`
- `I2C1_TIMINGR_SCLH = 38`
- `I2C1_TIMINGR_SCLL = 129`

At the 64MHz PCLK on this board this gives f_SCL = 64MHz / 169 ≈ 378kHz,
which is valid Fast-mode.

### Voltage notes

VBUS provides 5V to the slave side, with I2C lines pulled up to 5V
via 5kΩ resistors. PB6/PB7 are 5V tolerant (FTf) on STM32G0B1.

## License

This keyboard definition is licensed under the [GPL v2](https://www.gnu.org/licenses/old-licenses/gpl-2.0.html).