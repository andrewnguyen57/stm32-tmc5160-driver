/*
Reference TMC5160 Datasheet pg 32 - pg 49*/

#ifndef TMC5160_H
#define TMC5160_H

// --- Registers ---

// -- Global Configs --
#define TMC_GCONF   0x00 // Global configuration flags
#define TMC_GSTAT   0x01 // Global status flags

// -- Current --
#define TMC_IHOLD_IRUN  0x10 // Driver current control
#define TMC_TPOWERDOWN  0x11 // Set delay time after stand still to power down (0-4s)

// -- Ramp Generator --
#define TMC_RAMPMODE    0x20 // Ramp mode
#define TMC_XACTUAL     0x21 // Actual motor position (signed)
#define TMC_VSTART      0x23 // Motor start velocity
#define TMC_A1          0x24 // First acceleration between VSTART and V1 (unsigned)
#define TMC_V1          0x25 // First acceleration / deceleration phase threshole velocity (unsigned)
#define TMC_AMAX        0x26 // Second acceleration between V1 and VMAX (unsigned)
#define TMC_VMAX        0x27 // Motion ramp target velocity (for positioning ensure VMAX >= VSTART) (unsigned)
#define TMC_DMAX        0x28 // Deceleration between VMAX and V1 (unsigned)
#define TMC_D1          0x2A // Deceleration between V1 and VSTOP
#define TMC_VSTOP       0x2B // Motor stop velocity (unsigned)
#define TMC_XTARGET     0x2D // Target position for ramp mode (signed)

// -- Driver --
#define TMC_CHOPCONF    0x6C // Chopper and Driver configuration
#define TMC_COOLCONF    0x6D // CoolStep smart current control register and StallGuard2 configuration
#define TMC_DRV_STATUS  0x6F // StallGuard2 value and driver error flags
#define TMC_PWMCONF     0x70 // Voltage PWM mode chopper configuration

// -- TMC5160 -- 
#define SHORT_CONF  0x09 // Short circuit protection configuration
#define DRV_CONF    0x0A // Driver configuration
#define GLOBAL_SCALER 0x0B // Global scaling of Motor current

// --- Prototypes ---
void tmc_spi_write(uint8_t reg_address, uint32_t data);
uint32_t tmc_spi_read(uint8_t reg_address);

#endif

