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
    // Store the written data for later use
    htmc->reg_data[reg_address] = data;

    // Write data to the register address using SPI
    TMC5160_SPI_Write(htmc, reg_address, data);
}

// -- Read from register --
uint32_t TMC5160_ReadRegister(TMC5160_TypeDef *htmc, uint8_t reg_address)
{
    // Read data from the register address using SPI
    uint32_t data = TMC5160_SPI_Read(htmc, reg_address);

    return data;
}

void TMC5160_SetCurrent(TMC5160_TypeDef *htmc)
{
    // -- Manually set current --
    // Calculate run and hold current 
    // Formula: CS = 32 * sqrt(2) * I_RMS * (Rsense + 0.02) / VFS - 1
    // Datasheet pg 74, VFS = 0.325V (VSENSE = 0)
    uint8_t irun = 32 * SQRT_TWO * (htmc->motor_current/1000.0f) * (htmc->r_sense + 0.02f) / 0.325f - 1;
    if (irun > 31) {
        irun = 31; // IRUN cannot be greater than 31
    }
    uint8_t ihold = irun / 2;   


    // Set current
    TMC5160_WriteRegister(htmc, TMC5160_IHOLD_IRUN, 
    (ihold << 0) | // IHOLD = ihold 
    (irun << 8)  | // IRUN = irun
    (6     << 16));   // IHOLDDELAY
}

void TMC5160_MoveTo(TMC5160_TypeDef *htmc, int32_t pos)
{
    // -- Move the motor to a given position --
    // Set the ramp mode to position mode
    TMC5160_WriteRegister(htmc, TMC5160_RAMPMODE, TMC5160_RAMPMODE_POS);
    // Move the motor to position
    TMC5160_WriteRegister(htmc, TMC5160_XTARGET, pos);
}

void TMC5160_Init(TMC5160_TypeDef *htmc)
{   
    HAL_Delay(1000);

    // Setup motor direction (pg 32 & 33)
    TMC5160_WriteRegister(htmc, TMC5160_GCONF, (0 << 4)); // Default = 0 , Inverse = 1

    // Setup Chopper Configuration as Default (pg 51)
    TMC5160_WriteRegister(htmc, TMC5160_CHOPCONF, 
        (1 << 15) | // TBL = 1   
        (4 << 7)  | // HEND = 4
        (0 << 4)  | // HSTRT = 0
        (4 << 0));  // TOFF = 4

    // Setup Global Scaler of Motor Current (pg 36)
    TMC5160_WriteRegister(htmc, TMC5160_GLOBAL_SCALER, 0x00000000); // Default = 0 (Full Scale 256)

    // Setup default running and hold current (pg 38 & 74)
    // For Rsense = 0.075Ω; Imax = 3.1A (31 = 3.1);
    TMC5160_WriteRegister(htmc, TMC5160_IHOLD_IRUN, 
    (6 << 0) | // IHOLD = 6 (50% of IRUN)
    (12 << 8)  | // IRUN = 12 (use 1A as default, change later)
    (6 << 16)); // IHOLDDELAY = 6 (50% of IRUN)
    
    // Setup delay time after stand still before power down (pg 38)
    TMC5160_WriteRegister(htmc, TMC5160_TPOWERDOWN, 0x0000000A); // Default = 10

    // Setup Ramp Mode (pg 40) 
    TMC5160_WriteRegister(htmc, TMC5160_RAMPMODE, TMC5160_RAMPMODE_POS); // Default = 0 (position mode)

    // Setup starting velocity before acceleration
    TMC5160_WriteRegister(htmc, TMC5160_VSTART, 0x00000001); // Default = 1

    // Setup first acceleration rate
    TMC5160_WriteRegister(htmc, TMC5160_A1, 0x0000003E8); // Default = 1000

    // Setup velocity threshold between A1 and AMAX
    TMC5160_WriteRegister(htmc, TMC5160_V1, 0x0000C350); // Default = 50000

    // Setup maximum acceleration
    TMC5160_WriteRegister(htmc, TMC5160_AMAX, 0x0000001F4); // Default = 500

    // Setup maximum velocity
    TMC5160_WriteRegister(htmc, TMC5160_VMAX, 0x00030D40); // Default = 200000

    // Setup maximum deceleration
    TMC5160_WriteRegister(htmc, TMC5160_DMAX, 0x0000002BC); // Default = 700

    // Setup final deceleration before VSTOP
    TMC5160_WriteRegister(htmc, TMC5160_D1, 0x00000578); // Default = 1400

    // Setup stop velocity
    TMC5160_WriteRegister(htmc, TMC5160_VSTOP, 0x0000000A); // Default = 10

    // -- Position --
    // Setup actual position
    TMC5160_WriteRegister(htmc, TMC5160_XACTUAL, 0x00000000); // Default = 0
    
    // Setup target position
    TMC5160_WriteRegister(htmc, TMC5160_XTARGET, 0x00000000); // Default = 0
}

uint8_t TMC5160_TestConnection(TMC5160_TypeDef *htmc)
{
    // Read IOIN register (0x04) - version byte should be 0x30 for TMC5160
    uint32_t data = TMC5160_ReadRegister(htmc, 0x04);
    uint8_t version = (data >> 24) & 0xFF;
    return version;  // should return 0x30
}