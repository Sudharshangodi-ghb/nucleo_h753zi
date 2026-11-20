# Setup and operation

These steps assume Ubuntu and a checkout of this repository. The CMSIS headers,
startup file, and linker script are already included; there is no need to
recreate the project directories or copy those files from STM32CubeH7.

## 1. Install build and board tools

```bash
sudo apt update
sudo apt install -y \
    build-essential cmake git \
    gcc-arm-none-eabi binutils-arm-none-eabi \
    openocd gdb-multiarch picocom
```

Confirm the main tools are available:

```bash
cmake --version
arm-none-eabi-gcc --version
openocd --version
```

## 2. Configure and build

From the project root, configure and build:

```bash
cmake -S . -B build
cmake --build build
```

The ELF, raw binary, and linker map are written to `build/`.

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

## 3. Flash and view UART output

Connect the board through the ST-LINK USB connector (CN1), then flash:

```bash
openocd \
    -f interface/stlink.cfg \
    -f target/stm32h7x.cfg \
    -c "program build/nucleo_h753zi.elf verify reset exit"
```

The ST-LINK Virtual COM Port is connected to USART3: TX is PD8 and RX is PD9.
Open the detected serial device, for example:

```bash
picocom -b 115200 /dev/ttyACM0
```

The firmware uses 115200 baud, 8-N-1. It currently assumes a 64 MHz USART3
kernel clock; recalculate the baud divisor if the clock configuration changes.
Check the board revision and solder bridges against the
[NUCLEO-H753ZI user manual](Docs/um2407-stm32h7-nucleo144-boards-mb1364-stmicroelectronics.pdf).

## 4. Run in Renode (optional)

Install Renode and ensure its executable is on `PATH`. The portable Linux
release is available from the
[Renode download site](https://renode.io/#downloads).

Run the project launcher:

```bash
./renode/run.sh
```

At the monitor prompt:

```text
logLevel -1 sysbus.gpioPortB.GreenLED
start
```

Expected output includes USART3 application messages and GreenLED state
changes. Exit with `quit`.

The launcher resolves the ELF from this checkout and looks for
`platforms/boards/nucleo_h753zi.repl` next to the Renode executable. Override
the paths when necessary:

```bash
RENODE=/path/to/renode \
RENODE_PLATFORM_FILE=/path/to/platforms/boards/nucleo_h753zi.repl \
./renode/run.sh
```

Set `FIRMWARE_ELF=/path/to/firmware.elf` to load a different ELF. The launcher
creates and removes its temporary Renode script automatically.

## Troubleshooting

| Problem | Check |
|---|---|
| `arm-none-eabi-gcc` not found | Install `gcc-arm-none-eabi` and check `PATH` |
| Renode platform not found | Set `RENODE_PLATFORM_FILE` to `nucleo_h753zi.repl` |
| ST-LINK not detected | Check USB connection, board power, `lsusb`, and OpenOCD configuration |
| No UART output | Check `/dev/ttyACM*`, PD8/PD9 routing, solder bridges, and baud rate |
| Renode reports unmapped peripheral accesses | These may reflect simulator-model limits; validate behavior on hardware |

Use CN1 for ST-LINK and board power. CN13 is the USB OTG connector, not the
board's power input. Verify jumper and solder-bridge settings in the board
manual before changing them.
