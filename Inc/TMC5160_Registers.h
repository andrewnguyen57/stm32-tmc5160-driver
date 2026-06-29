/*
Reference TMC5160 Datasheet pg 32 - pg 49*/

#ifndef TMC5160_REGISTERS_H
#define TMC5160_REGISTERS_H

// --- REGISTERS ---
// -- Global Configs --
#define TMC5160_GCONF   0x00 // Global configuration flags
#define TMC5160_GSTAT   0x01 // Global status flags
#define TMC5160_IOIN    0x04 // IOIN

// -- Current --
#define TMC5160_IHOLD_IRUN  0x10 // Driver current control
#define TMC5160_TPOWERDOWN  0x11 // Set delay time after stand still to power down (0-4s)
#define TMC5160_TPWMTHRS    0x13 // SpreadCycle/SteathChop
#define TMC5160_TCOOLTHRS   0x14 // CoolStep / StallGuard threshold velocity

// -- Ramp Generator --
#define TMC5160_RAMPMODE    0x20 // Ramp mode
#define TMC5160_XACTUAL     0x21 // Actual motor position (signed)
#define TMC5160_VACTUAL     0x22 // Actual velocity from ramp generator (signed)
#define TMC5160_VSTART      0x23 // Motor start velocity
#define TMC5160_A1          0x24 // First acceleration between VSTART and V1 (unsigned)
#define TMC5160_V1          0x25 // First acceleration / deceleration phase threshole velocity (unsigned)
#define TMC5160_AMAX        0x26 // Second acceleration between V1 and VMAX (unsigned)
#define TMC5160_VMAX        0x27 // Motion ramp target velocity (for positioning ensure VMAX >= VSTART) (unsigned)
#define TMC5160_DMAX        0x28 // Deceleration between VMAX and V1 (unsigned)
#define TMC5160_D1          0x2A // Deceleration between V1 and VSTOP
#define TMC5160_VSTOP       0x2B // Motor stop velocity (unsigned)
#define TMC5160_XTARGET     0x2D // Target position for ramp mode (signed)

// -- Driver --
#define TMC5160_CHOPCONF    0x6C // Chopper and Driver configuration
#define TMC5160_COOLCONF    0x6D // CoolStep smart current control register and StallGuard2 configuration
#define TMC5160_DRV_STATUS  0x6F // StallGuard2 value and driver error flags
#define TMC5160_PWMCONF     0x70 // Voltage PWM mode chopper configuration

// -- TMC5160 -- 
#define TMC5160_SHORT_CONF  0x09 // Short circuit protection configuration
#define TMC5160_DRV_CONF    0x0A // Driver configuration
#define TMC5160_GLOBAL_SCALER 0x0B // Global scaling of Motor current

#endif

