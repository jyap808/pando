# Installing Firmware

> **Note:** Pando is a split keyboard with an MCU on the master half only. The secondary half uses an I/O expander over I2C, so only the master MCU needs to be flashed.

There are two ways to get the latest Vial firmware for Pando:

1. **Precompiled firmware** — Download precompiled firmware from [GitHub Releases](https://github.com/jyap808/pando/releases) and flash via `dfu-util` (simplest option).
2. **Manual build method** — Clone the vial-qmk repo, build locally, and flash via CLI.

## Prerequisites: Enable the Boot Button (Legacy Boot Mode)

The STM32G0 series handles booting differently from older STM32 parts. Booting is controlled by a software configuration bit called `nBOOT_SEL` in the Option Bytes rather than purely by hardware.

By default, `nBOOT_SEL` is set to `1`, which tells the MCU to **ignore the physical BOOT0 pin** entirely. Instead it uses an **Empty Check** mechanism: a blank chip automatically enters DFU mode, but once code is flashed, it always boots straight into your firmware — ignoring BOOT0 even if it's held HIGH.

### Why?

The BOOT0 pin on the G0 is multiplexed with GPIOs and programming lines (e.g. SWCLK on PA14). Disabling it by default lets designers use that pin as a regular I/O without risking accidental bootloader entry.

### Enabling Legacy Boot Behavior

To restore hardware BOOT0 control (so a physical button can force DFU mode at any time), clear the `nBOOT_SEL` bit. This can be done via the GUI or the CLI, both using the [STM32CubeProgrammer](https://www.st.com/en/development-tools/stm32cubeprog.html) tool.

**GUI method:** Open STM32CubeProgrammer → Option Bytes → User Configuration, uncheck `nBOOT_SEL`, and apply.

**CLI method:** Use `STM32_Programmer_CLI` to list devices, display option bytes, and set the bit.

First, list available DFU devices:

    $ STM32_Programmer_CLI -l

    =====  DFU Interface   =====

    Total number of available STM32 device in DFU mode: 1

      Device Index           : USB1
      Product ID             : DFU in FS Mode
      Device ID              : 0x0467

Display current option bytes (factory default shows `nBOOT_SEL : 0x1`):

    $ STM32_Programmer_CLI -c port=usb1 -ob displ

      User Configuration:

         nBOOT_SEL    : 0x1 (BOOT0 signal is defined by nBOOT0 option bit)

Clear `nBOOT_SEL` to enable legacy boot pin behavior:

    $ STM32_Programmer_CLI -c port=usb1 -ob nBOOT_SEL=0

    OPTION BYTE PROGRAMMING VERIFICATION:

    Option Bytes successfully programmed

Confirm the change (`nBOOT_SEL` now shows `0x0`):

    $ STM32_Programmer_CLI -c port=usb1 -ob displ

      User Configuration:

         nBOOT_SEL    : 0x0 (BOOT0 signal is defined by BOOT0 pin value (legacy mode))

### Recommended Setup Sequence

Clear the option byte on a fresh chip **before** flashing firmware. This ensures the BOOT0 pin is active even if your first firmware build has a catastrophic bug, guaranteeing you can always recover via DFU:

**Brand New MCU** → **Clear nBOOT_SEL** → **Test Bootloader** → **Flash Firmware**

## Precompiled Method: Download from Releases + `dfu-util`

1. Go to the [Releases page](https://github.com/jyap808/pando/releases)

2. Download the latest firmware file (e.g. `jyap808_pando_vial.bin`)

3. Put the keyboard into DFU mode by holding the boot/reset button on the MCU while plugging in the USBC cable.

4. Flash the firmware with `dfu-util`:

        dfu-util -d 0483:df11 -a 0 -s 0x08000000:leave -D jyap808_pando_vial.bin

    The device should show up as an STM32 BOOTLOADER. On Linux you may need `sudo` or a udev rule for the DFU device.

    A quick breakdown of the `dfu-util` flags:

    - `-d 0483:df11` — the USB VID:PID of ST's STM32 bootloader, which all STM32-system-DFU devices enumerate as.
    - `-a 0` — select alternate 0, the internal flash media. This is the same alternate QMK uses when flashing `stm32-dfu` boards.
    - `-s 0x08000000:leave` — program at `0x08000000`, the standard STM32G0 flash base address, then `leave` to jump to the application on completion.

5. The keyboard will reboot with the new firmware

## Manual Build Method: CLI Build + Flash (Advanced)

1. Clone the official Vial QMK fork:

        git clone https://github.com/vial-kb/vial-qmk.git
        cd vial-qmk

2. Initialize submodules (important for QMK dependencies):

        make git-submodule

3. Clone or download the Pando keyboard folder from the [GitHub repo](https://github.com/jyap808/pando).

    Copy the entire `keyboards/jyap808/pando/` folder into `vial-qmk/keyboards/jyap808/pando/`.

4. Compile the Vial firmware:

        make jyap808/pando:vial

5. Flash using the CLI.

    For an STM32 target this uses USB DFU. You can use QMK's wrapper, which runs `dfu-util` under the hood:

        qmk flash jyap808/pando:vial
