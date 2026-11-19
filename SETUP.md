# STM32H753ZI Bare-Metal Setup Guide

This document describes how to prepare an Ubuntu Linux development environment for bare-metal firmware development on the STM32H753ZI NUCLEO-144 board.

The project uses the GNU Arm Embedded Toolchain, CMSIS, GNU Make, OpenOCD and GDB Multiarch.

No STM32 HAL, LL or STM32CubeIDE-generated application code is used.

# 1. Development Environment

| Component | Tool |
|---|---|
| Operating system | Ubuntu Linux 64-bit |
| Shell | Bash |
| Compiler | arm-none-eabi-gcc |
| Binary utilities | GNU Binutils |
| Build system | GNU Make |
| Debugger | GDB Multiarch |
| Flash programmer | OpenOCD |
| Serial terminal | picocom |
| Code editor | VS Code (optional) |

# 2. Install Development Tools

Update Ubuntu:

```bash
sudo apt update
sudo apt upgrade -y
```

Install essential utilities:

```bash
sudo apt install -y \
    build-essential \
    make \
    git \
    wget \
    curl \
    unzip \
    xz-utils
```

Install GNU Arm Embedded Toolchain:

```bash
sudo apt install -y \
    gcc-arm-none-eabi \
    binutils-arm-none-eabi
```

Install GDB Multiarch:

```bash
sudo apt install -y gdb-multiarch
```

Install OpenOCD:

```bash
sudo apt install -y openocd
```

Install picocom:

```bash
sudo apt install -y picocom
```

# 3. Verify Toolchain Installation

Check GCC:

```bash
arm-none-eabi-gcc --version
```

Check binary utilities:

```bash
arm-none-eabi-objcopy --version
arm-none-eabi-size --version
```

Check debugger:

```bash
gdb-multiarch --version
```

Check OpenOCD:

```bash
openocd --version
```

Check Make:

```bash
make --version
```

Check picocom:

```bash
picocom --version
```

The actual tool versions depend on the Ubuntu release and package repositories.

# 4. Create Project Workspace

Create the workspace:

```bash
mkdir -p ~/baremetal
cd ~/baremetal
```

Create the H753ZI project:

```bash
mkdir -p nucleo_h753zi
cd nucleo_h753zi
```

Create the project directories:

```bash
mkdir -p CMSIS/Include
mkdir -p CMSIS/Device/ST/STM32H7xx/Include

mkdir -p Core/Inc
mkdir -p Core/Src

mkdir -p Drivers/Ethernet
mkdir -p Drivers/GPIO
mkdir -p Drivers/UART
mkdir -p Linker
mkdir -p Startup
mkdir -p build
```

Project location:

```text
~/baremetal/nucleo_h753zi
```

# 5. CMSIS and STM32H7 Device Header Setup

CMSIS provides the ARM Cortex-M7 core definitions and STM32H7 peripheral register definitions.

Official STM32CubeH7 repository:

https://github.com/STMicroelectronics/STM32CubeH7

CMSIS Core repository:

https://github.com/ARM-software/CMSIS_5

## 5.1 CMSIS Core headers

Required headers include:

```text
CMSIS/Include/
├── cmsis_compiler.h
├── cmsis_gcc.h
├── cmsis_version.h
├── core_cm7.h
└── m-profile/
    ├── armv7m_cachel1.h
    ├── armv7m_mpu.h
    └── cmsis_gcc_m.h
```

Use the CMSIS Core version compatible with the STM32CubeH7 package.

## 5.2 STM32H753 device headers

Required headers:

```text
CMSIS/Device/ST/STM32H7xx/Include/
├── stm32h753xx.h
├── stm32h7xx.h
└── system_stm32h7xx.h
```

These headers provide:

- Peripheral base addresses.
- Register structure definitions.
- Register bit definitions.
- Interrupt definitions.
- Cortex-M7 device integration.

The device selection macro is:

```c
STM32H753xx
```

## 5.3 Locate the vendor files

After downloading or cloning STM32CubeH7, locate the required files:

```bash
find /path/to/STM32CubeH7 \
    -name "stm32h753xx.h" \
    -o -name "stm32h7xx.h" \
    -o -name "system_stm32h7xx.h"
```

Locate the CMSIS Core headers:

```bash
find /path/to/STM32CubeH7 \
    -name "core_cm7.h" \
    -o -name "cmsis_gcc.h" \
    -o -name "armv7m_mpu.h"
```

Copy the required files into the corresponding project directories.

# 6. Startup Assembly Setup

Use the official STM32H753 startup assembly file from STM32CubeH7.

Required file:

```text
Startup/startup_stm32h753xx.s
```

Locate it in the vendor package:

```bash
find /path/to/STM32CubeH7 \
    -name "startup_stm32h753xx.s"
```

The startup file provides:

1. Interrupt vector table.
2. Initial stack pointer.
3. Reset handler.
4. `SystemInit()` invocation.
5. `.data` section initialization.
6. `.bss` section initialization.
7. C runtime initialization.
8. Transfer of control to `main()`.

# 7. Linker Script Setup

Use the STM32H753ZIT6 Flash linker script supplied with the STM32CubeH7 toolchain examples.

Expected project file:

```text
Linker/STM32H753ZITX_FLASH.ld
```

Locate candidate linker scripts:

```bash
find /path/to/STM32CubeH7 \
    -iname "*H753*FLASH*.ld"
```

Inspect the selected linker script before copying it.

### Memory architecture

The STM32H753ZI provides:

| Memory | Size |
|---|---:|
| ITCM RAM | 64 KB |
| DTCM RAM | 128 KB |
| User SRAM | 864 KB |
| Backup SRAM | 4 KB |
| Flash | 2 MB |

The linker script must define the memory regions and section placement appropriate for the application.

Use a linker script configured for the STM32H753ZI memory map.

For DMA and Ethernet applications, memory accessibility and cache coherency must also be considered.

# 8. System Initialization

The project uses:

```text
Core/Src/system_stm32h7xx.c
```

The STM32H753ZI uses a Cortex-M7 core with a maximum frequency of 480 MHz,
64 MHz HSI, a double-precision FPU, 16 KB instruction and data caches, and
multiple clock domains across its Flash, ITCM, DTCM, and SRAM.

The H753 firmware must explicitly configure and verify its clock tree.

Configure `SystemInit()` for the STM32H753ZI clock tree and memory system.

# 9. Configure the Makefile

Target architecture:

```makefile
CPU_FLAGS = -mcpu=cortex-m7 -mthumb \
            -mfpu=fpv5-d16 -mfloat-abi=hard
```

Device definition:

```makefile
DEFINES = -DSTM32H753xx
```

Include directories:

```makefile
INCLUDES = -ICore/Inc \
           -ICMSIS/Include \
           -ICMSIS/Device/ST/STM32H7xx/Include \
           -IDrivers/Ethernet \
           -IDrivers/GPIO \
           -IDrivers/UART
```

The Makefile must:

- Compile application C sources.
- Assemble startup assembly.
- Link with the H753 Flash linker script.
- Generate ELF and BIN files.
- Generate a linker map file.
- Display firmware memory usage.

Expected build artifacts:

```text
build/
├── nucleo_h753zi.elf
├── nucleo_h753zi.bin
└── nucleo_h753zi.map
```

# 10. Connect and Detect ST-LINK V3E

Connect the NUCLEO-H753ZI to the Ubuntu PC using the ST-LINK USB connector CN1.

The board has an embedded STLINK-V3E debugger/programmer.

Check USB enumeration:

```bash
lsusb
```

Check serial interfaces:

```bash
ls -l /dev/ttyACM*
```

Check USB device messages:

```bash
dmesg | tail -n 40
```

The serial device name may differ depending on the connected USB devices.

# 11. Linux Serial Port Permissions

Check the current user groups:

```bash
groups
```

If the serial port requires dialout access:

```bash
sudo usermod -aG dialout $USER
```

Log out and log back in for the group change to take effect.

# 12. Build the Firmware

Navigate to the project:

```bash
cd ~/baremetal/nucleo_h753zi
```

Clean previous build artifacts:

```bash
make clean
```

Build the firmware:

```bash
make
```

Check firmware size:

```bash
arm-none-eabi-size build/nucleo_h753zi.elf
```

Inspect ELF sections:

```bash
arm-none-eabi-objdump -h build/nucleo_h753zi.elf
```

# 13. Flash Using OpenOCD

Connect the board and run:

```bash
openocd \
    -f interface/stlink.cfg \
    -f target/stm32h7x.cfg \
    -c "program build/nucleo_h753zi.elf verify reset exit"
```

The operation performs:

1. ST-LINK initialization.
2. STM32H753 target detection.
3. Flash programming.
4. Flash verification.
5. Target reset.

Expected successful output includes:

```text
** Programming Started **
** Programming Finished **
** Verify Started **
** Verified OK **
** Resetting Target **
```

# 14. UART Debugging Using ST-LINK VCP

The default ST-LINK Virtual COM Port is connected to USART3.

| Signal | MCU pin |
|---|---|
| USART3 TX | PD8 |
| USART3 RX | PD9 |

Default solder bridges:

- SB19 ON.
- SB12 ON.

Open the serial terminal:

```bash
picocom -b 115200 /dev/ttyACM0
```

The initial firmware target is 115200 baud, 8-N-1.

The baud-rate configuration must be derived from the actual USART3 kernel clock.

# 15. Board Hardware Configuration

### Default power configuration

For normal USB-powered development, check the board's default jumper configuration:

| Jumper | Default setting |
|---|---|
| JP1 | OFF |
| JP2 | ON, pins 1-2 |
| JP3 | ON |
| JP4 | ON |
| JP5 | ON, pins 1-2 (3.3 V) |

Verify the actual board revision and jumper positions against the official user manual before changing them.

### HSE clock source

The board's default HSE input is an 8 MHz MCO output from ST-LINK.

The clock configuration must match the physical board solder-bridge configuration.

### Important USB note

CN1 is the ST-LINK USB connector.

CN13 is the USB OTG connector. It must not be used as the board's power input. Power the NUCLEO-H753ZI separately before connecting CN13 for USB OTG operation.

# 16. Complete Build and Flash Workflow

```bash
cd ~/baremetal/nucleo_h753zi

make clean
make

openocd \
    -f interface/stlink.cfg \
    -f target/stm32h7x.cfg \
    -c "program build/nucleo_h753zi.elf verify reset exit"

picocom -b 115200 /dev/ttyACM0
```

# 17. Troubleshooting

### Compiler not found

```bash
which arm-none-eabi-gcc
```

Install the GNU Arm toolchain if it is missing.

### ST-LINK not detected

```bash
lsusb
dmesg | tail -n 40
```

Check USB connection, board power and ST-LINK enumeration.

### OpenOCD cannot detect the target

Check:

- ST-LINK V3E USB connection.
- Board power.
- SWD connection.
- OpenOCD STM32H7 target configuration.
- Debug reset configuration.

### UART terminal is blank

Check:

1. USART3 is enabled in RCC.
2. PD8 and PD9 are configured for the correct alternate function.
3. USART3 clock source and baud-rate calculation match.
4. SB12 and SB19 are ON.
5. The correct `/dev/ttyACM*` device is selected.
6. The firmware has been rebuilt and flashed.

### Firmware faults after enabling the cache

Check:

- Cache maintenance for DMA buffers.
- MPU memory attributes.
- Buffer alignment.
- Memory-region accessibility.
- Correct placement of interrupt vectors and application sections.

---

## Official References

- STM32H753ZI product page: https://www.st.com/en/microcontrollers-microprocessors/stm32h753zi.html
- NUCLEO-H753ZI product page: https://www.st.com/en/evaluation-tools/nucleo-h753zi.html
- STM32H7 Nucleo-144 User Manual UM2407: https://www.st.com/resource/en/user_manual/um2407-stm32h7-nucleo144-board-stmicroelectronics.pdf
- STM32CubeH7: https://github.com/STMicroelectronics/STM32CubeH7
- CMSIS: https://github.com/ARM-software/CMSIS_5

**Setup guide scope:** Ubuntu Linux environment, GNU Arm toolchain, CMSIS integration, startup assembly, linker configuration, Makefile, ST-LINK V3E, OpenOCD and UART terminal setup.


# 18. Install Renode for STM32H753ZI Simulation

Renode is used to execute and inspect the firmware in a simulated NUCLEO-H753ZI environment before physical-board testing.

The current development environment uses the portable Renode distribution.

## 18.1 Download and extract Renode

Create a tools directory:

```bash
mkdir -p ~/tools
cd ~/tools
```

Download and extract the portable Linux build:

```bash
wget https://builds.renode.io/renode-latest.linux-portable.tar.gz
mkdir -p renode
tar -xzf renode-latest.linux-portable.tar.gz -C renode
```

The exact archive extraction layout can vary by release. Confirm the location of the `renode` executable:

```bash
find ~/tools/renode -type f -name renode
```

If the executable is directly at `~/tools/renode/renode`, add it to PATH:

```bash
echo 'export PATH="$HOME/tools/renode:$PATH"' >> ~/.bashrc
source ~/.bashrc
```

Verify the installation:

```bash
which renode
renode --version
renode --help
```

The version observed during this project setup was Renode 1.17.0. The version installed from `renode-latest` can change over time.

## 18.2 Confirm the NUCLEO-H753ZI platform files

The Renode installation should contain the board and CPU platform descriptions:

```text
platforms/
├── boards/
│   └── nucleo_h753zi.repl
└── cpus/
    └── stm32h753.repl
```

Check the board file:

```bash
test -f ~/tools/renode/platforms/boards/nucleo_h753zi.repl \
    && echo "NUCLEO-H753ZI platform file found"
```

Check the CPU file:

```bash
test -f ~/tools/renode/platforms/cpus/stm32h753.repl \
    && echo "STM32H753 CPU platform file found"
```

The board description defines the board-level peripherals and LED/button connections. The CPU description supplies the STM32H753-compatible processor/peripheral model.

# 19. Project Renode Script

The project-specific script is:

```text
renode/nucleo_h753zi.resc
```

The current script is:

```resc
mach create "nucleo_h753zi"
machine LoadPlatformDescription @/home/sudharshanreddy/tools/renode/platforms/boards/nucleo_h753zi.repl
cpu VectorTableOffset 0x08000000
sysbus LoadELF @/home/sudharshanreddy/baremetal/nucleo_h753zi/build/nucleo_h753zi.elf
showAnalyzer sysbus.usart3
```

This script uses absolute paths from the current development machine. If the project or Renode installation is moved, update those paths. A later improvement can make the script relocatable.

## 19.1 What each command does

| Renode command | Purpose |
|---|---|
| `mach create` | Creates the simulated machine |
| `machine LoadPlatformDescription` | Loads the NUCLEO-H753ZI board model |
| `cpu VectorTableOffset` | Sets the vector-table base address |
| `sysbus LoadELF` | Loads the ELF sections into simulated memory |
| `showAnalyzer sysbus.usart3` | Displays USART3 output in the console |
| `start` | Starts CPU emulation |

The ELF is loaded at its linked memory addresses. It contains machine code, section placement information and symbols. The `.bin` file is a raw binary and does not carry the same ELF metadata.

# 20. Build and Run Firmware in Renode

Build the firmware first:

```bash
cd ~/baremetal/nucleo_h753zi
make clean
make
```

Confirm that the ELF exists:

```bash
test -f build/nucleo_h753zi.elf \
    && echo "Firmware ELF generated"
```

Launch Renode:

```bash
renode --console --disable-gui --plain renode/nucleo_h753zi.resc
```

At the Renode monitor prompt, enable GPIO LED state logging:

```text
logLevel -1 sysbus.gpioPortB.GreenLED
```

Start the simulation:

```text
start
```

Expected output includes:

- CPU initial PC and SP values.
- Firmware initialization messages through USART3.
- GreenLED state changes from the GPIO/LED model.

Example LED state messages:

```text
[NOISY] gpioPortB.GreenLED: LED state changed to True
[NOISY] gpioPortB.GreenLED: LED state changed to False
```

The UART analyzer output and LED state logger are separate observation paths:
- USART3 messages are generated by firmware UART transmissions.
- LED state messages are generated by Renode's LED model after GPIO transitions.

To query the LED state manually at the Renode monitor:

```text
sysbus.gpioPortB.GreenLED State
```

# 21. Understanding Firmware Loading and Startup in Renode

The high-level execution sequence is:

```text
GNU Arm toolchain
    |
    +-- Compile application C files
    +-- Assemble startup file
    +-- Link using STM32H753 linker script
    |
    +-- build/nucleo_h753zi.elf
              |
              v
Renode loads NUCLEO-H753ZI platform
              |
              v
Renode loads ELF sections into simulated memory
              |
              v
Renode starts Cortex-M7 CPU model
              |
              v
Reset_Handler
    |
    +-- ExitRun0Mode()
    +-- SystemInit()
    +-- Copy .data to SRAM
    +-- Clear .bss
    +-- __libc_init_array()
    +-- main()
          |
          +-- App_Init()
          +-- App_Run()
              |
              +-- GPIO and USART register accesses
              +-- Renode peripheral models respond
```

The CPU executes compiled ARM machine instructions, not C source code. Memory-mapped peripheral accesses are routed by Renode's system bus to the simulated peripheral models.

In the current run, Renode reported these ELF load blocks:

```text
Loading block of 2028 bytes length at 0x08000000.
Loading block of 1536 bytes length at 0x080007EC.
```

The second block is the initialized data load image; startup code copies it to the `.data` runtime address in SRAM.

The CPU startup log reported:

```text
PC = 0x080006BD
SP = 0x24080000
```

`PC` points to the reset handler, while `SP` is the initial stack pointer.

# 22. Stop Renode

To stop the emulation, use the Renode monitor command:

```text
quit
```

If the monitor is unresponsive, open another terminal and list Renode
processes:

```bash
ps -eo pid,user,args | grep '[r]enode'
```

Check the user, command line, and PID so you only signal the Renode process
you intend to stop. To stop one instance, substitute its exact PID for
`12345`:

```bash
kill -INT 12345
```

If you intentionally want to stop all Renode instances owned by your user,
review the list first and pass each verified PID explicitly:

```bash
kill -INT 12345 23456 34567
```

Only include PIDs that belong to Renode sessions you intend to stop. Do not
signal unrelated users' processes.

If one selected instance remains alive, recheck that exact PID and request
termination:

```bash
ps -p 12345 -o pid,user,stat,args
kill -TERM 12345
```

# 23. Simulation Limitations and Physical-Board Validation

Renode simulation is useful for early firmware execution and peripheral interaction checks, but it is not equivalent to physical STM32H753ZI hardware.

The current simulation has successfully demonstrated:

- ELF loading into the Renode platform.
- Cortex-M7 startup reaching the application.
- USART3 output in the UART analyzer.
- GPIOB pin 0 transitions observed by the GreenLED model.

The current run also reported accesses to unmapped addresses `0x5C001000` and `0x51008108`. These warnings did not prevent the demonstrated application behavior, but their origin and impact should be investigated before treating the simulation as a complete hardware validation.

Physical-board checks remain pending until the NUCLEO-H753ZI board is available:

- ST-LINK programming and verification.
- Actual LED electrical behavior.
- USART3 VCP routing and baud-rate verification.
- Clock configuration against the board's actual clock source.
- Peripheral timing and hardware-specific behavior.

Renode success should therefore be recorded as simulation validation, not physical-board bring-up.
