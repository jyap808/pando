# Features

[Pando](https://github.com/jyap808/pando/) is Open Hardware, licensed under the [CERN-OHL-S v2](https://ohwr.org/cern_ohl_s_v2.txt).

- **MCU:** [STM32G0B1KBT6](https://www.st.com/en/microcontrollers-microprocessors/stm32g0b1kb.html). This is a newer MCU that offers benefits over an older, more commonly used MCU such as the STM32F072CBT6:
    - 64 MHz ARM Cortex-M0+ core, up from 48 MHz
    - 144 KB SRAM, up from 16 KB (~9x)
    - Consolidated single-supply core (VDD) with a modern power layout, reducing decoupling capacitors from 6 to 1
    - Streamlined LQFP-32 package instead of LQFP-48, keeping the same 7x7 mm body with a larger 0.875 mm pin pitch for easier soldering
    - Active ST support, keeping the design future-proof
- **IO expander:** PCA9555
- **Split communication:** USB-C with ESD protection on all ports
- **Switches:** Hotswap sockets
- **Assembly:** Single-sided SMD components for hotplate soldering

The project uses [Ergogen](https://github.com/ergogen/ergogen) for layout generation and [KiCad](https://www.kicad.org/) for schematic/PCB design.
