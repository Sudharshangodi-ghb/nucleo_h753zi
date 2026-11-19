# STM32H753ZI Bare-Metal Development

A register-level bare-metal firmware project for the **STM32H753ZI NUCLEO-144** development board.

The project focuses on understanding the STM32H7 hardware architecture through direct peripheral register programming using CMSIS and the GNU Arm Embedded Toolchain.

No STM32 HAL, LL, RTOS or STM32CubeIDE-generated application code is used.

## 1. Project Overview

The objective is to build a reusable bare-metal firmware foundation for the STM32H753ZI.

### Development objectives

- Bare-metal startup and reset handling.
- Custom linker script and memory configuration.
- Cortex-M7 system initialization.
- Clock-tree configuration.
- GPIO configuration and LED control.
- USART3 debug logging.
- Interrupt and NVIC management.
- Timer and peripheral driver development.
- GNU Make-based build system.
- OpenOCD-based firmware programming and debugging.

## 2. Hardware Platform

| Parameter | Configuration |
|---|---|
| Development board | NUCLEO-H753ZI |
| Board reference | MB1364 |
| Microcontroller | STM32H753ZIT6 |
| Processor | ARM Cortex-M7 |
| Maximum CPU frequency | 480 MHz |
| Flash | 2 MB |
| RAM | 1 MB |
| FPU | Double-precision FPU |
| Cache | 16 KB instruction + 16 KB data cache |
| Debug interface | ST-LINK V3E |
| Board type | Nucleo-144 |

### MCU memory architecture

The STM32H753ZI provides:

- 64 KB ITCM RAM.
- 128 KB DTCM RAM.
- 864 KB user SRAM.
- 4 KB backup SRAM.
- 2 MB dual-bank Flash.

Memory placement must be considered when developing DMA, Ethernet and other high-performance peripheral applications.

## 3. Software Environment

The project is developed using Ubuntu Linux.

| Component | Tool |
|---|---|
| Operating system | Ubuntu Linux |
| Compiler | arm-none-eabi-gcc |
| Binary utilities | GNU Binutils |
| Build system | GNU Make |
| Debugger | GDB Multiarch |
| Flash programmer | OpenOCD |
| Serial terminal | picocom |
| Code editor | VS Code (optional) |

Refer to [SETUP.md](SETUP.md) for complete environment installation and project setup instructions.

## 4. Repository Structure

```text
nucleo_h753zi/
│
├── CMSIS/
│   ├── Include/
│   └── Device/
│       └── ST/
│           └── STM32H7xx/
│               └── Include/
│
├── Core/
│   ├── Inc/
│   │   └── app.h
│   │
│   └── Src/
│       ├── app.c
│       ├── main.c
│       └── system_stm32h7xx.c
│
├── Drivers/
│   ├── Ethernet/
│   │   ├── ethernet_mac.c
│   │   ├── ethernet_mac.h
│   │   └── README.md
│   │
│   ├── GPIO/
│   │   ├── gpio.c
│   │   ├── gpio.h
│   │   ├── led.c
│   │   ├── led.h
│   │   └── README.md
│   │
│   └── UART/
│       ├── uart3.c
│       ├── uart3.h
│       └── README.md
│
├── Linker/
│   └── STM32H753ZITX_FLASH.ld
│
├── Startup/
│   └── startup_stm32h753xx.s
│
├── build/
│
├── Makefile
├── README.md
└── SETUP.md
```

## 5. Application Architecture

The application separates hardware initialization from the main application loop.

```text
                  main.c
                    |
             +------+------+
             |             |
          App_Init()     App_Run()
             |             |
        Peripheral      Application
        initialization  execution
             |             |
       +-----+-----+----+
       |     |          |
  Led_Init Uart3_Init Ethernet_Init
       |     |          |
       +-----------+-------+
                   |
             Application logic
                   |
                  Repeat
```

### Source-file responsibilities

| File | Responsibility |
|---|---|
| `main.c` | Application entry point |
| `app.h` | Application interface declarations |
| `app.c` | Application and peripheral functionality |
| `system_stm32h7xx.c` | System initialization |
| `startup_stm32h753xx.s` | Reset handler and vector table |
| `STM32H753ZITX_FLASH.ld` | Flash and RAM memory layout |
| `Makefile` | Compilation and linking |

The entry point remains minimal:

```c
#include "app.h"

int main(void)
{
    App_Init();

    App_Run();

    while (1)
    {
    }
}
```

## 6. Initial Peripheral Development

### 6.1 GPIO – User LEDs

The NUCLEO-H753ZI provides three user LEDs.

| LED | Color | MCU pin |
|---|---|---|
| LD1 | Green | PB0 (default configuration) |
| LD2 | Yellow | PE1 |
| LD3 | Red | PB14 |

LD1 can alternatively be connected to PA5 by changing the board solder-bridge configuration.

The user LEDs are active-high.

**Initial implementation target:** LD1 green LED blinking using PB0.

### 6.2 USART3 – UART Debug Logging

The default ST-LINK Virtual COM Port is connected to USART3.

| Configuration | Value |
|---|---|
| Peripheral | USART3 |
| TX | PD8 |
| RX | PD9 |
| Alternate function | AF7 |
| Baud rate | 115200 (initial target) |
| Data bits | 8 |
| Parity | None |
| Stop bits | 1 |
| Flow control | None |

Default VCP solder-bridge configuration:

| Solder bridge | Configuration |
|---|---|
| SB19 | ON |
| SB12 | ON |
| SB9, SB18, SB82 | OFF |
| SB34, SB66, SB81 | OFF |

The UART clock and baud-rate divisor must be calculated from the actual USART3 kernel clock selected by the H7 clock configuration.

**Initial implementation target:** USART3 polling-based debug transmission.

## 7. System Clock and Cortex-M7 Configuration

The STM32H753ZI provides multiple clock sources and clock domains.

Available clock sources include:

- HSI: 64 MHz.
- HSE: external clock source.
- CSI: 4 MHz.
- HSI48: 48 MHz.
- LSI: 32 kHz.
- LSE: 32.768 kHz.

The NUCLEO-H753ZI board provides an 8 MHz ST-LINK MCO clock input as its default HSE clock arrangement.

The system clock configuration will be implemented explicitly in the firmware.

The initial implementation will prioritize a known, stable clock configuration before increasing the system clock toward the 480 MHz operating limit.

### Cortex-M7 considerations

The Cortex-M7 introduces additional configuration considerations:

- FPU initialization.
- Instruction cache.
- Data cache.
- MPU configuration.
- TCM and SRAM memory placement.
- Cache coherency for DMA buffers.
- Memory and peripheral bus domains.

Cache and MPU configuration will be introduced deliberately as the project progresses.

## 8. Build Configuration

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

The build system will generate:

```text
build/
├── nucleo_h753zi.elf
├── nucleo_h753zi.bin
└── nucleo_h753zi.map
```

## 9. Development Workflow

Build:

```bash
make clean
make
```

Flash and verify:

```bash
openocd \
    -f interface/stlink.cfg \
    -f target/stm32h7x.cfg \
    -c "program build/nucleo_h753zi.elf verify reset exit"
```

Open UART terminal:

```bash
picocom -b 115200 /dev/ttyACM0
```

The UART settings must match the firmware configuration.

## 10. Implementation Status

| Feature | Status |
|---|---|
| Ubuntu development environment | Configured |
| GNU Arm toolchain configuration | Configured |
| CMSIS integration | Integrated |
| Startup assembly | Integrated |
| Linker configuration | Integrated; ELF layout inspected |
| System initialization | Builds and executes in Renode |
| Makefile build system | Builds ELF and BIN successfully |
| GPIO LED control | Executed and observed in Renode |
| USART3 debug logging | Executed and observed in Renode |
| Generic GPIO and LED drivers | Integrated and builds successfully |
| Ethernet MAC/DMA/MTL/PHY initialization | Implemented; physical Ethernet validation pending |
| Renode platform and run script | Configured and tested |
| OpenOCD programming on physical board | Pending board availability |
| Physical GPIO/UART validation | Pending board availability |
| GDB debugging | Tool installed; debug workflow not yet validated |

## 11. Development Roadmap

### Phase 1 – Bare-Metal Foundation
- Linux development environment.
- CMSIS integration.
- Startup assembly.
- Linker script.
- System initialization.
- Build and flash workflow.

### Phase 2 – Basic Peripherals
- GPIO output and input.
- LED control.
- USART3 debug logging.
- UART reception.
- External interrupts.
- NVIC configuration.

### Phase 3 – Clock and Timing
- RCC configuration.
- PLL configuration.
- Clock-tree management.
- SysTick.
- General-purpose timers.
- PWM generation.

### Phase 4 – Memory and Cortex-M7
- FPU configuration.
- I-cache and D-cache.
- MPU configuration.
- TCM and SRAM placement.
- DMA memory considerations.

### Phase 5 – Communication Peripherals
- SPI.
- I2C.
- ADC.
- DMA.
- Ethernet RMII.
- USB OTG.

### Phase 6 – Advanced Development
- Interrupt-driven peripheral drivers.
- DMA-based communication.
- Ethernet packet handling.
- Performance measurement.
- Low-power modes.

## 12. Development Philosophy

- Direct peripheral register programming.
- CMSIS device headers for register definitions.
- No HAL or LL dependency.
- Minimal external dependencies.
- Explicit clock and memory configuration.
- Separation of application logic and system initialization.
- Reusable application architecture.
- Linux command-line development workflow.

**Project target:** Develop a reusable STM32H753ZI bare-metal firmware platform using the Cortex-M7 architecture.

## 13. Renode Simulation Architecture

The firmware is currently built and executed in Renode before the physical NUCLEO-H753ZI board is available. Renode provides a simulated STM32H753-compatible platform, including a Cortex-M7 CPU model, memory and peripheral models.

### 13.1 Host, firmware and simulator relationship

```text
Ubuntu Linux host
|
+-- GNU Arm toolchain
|   |
|   +-- Compile C and startup assembly
|   +-- Link using STM32H753 linker script
|   +-- Generate build/nucleo_h753zi.elf
|
+-- Renode
    |
    +-- Load NUCLEO-H753ZI platform description (.repl)
    +-- Load firmware ELF into simulated memory
    +-- Initialize simulated CPU and peripherals
    +-- Execute ARM machine instructions
    |
    +-- Cortex-M7 CPU model
    |   |
    |   +-- Fetch and execute firmware instructions
    |   +-- Perform memory-mapped peripheral accesses
    |
    +-- Peripheral models
        |
        +-- GPIOB pin 0 -> GreenLED model -> LED state log
        |
        +-- USART3 -> UART analyzer -> console output
```

Renode does not execute the C source directly. GCC has already compiled the C and assembly sources into ARM machine code. The Renode CPU model executes those instructions, and the system bus routes peripheral accesses to the corresponding simulated peripheral models.

### 13.2 Firmware loading and execution sequence

The project-specific Renode script is:

```text
renode/nucleo_h753zi.resc
```

Its current commands are:

```resc
mach create "nucleo_h753zi"
machine LoadPlatformDescription @/home/sudharshanreddy/tools/renode/platforms/boards/nucleo_h753zi.repl
cpu VectorTableOffset 0x08000000
sysbus LoadELF @/home/sudharshanreddy/baremetal/nucleo_h753zi/build/nucleo_h753zi.elf
showAnalyzer sysbus.usart3
```

The sequence is:

1. `mach create` creates the simulated machine.
2. `LoadPlatformDescription` instantiates the NUCLEO-H753ZI platform model.
3. `VectorTableOffset` identifies the firmware vector-table address at `0x08000000`.
4. `sysbus LoadELF` parses the ELF and loads its loadable sections at their linked addresses.
5. `showAnalyzer` opens the USART3 UART analyzer.
6. The Renode `start` command starts emulation.

The ELF contains more than raw instructions: it also carries section addresses and symbol/debug information. The `.bin` file is a raw binary and does not carry the same ELF metadata.

### 13.3 CPU startup and application flow

After `start`, the CPU begins at the reset handler. The startup code then performs the project's reset sequence:

```text
Reset_Handler
    |
    +-- ExitRun0Mode()
    |
    +-- SystemInit()
    |
    +-- Copy .data from its load address to SRAM
    |
    +-- Zero-initialize .bss
    |
    +-- __libc_init_array()
    |
    +-- main()
          |
          +-- App_Init()
          |     |
          |     +-- Led_Init()
          |     +-- Uart3_Init()
          |     +-- Ethernet_Init()
          |     +-- Print initialization messages
          |
          +-- App_Run()
                |
                +-- Toggle GPIOB pin 0
                +-- Delay
                +-- Repeat
```

The startup file, system initialization source, linker script and application code all participate in this sequence.

### 13.4 ELF load addresses observed in the current build

The current Renode run reported:

```text
Loading block of 2028 bytes length at 0x08000000.
Loading block of 1536 bytes length at 0x080007EC.
```

These are ELF load blocks, not two independent firmware applications. The first contains the main image at the Flash address. The second is the initialized data image whose runtime destination is SRAM; startup code copies it to the `.data` execution address.

The current CPU startup log reported:

```text
PC = 0x080006BD
SP = 0x24080000
```

`PC` is the reset-handler execution address. `SP` is the initial stack pointer at the top of the configured RAM_D1 region.

### 13.5 GPIO observation path

The board model connects the GreenLED to GPIOB pin 0. Application code calls
`Led_Toggle()`; the GPIO driver performs the register access:

```c
Led_Toggle();
```

The simulated CPU performs the memory-mapped register access. Renode's GPIO
model updates the pin state, and the board's LED model observes that
transition.

Enable LED state-change logging at the Renode monitor:

```text
logLevel -1 sysbus.gpioPortB.GreenLED
```

Then start the simulation:

```text
start
```

Expected messages include:

```text
[NOISY] gpioPortB.GreenLED: LED state changed to True
[NOISY] gpioPortB.GreenLED: LED state changed to False
```

`True` and `False` represent the model's logical LED state. For this board mapping, the LED is connected to PB0 and is active-high.

To inspect the state on demand, use the Renode monitor:

```text
sysbus.gpioPortB.GreenLED State
```

To reduce console traffic, keep the firmware's repeated LED status UART print disabled and use Renode's GPIO LED logger instead. The software delay controls the transition interval; its exact wall-clock timing depends on the simulation and firmware implementation.

### 13.6 UART observation path

The firmware writes characters to USART3's transmit data register. Renode's USART3 model receives those writes, and the UART analyzer displays the transmitted characters in the Renode console.

The current script uses:

```resc
showAnalyzer sysbus.usart3
```

The UART analyzer output is generated from firmware UART transmissions. GPIO LED state messages, by contrast, are generated by Renode's LED model.

### 13.7 Identifying log sources

The console combines Renode's own messages with firmware output carried through
the simulated USART3. Use the prefix and device name to distinguish them:

| Example | Source | Meaning |
|---|---|---|
| `[INFO] Loaded monitor commands...` | Renode | Emulator startup or monitor information |
| `[INFO] sysbus: Loading block...` | Renode | ELF sections loaded into simulated memory |
| `[INFO] cpu: Setting initial values...` | Renode | Simulated CPU reset state |
| `[INFO] nucleo_h753zi: Machine started.` | Renode | Simulation lifecycle |
| `[WARNING] sysbus: ...` | Renode | Firmware accessed an address with no modeled peripheral |
| `[WARNING] ethernet: ...` or `[WARNING] syscfg: ...` | Renode | Peripheral model does not handle a register access or bit |
| `[NOISY] gpioPortB.GreenLED: LED state changed...` | Renode | Board LED model observed a GPIO transition |
| `[INFO] usart3: ...` | Renode transport prefix | The text following the prefix was sent by firmware over USART3 |
| `STM32H753ZI Bare-Metal App`, `[INIT] ...`, `[APP] ...`, `[LED] ...` | Firmware application | Strings emitted by `Uart3_SendString()` |
| `[ETH] Link up` | Firmware application, based on simulated PHY reads | The app interpreted the simulated PHY status as link up; this is not physical link validation |

For example, in:

```text
[INFO] usart3: ... [INIT] Ethernet MAC/PHY initialized
```

`[INFO] usart3:` is Renode's analyzer prefix; `[INIT] Ethernet MAC/PHY
initialized` is application text. Conversely, register-access warnings and
the LED state-change message are generated by Renode, not printed by the
application.

### 13.8 Build and run in Renode

From the project directory:

```bash
cd ~/baremetal/nucleo_h753zi
make clean
make
renode --console --disable-gui --plain renode/nucleo_h753zi.resc
```

At the Renode monitor prompt:

```text
logLevel -1 sysbus.gpioPortB.GreenLED
start
```

Stop emulation with `quit` at the Renode monitor. If the monitor is
unresponsive, use the PID-specific stop procedure in
[SETUP.md](SETUP.md#22-stop-renode) from another terminal.

To manage Renode processes from a terminal, list them and verify each PID:

```bash
ps -eo pid,user,args | grep '[r]enode'
```

Stop one selected instance with `kill -INT 12345`, replacing `12345` with its
verified PID. To stop multiple Renode instances, verify they are all yours
and explicitly list their PIDs, for example `kill -INT 12345 `. This
avoids signaling unrelated Renode sessions.

### 13.9 Simulation warnings and limitations

The current simulation emitted accesses to unmapped addresses:

```text
0x5C001000
0x51008108
```

These warnings did not prevent the observed application from reaching `main()`, transmitting USART3 messages or toggling the simulated LED. They should remain documented as simulator/model limitations to investigate, not be treated as proof that the corresponding physical hardware accesses are harmless.

The Renode STM32H753 platform uses inherited STM32H7 peripheral models. Simulation success validates the firmware against the implemented model behavior; it is not a substitute for physical-board validation, electrical verification, timing validation or confirmation of every STM32H753 silicon detail.

## 14. Renode Simulation Validation Status

| Validation item | Status |
|---|---|
| Renode installation on Ubuntu | Configured |
| NUCLEO-H753ZI platform description loaded | Successful |
| Project ELF loaded into simulated memory | Successful |
| Cortex-M7 reset/startup sequence | Reached application |
| GPIOB pin 0 / GreenLED transitions | Observed |
| USART3 output | Observed |
| Physical NUCLEO-H753ZI board testing | Pending board availability |

## 15. Ethernet MAC and PHY

The NUCLEO-H753ZI has an onboard Ethernet PHY and RJ45 connector. The STM32H753 provides the Ethernet MAC with DMA, while the Microchip LAN8742A PHY handles the cable-side physical interface.

### 15.1 Hardware and standards

| Item | Description |
|---|---|
| Ethernet MAC | Integrated STM32H753 MAC with DMA |
| PHY | Microchip LAN8742A |
| MAC–PHY interface | RMII |
| Ethernet modes | 10BASE-T / 100BASE-TX |
| Connector | Onboard RJ45 |
| RMII reference clock | 50 MHz |

The initial target is 100 Mbps Fast Ethernet (100BASE-TX).

| Standard / interface | Purpose |
|---|---|
| IEEE 802.3 | Ethernet MAC and frame format |
| IEEE 802.3u | Fast Ethernet / 100BASE-TX |
| RMII v1.2 | MAC-to-PHY data interface |
| IEEE 802.3 Clause 22 | PHY register access over MDC/MDIO |
| IEEE 802.3 Clause 28 | Link auto-negotiation |
| IEEE 1588 | Hardware timestamping; possible advanced feature |

RMII transfers two data bits per 50 MHz clock cycle, providing a 100 Mbps data interface. The RMII clock frequency is not the Ethernet line rate.

### 15.2 RMII and PHY management

RMII signals used by the MAC include:

- `REF_CLK`: 50 MHz reference clock from the PHY.
- `TXD0`, `TXD1`, `TX_EN`: transmit data and enable.
- `RXD0`, `RXD1`, `CRS_DV`: receive data and status.
- `MDC`, `MDIO`: PHY management interface.

The exact MCU pin mapping and solder-bridge settings must be verified against the NUCLEO-H753ZI schematic and the board revision before hardware testing.

The driver will use Clause 22 MDIO transactions to identify and monitor the LAN8742A. Initial registers include:

| Register | Address | Purpose |
|---|---:|---|
| BMCR | 0x00 | PHY control |
| BMSR | 0x01 | Link status |
| PHYID1 / PHYID2 | 0x02 / 0x03 | PHY identification |
| ANAR / ANLPAR | 0x04 / 0x05 | Auto-negotiation capabilities |

The PHY address and negotiated link mode will be verified from the board configuration and PHY registers.

### 15.3 MAC, MTL, DMA and memory

The driver will cover the main Ethernet blocks:

- **MAC:** frame handling, MAC address, filtering, speed and duplex.
- **MTL:** transmit/receive queues and FIFO configuration.
- **DMA:** descriptors, packet buffers, interrupts and transfer status.
- **PHY management:** MDIO access, identification and link status.

Ethernet DMA requires careful memory placement and Cortex-M7 cache handling. Descriptor and packet-buffer memory must be DMA-accessible; D-cache maintenance, alignment and memory barriers must be handled. DTCM must not be assumed to be accessible to Ethernet DMA.

## 16. Bare-Metal Ethernet Driver Plan

The driver will start with one header and one source file:

```text
Drivers/
└── Ethernet/
    ├── ethernet_mac.h
    ├── ethernet_mac.c
    └── README.md
```

`ethernet_mac.h` contains the public APIs and status data. `ethernet_mac.c` implements RMII configuration, LAN8742A MDIO access, DMA descriptors, MTL queues and the Ethernet MAC initialization sequence. The PHY and MAC remain in the Ethernet driver directory, while future LAN8742A-specific code may be split into dedicated `lan8742.c` and `lan8742.h` files.

### Initial milestone

The first milestone is to configure RMII, communicate with the LAN8742A over MDIO, read its PHY identifier and verify link status and negotiated mode. MAC initialization and raw frame handling will follow.

### Development constraints

- Embedded C and CMSIS register definitions; no HAL or LL.
- No RTOS or LwIP for the initial implementation.
- Application code uses the public APIs in `ethernet_mac.h`.
- Renode will be used only for features supported by its platform model. PHY link negotiation and real packet exchange require physical-board testing.

### References

- [STM32H753 reference manual RM0433](https://www.st.com/resource/en/reference_manual/rm0433-stm32h742-stm32h743-753-and-stm32h750-value-line-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)
- [NUCLEO-H753ZI board documentation](https://www.st.com/en/evaluation-tools/nucleo-h753zi.html)
- [Microchip LAN8742A documentation](https://www.microchip.com/en-us/product/lan8742a)