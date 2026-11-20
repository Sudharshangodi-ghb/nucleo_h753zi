# STM32H753ZI Bare-Metal Firmware

Register-level firmware for the STM32H753ZI NUCLEO-144 board. The project uses
CMSIS and the GNU Arm toolchain; it does not use STM32 HAL, LL, an RTOS, or
STM32CubeIDE-generated application code.

## Build and run

Install the prerequisites in [SETUP.md](SETUP.md), then configure and build
from the project root:

```bash
cmake -S . -B build
cmake --build build
```

The build produces `build/nucleo_h753zi.elf`, `.bin`, and `.map`.

To remove the build directory:

```bash
cmake -E rm -rf build
```

To rebuild from scratch:

```bash
cmake -E rm -rf build
cmake -S . -B build
cmake --build build
```

To simulate, run `./renode/run.sh`. At the Renode monitor:

```text
logLevel -1 sysbus.gpioPortB.GreenLED
start
```

Use `quit` to exit. The launcher finds the ELF in this checkout and the board
description next to Renode. Set `RENODE` or `RENODE_PLATFORM_FILE` if your
installation uses a different location.

To flash a connected board:

```bash
openocd \
    -f interface/stlink.cfg \
    -f target/stm32h7x.cfg \
    -c "program build/nucleo_h753zi.elf verify reset exit"
```

## Project layout

| Path | Purpose |
|---|---|
| `Core/` | Application entry point, app logic, and CMSIS system initialization |
| `Drivers/GPIO/` | GPIO and LD1 drivers |
| `Drivers/UART/` | Polling USART3 output |
| `Drivers/Ethernet/` | Ethernet MAC/DMA setup and LAN8742A status probe |
| `CMSIS/` | Cortex-M7 and STM32H753 device headers |
| `Startup/` | Reset handler and interrupt vector table |
| `Linker/` | Flash and RAM layout |
| `CMakeLists.txt` and subdirectory CMake files | One firmware target and automatic source discovery |
| `renode/run.sh` | Portable Renode launcher |

## Current status

- Startup, linker layout, CMSIS integration, GPIO, and USART3 are included.
- LD1 is driven on PB0. USART3 uses PD8/PD9 at 115200 baud and currently
  assumes a 64 MHz peripheral clock.
- The system initialization uses the reset clock configuration; a final PLL
  clock setup has not been implemented.
- Ethernet initialization configures RMII, DMA descriptors, and reads PHY
  status. Frame transmit and payload delivery to the application are not
  implemented; there is no LwIP integration yet.
- Renode has been used to check firmware loading, LED transitions, and UART
  output. Simulation does not replace physical-board validation.

## Component notes

- [Setup, flashing, UART, and Renode instructions](SETUP.md)
- [Ethernet driver status and hardware details](Drivers/Ethernet/README.md)
- [GPIO and LED driver](Drivers/GPIO/README.md)
- [USART3 driver](Drivers/UART/README.md)
- [Reference manuals, datasheets, and schematics](Docs/README.md)
