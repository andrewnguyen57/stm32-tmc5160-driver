# STM32 TMC5160 DRIVER LIBRARY

A lightweight STM32 driver library for the TMC5160 stepper motor driver using SPI.

This project is developed for the purpose of learning low-level development and out of necessity, it eventually evolved to a reusable library that anyone can use for the STM32 platform.

The library is fully functional, but I am still working on it to add features and optimization.

## Installation

1. Copy the driver files into your STM32 project:

```
inc/
    TMC5160.h
    TMC5160_SPI.h
    TMC5160_Registers.h

src/
    TMC5160.c
    TMC5160_SPI.c
```

2. Add the header files to your project's include paths.
3. Enable an SPI peripheral in STM32CubeMX (see the Example below for the required settings).
4. Include the driver in your code:

```c
#include "TMC5160.h"
```

## Example

The example was written for the **STM32 Nucleo-F446RE** development board. Adapt it to your own board if needed.

**SPI connections:**

| STM32 Pin | TMC5160 |
|-----------|---------|
| PA5 | SCK |
| PA6 | MISO |
| PA7 | MOSI |
| PB6 | CS |
| PB5 | EN |

Configure the SPI peripheral as **Mode 3** (CPOL = 1, CPHA = 1), MSB first, 8-bit, and set CS as a normal GPIO output.

Connect the SPI pins as shown and flash the `main.c` file in the example directory onto your STM32 board.

```c
#include "TMC5160.h"

TMC5160_TypeDef htmc;
const TMC5160_Config_TypeDef htmc_cfg = {
    .hspi    = &hspi1,
    .cs      = { .port = GPIOB, .pin = GPIO_PIN_6 }, // CS Pin
    .en      = { .port = GPIOB, .pin = GPIO_PIN_5 }, // EN Pin
    .r_sense = 0.075f,   // board sense resistor in ohms
};

void motor_setup(void)
{
    TMC5160_Init(&htmc, &htmc_cfg);       // load default configuration
    TMC5160_SetEN(&htmc, GPIO_PIN_RESET); // enable driver
    TMC5160_MoveTo(&htmc, 51200);         // move to an absolute position
}
```

## Files

| File | Purpose |
|------|---------|
| `TMC5160.h` | Public API, handle struct, enums, defaults |
| `TMC5160.c` | Driver logic: init, current, microstep, ramp, motion, diagnostics |
| `TMC5160_SPI.h` | SPI transport API |
| `TMC5160_SPI.c` | SPI read/write datagram implementation |
| `TMC5160_Registers.h` | TMC5160 register address map |

## API

### Control

| Function | Returns | Description |
|----------|---------|-------------|
| `TMC5160_Init(htmc, cfg)` | `Status` | Load the default configuration |
| `TMC5160_SetEN(htmc, state)` | `Status` | Drive the EN pin (`GPIO_PIN_RESET` = enabled) |
| `TMC5160_SetCurrent(htmc, current_ma)` | `Status` | Set run/hold current in milliamps |
| `TMC5160_SetRampMode(htmc, mode)` | `Status` | Select ramp mode |
| `TMC5160_SetMicrostep(htmc, microstep)` | `Status` | Set microstep resolution (1–256) |
| `TMC5160_SetVelocity(htmc, vmax)` | `Status` | Set max velocity (`VMAX`) |
| `TMC5160_SetAcceleration(htmc, amax)` | `Status` | Set max acceleration (`AMAX`) |
| `TMC5160_MoveTo(htmc, position)` | `Status` | Switch to position mode and move to target |

### Readback

| Function | Returns | Description |
|----------|---------|-------------|
| `TMC5160_GetPosition(htmc)` | `int32_t` | Actual position (`XACTUAL`) |
| `TMC5160_GetVelocity(htmc)` | `int32_t` | Actual velocity (`VACTUAL`) |
| `TMC5160_GetIOIN(htmc)` | `IOIN` | Input pin states and chip version |
| `TMC5160_GetDrvStat(htmc)` | `DrvStat` | Driver status flags |
| `TMC5160_GetGStat(htmc)` | `GStat` | Global status flags |

### Register access

| Function | Returns | Description |
|----------|---------|-------------|
| `TMC5160_WriteRegister(htmc, reg, data)` | `void` | Write a 32-bit value and cache it |
| `TMC5160_ReadRegister(htmc, reg)` | `uint32_t` | Read a 32-bit value |

**Status codes:**
- `TMC5160_OK`
- `TMC5160_BADARG` (NULL/invalid argument)
- `TMC5160_CLAMPED` (value out of range, clamped to the limit)

**Ramp modes:**
- `TMC5160_RAMPMODE_POSITION`
- `TMC5160_RAMPMODE_VEL_POS`
- `TMC5160_RAMPMODE_VEL_NEG`
- `TMC5160_RAMPMODE_HOLD`

## Related Projects

- [Stepper Motor Library](https://github.com/andrewnguyen57/stepper-motor-driver): depends on this TMC5160 driver library.
- [Robotic Chess Project](https://github.com/RoboticChessProject): the eventual goal of this project.

## Demo Video

- https://youtube.com/shorts/sfI2iLDyZvg?feature=share

## Documentation

- [TMC5160 Datasheet](docs/tmc5160a_datasheet_rev1.17.pdf)

## License

- This project is licensed under the MIT License.

## Notes

- Add a decoupling capacitor (min 100 µF) between the VM inputs.
- The SD_MODE pin must be pulled low for RAMP mode to work.
- STEP and DIR pins are not needed for the SPI functions.
- Some TMC5160 registers are write-only, so the driver keeps a cache copy of every value it writes.