/** 
* @file TMC5160.c
* @brief TMC5160 Stepper Driver Library
*
* Author: Andrew Nguyen
* Date: June 2026
*
* References:
* Trinamic TMC5160 Datasheet (see docs)
* Relevant datasheet page numbers are cited throughout the source code.
*/

#include "TMC5160.h"
#include "TMC5160_SPI.h"

// --- REGISTER CONFIGURATION ---
// -- Write to register --
void TMC5160_WriteRegister(TMC5160_TypeDef *htmc, uint8_t reg_address, uint32_t data) 
{
    if (htmc == NULL) {return;}
    if (reg_address >= 128) {return;}

    // Store the written data for later use
    htmc->reg_cache[reg_address] = data;

    // Write data to the register address using SPI
    TMC5160_SPI_Write(htmc, reg_address, data);
}

// -- Read from register --
uint32_t TMC5160_ReadRegister(TMC5160_TypeDef *htmc, uint8_t reg_address)
{
    if (htmc == NULL) {return 0;}
    if (reg_address >= 128) {return 0;}

    // Read data from the register address using SPI
    return TMC5160_SPI_Read(htmc, reg_address);
}

void TMC5160_Init(TMC5160_TypeDef *htmc)
{   
    if (htmc == NULL) {return;}

    // Change htmc settings to default values
    htmc->irun = TMC5160_IRUN_DEFAULT;
    htmc->ihold = TMC5160_IHOLD_DEFAULT;
    htmc->microstep = TMC5160_MICROSTEP_DEFAULT;
    htmc->vmax = TMC5160_VMAX_DEFAULT;
    htmc->amax = TMC5160_AMAX_DEFAULT;
    if (htmc->r_sense <= 0.0f) {
        htmc->r_sense = TMC5160_R_SENSE_DEFAULT;
    }

    // Setup motor direction (pg 32 & 33)
    TMC5160_WriteRegister(htmc, TMC5160_GCONF, (0UL << 4)); // Default = 0 , Inverse = 1

    // Setup Chopper Configuration as Default (pg 51)
    TMC5160_WriteRegister(htmc, TMC5160_CHOPCONF, TMC5160_CHOPCONF_DEFAULT);

    // Setup Global Scaler of Motor Current (pg 36)
    TMC5160_WriteRegister(htmc, TMC5160_GLOBAL_SCALER, TMC5160_GLOBALSCALER_DEFAULT); // Default = 0 (Full Scale 256)

    // Setup default running and hold current (pg 38 & 74)
    // For Rsense = 0.075Ω; Imax = 3.1A (31 = 3.1);
    TMC5160_WriteRegister(htmc, TMC5160_IHOLD_IRUN, TMC5160_IRUNIHOLD_DEFAULT);
    
    // Setup delay time after stand still before power down (pg 38)
    TMC5160_WriteRegister(htmc, TMC5160_TPOWERDOWN, TMC5160_TPOWERDOWN_DEFAULT); // Default = 10

    // Setup switching velocity
    TMC5160_WriteRegister(htmc, TMC5160_TPWMTHRS, TMC5160_TPWMTHRS_DEFAULT); // Default = 500

    // Setup Ramp Mode (pg 40) 
    TMC5160_WriteRegister(htmc, TMC5160_RAMPMODE, TMC5160_RAMPMODE_POSITION); // Default = 0 (position mode)

    // Setup PWM configuration
    TMC5160_WriteRegister(htmc, TMC5160_PWMCONF, TMC5160_PWMCONF_DEFAULT);
    
    // Setup starting velocity before acceleration
    TMC5160_WriteRegister(htmc, TMC5160_VSTART, TMC5160_VSTART_DEFAULT); // Default = 1

    // Setup first acceleration rate
    TMC5160_WriteRegister(htmc, TMC5160_A1, TMC5160_A1_DEFAULT); // Default = 1000

    // Setup velocity threshold between A1 and AMAX
    TMC5160_WriteRegister(htmc, TMC5160_V1, TMC5160_V1_DEFAULT); // Default = 50000

    // Setup maximum acceleration
    TMC5160_WriteRegister(htmc, TMC5160_AMAX, TMC5160_AMAX_DEFAULT); // Default = 500

    // Setup maximum velocity
    TMC5160_WriteRegister(htmc, TMC5160_VMAX, TMC5160_VMAX_DEFAULT); // Default = 200000

    // Setup maximum deceleration
    TMC5160_WriteRegister(htmc, TMC5160_DMAX, TMC5160_DMAX_DEFAULT); // Default = 700

    // Setup final deceleration before VSTOP
    TMC5160_WriteRegister(htmc, TMC5160_D1, TMC5160_D1_DEFAULT); // Default = 1400

    // Setup stop velocity
    TMC5160_WriteRegister(htmc, TMC5160_VSTOP, TMC5160_VSTOP_DEFAULT); // Default = 10

    // -- Position --
    // Setup actual position
    TMC5160_WriteRegister(htmc, TMC5160_XACTUAL, TMC5160_POSITION_DEFAULT); // Default = 0
    
    // Setup target position
    TMC5160_WriteRegister(htmc, TMC5160_XTARGET, TMC5160_POSITION_DEFAULT); // Default = 0
}

TMC5160_Status_TypeDef TMC5160_SetCurrent(TMC5160_TypeDef *htmc, uint16_t current_ma)
{
    if (htmc == NULL) {return TMC5160_BADARG;}

    if (htmc->r_sense <= 0.0f) {return TMC5160_BADARG;}

    // -- Manually set current --
    // Calculate run and hold current 
    // Formula: CS = 32 * sqrt(2) * I_RMS * (Rsense) / VFS - 1
    // Datasheet pg 74, VFS = 0.325V (VSENSE = 0)
    float cs = 32 * SQRT_TWO * (current_ma/1000.0f) * (htmc->r_sense) / 0.325f - 1;

    TMC5160_Status_TypeDef status = TMC5160_OK;
    if (cs > 31.0f) {
        cs = 31.0f; // IRUN cannot be greater than 31
        status = TMC5160_CLAMPED;
    }
    else if (cs < 0.0f) {
        cs = 0.0f;
        status = TMC5160_CLAMPED;
    }
    htmc->irun = (uint8_t)(cs + 0.5f);
    htmc->ihold = htmc->irun / 2;   

    // Set current
    TMC5160_WriteRegister(htmc, TMC5160_IHOLD_IRUN, 
    ((uint32_t)htmc->ihold << 0) | // IHOLD = ihold 
    ((uint32_t)htmc->irun << 8)  | // IRUN = irun
    (6UL << 16));   // IHOLDDELAY

    return status;
}

void TMC5160_SetEN(TMC5160_TypeDef *htmc, GPIO_PinState state)
{
    if (htmc == NULL || htmc->en.port == NULL) {return;}

    // Pull EN Pin LOW or HIGH
    HAL_GPIO_WritePin(htmc->en.port, htmc->en.pin, state);
}

TMC5160_Status_TypeDef TMC5160_SetRampMode(TMC5160_TypeDef *htmc, TMC5160_RampMode_TypeDef ramp_mode)
{
    if (htmc == NULL) {return TMC5160_BADARG;}

    switch (ramp_mode)
    {
        case TMC5160_RAMPMODE_POSITION:
        case TMC5160_RAMPMODE_VEL_POS:
        case TMC5160_RAMPMODE_VEL_NEG:
        case TMC5160_RAMPMODE_HOLD:
            TMC5160_WriteRegister(htmc, TMC5160_RAMPMODE, ramp_mode);
            break;
        default:
            return TMC5160_BADARG;
    }
    return TMC5160_OK;
}

TMC5160_Status_TypeDef TMC5160_SetMicrostep(TMC5160_TypeDef *htmc, uint16_t microstep)
{   
    if (htmc == NULL) {return TMC5160_BADARG;}

    // Get the data at chopconf's register (pg 51)
    uint32_t chopconf = htmc->reg_cache[TMC5160_CHOPCONF];

    // Clear MRES bits (bit 24-27)
    chopconf &= ~(0xFUL << 24);
    // Set MRES bits
    switch (microstep)
    {
        case 256: chopconf |= (0x00UL << 24); htmc->microstep = 256; break;
        case 128: chopconf |= (0x01UL << 24); htmc->microstep = 128; break;
        case 64: chopconf |= (0x02UL << 24); htmc->microstep = 64; break;
        case 32: chopconf |= (0x03UL << 24); htmc->microstep = 32; break;
        case 16: chopconf |= (0x04UL << 24); htmc->microstep = 16; break;
        case 8: chopconf |= (0x05UL << 24); htmc->microstep = 8; break;
        case 4: chopconf |= (0x06UL << 24); htmc->microstep = 4; break;
        case 2: chopconf |= (0x07UL << 24); htmc->microstep = 2; break;
        case 1: chopconf |= (0x08UL << 24); htmc->microstep = 1; break;

        // Default = 256 microsteps 
        default: return TMC5160_BADARG;
    }

    // Write back to register
    TMC5160_WriteRegister(htmc, TMC5160_CHOPCONF, chopconf);
    return TMC5160_OK;
}

TMC5160_Status_TypeDef TMC5160_SetVelocity(TMC5160_TypeDef *htmc, uint32_t max_velocity)
{  
    if (htmc == NULL) {return TMC5160_BADARG;}

    TMC5160_Status_TypeDef status = TMC5160_OK;
    if (max_velocity > TMC5160_VMAX_MAX) {
        max_velocity = TMC5160_VMAX_MAX;
        status = TMC5160_CLAMPED;
    }

    htmc->vmax = max_velocity;
    TMC5160_WriteRegister(htmc, TMC5160_VMAX, max_velocity);
    return status;
}

TMC5160_Status_TypeDef TMC5160_SetAcceleration(TMC5160_TypeDef *htmc, uint32_t max_acceleration)
{
    if (htmc == NULL) {return TMC5160_BADARG;}

    TMC5160_Status_TypeDef status = TMC5160_OK;
    if (max_acceleration > TMC5160_AMAX_MAX) {
        max_acceleration = TMC5160_AMAX_MAX;
        status = TMC5160_CLAMPED;
    }

    htmc->amax = max_acceleration;
    TMC5160_WriteRegister(htmc, TMC5160_AMAX, max_acceleration);
    return status;
}

void TMC5160_MoveTo(TMC5160_TypeDef *htmc, int32_t position)
{
    if (htmc == NULL) {return;}

    // -- Move the motor to a given position --
    // Set the ramp mode to position mode
    TMC5160_WriteRegister(htmc, TMC5160_RAMPMODE, TMC5160_RAMPMODE_POSITION);
    // Move the motor to position
    TMC5160_WriteRegister(htmc, TMC5160_XTARGET, position);
}

int32_t TMC5160_GetPosition(TMC5160_TypeDef *htmc)
{    
    if (htmc == NULL) {return 0;}

    // Read and return actual position of the motor
    return (int32_t)TMC5160_ReadRegister(htmc, TMC5160_XACTUAL);
}

int32_t TMC5160_GetVelocity(TMC5160_TypeDef *htmc)
{
    if (htmc == NULL) {return 0;}

    uint32_t data = TMC5160_ReadRegister(htmc, TMC5160_VACTUAL) & 0x00FFFFFF;
    if (data & 0x00800000UL) {       // bit 23 set -> negative
        data |= 0xFF000000UL;        // sign-extend to 32-bit
    }

    // Read and return actual velocity of the motor
    return (int32_t)data;
}

TMC5160_IOIN_TypeDef TMC5160_GetIOIN(TMC5160_TypeDef *htmc)
{
    if (htmc == NULL) {return (TMC5160_IOIN_TypeDef){0};}
    // Read IOIN register (pg 34)
    uint32_t ioin = TMC5160_ReadRegister(htmc, TMC5160_IOIN);

    TMC5160_IOIN_TypeDef data = {
        .version = (ioin >> 24) & 0xFF, // Check TMC5160 Version (0x30)
        .sd_mode = (ioin >> 6) & 1, // Check SD MODE
        .drv_enn = (ioin >> 4) & 1 // Check EN Pin
    };

    return data;
}

TMC5160_DrvStat_TypeDef TMC5160_GetDrvStat(TMC5160_TypeDef *htmc)
{
    if (htmc == NULL) {return (TMC5160_DrvStat_TypeDef){0};}

    // Read Driver Status (pg 56)
    uint32_t stat = TMC5160_ReadRegister(htmc, TMC5160_DRV_STATUS);

    TMC5160_DrvStat_TypeDef data = {
        .stst = (stat >> 31) & 1, // Standstill 
        .olb = (stat >> 30) & 1, // Open Load B
        .ola = (stat >> 29) & 1, // Open Load A
        .s2gb = (stat >> 28) & 1, // Short to GND B
        .s2ga = (stat >> 27) & 1, // Short to GND A
        .otpw = (stat >> 26) & 1, // Overtemperature Prewarning
        .ot = (stat >> 25) & 1, // Overtemperature
        .stallguard = (stat >> 24) & 1 // StallGuard
    };

    return data;
}

TMC5160_GStat_TypeDef TMC5160_GetGStat(TMC5160_TypeDef *htmc)
{
    if (htmc == NULL) {return (TMC5160_GStat_TypeDef){0};}

    // Read GStat (pg 33)
    uint32_t gstat = TMC5160_ReadRegister(htmc, TMC5160_GSTAT);

    TMC5160_GStat_TypeDef data = {
        .reset = (gstat >> 0) & 1, // IC has been reset
        .drv_err = (gstat >> 1) & 1, // Driver shutdown
        .uv_cp = (gstat >> 2) & 1 // Undervoltage on charge pump
    };

    return data;
}
