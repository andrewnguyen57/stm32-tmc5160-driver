# stm32-tmc5160-driver
Work-in-progress STM32 driver for the TMC5160 stepper motor driver, developed to learn low-level programming and will later be integrated into my robotic arm project.

The library is fully functional and can perform basic motor movements. Development is still on going and I will continue to improve the architecture and add functionality.

The goal is to eventually make the library easy to use for future users.

### Example
The example was written for the STM32 Nucleo-F446RE Development Board.

SPI Connections:
- PA5 <-> SCK
- PA6 <-> MISO
- PA7 <-> MOSI
- PB6 <-> CS
- PB5 <-> EN

### Robotic Arm Project
- [Robotic Chess Project](https://github.com/RoboticChessProject)

### Demo Video
- https://youtube.com/shorts/sfI2iLDyZvg?feature=share

### Documentation
- [TMC5160 Datasheet](docs/tmc5160a_datasheet_rev1.17.pdf)

### Note
- Add a decoupling capacitor (min 100uF) between the VM inputs.
- The SD_MODE pin must be pulled low for RAMP Mode to work.
- STEP and DIR pins are not needed for SPI functions.