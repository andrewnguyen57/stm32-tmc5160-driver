#ifndef TMC5160_H
#define TMC5160_H

#include "TMC5160_Registers.h"
#include "stm32f4xx_hal.h"

// -- DEFAULTS --
// Default init values
#define TMC5160_CHOPCONF_DEFAULT    0x000100C3UL
#define TMC5160_IRUN_DEFAULT        20UL
#define TMC5160_IHOLD_DEFAULT       10UL
#define TMC5160_MICROSTEP_DEFAULT   256UL
#define TMC5160_R_SENSE_DEFAULT     0.075f

// Register defaults
#define TMC5160_GLOBALSCALER_DEFAULT 0x00000000UL
#define TMC5160_IRUNIHOLD_DEFAULT   0x0006140AUL
#define TMC5160_TPOWERDOWN_DEFAULT  0x0000000AUL
#define TMC5160_TPWMTHRS_DEFAULT    0x000001F4UL
#define TMC5160_PWMCONF_DEFAULT     0xC10D0024UL
#define TMC5160_VSTART_DEFAULT      0x00000001UL
#define TMC5160_A1_DEFAULT          0x0000003E8UL
#define TMC5160_V1_DEFAULT          0x0000C350UL
#define TMC5160_AMAX_DEFAULT        0x000001F4UL  
#define TMC5160_VMAX_DEFAULT        0x00030D40UL
#define TMC5160_DMAX_DEFAULT        0x0000002BCUL
#define TMC5160_D1_DEFAULT          0x00000578UL
#define TMC5160_VSTOP_DEFAULT       0x0000000AUL
#define TMC5160_POSITION_DEFAULT    0x00000000UL

// Max 
#define TMC5160_VMAX_MAX            0x007FFFFFUL
#define TMC5160_AMAX_MAX            0x0003FFFFUL

// Math
#define SQRT_TWO                    1.41421356f

// --- TypeDefs ---
// -- Port --
typedef struct {
    GPIO_TypeDef *port;
    uint16_t pin;
} GPIO_Pin_TypeDef;

// -- RAMP MODE --
typedef enum {
    TMC5160_RAMPMODE_POSITION = 0x00,
    TMC5160_RAMPMODE_VEL_POS = 0x01,
    TMC5160_RAMPMODE_VEL_NEG = 0x02,
    TMC5160_RAMPMODE_HOLD = 0x03
} TMC5160_RampMode_TypeDef;

// -- TMC5160 Status TypeDef --
typedef enum {
    TMC5160_OK = 0,
    TMC5160_BADARG,
    TMC5160_CLAMPED
} TMC5160_Status_TypeDef;

// -- TMC5160 TypeDef --
typedef struct {
    // - SPI -
    SPI_HandleTypeDef *hspi;
    
    // - CS -
    GPIO_Pin_TypeDef cs;

    // - EN -
    GPIO_Pin_TypeDef en;

    // - Register data -
    // Keeps a copy of data written to the registers (because some registers are write only)
    uint32_t reg_cache[128]; // 512 bytes stored in RAM

    // - Current -
    float r_sense; // TMC5160 board sense resistor (default = 0.075Ω)
    uint8_t irun; // Running current
    uint8_t ihold; // Hold current

    // - Velocity & Acceleration -
    uint32_t vmax;
    uint32_t amax;

    // - Microstep -
    uint16_t microstep;
} TMC5160_TypeDef;

typedef struct {
    uint8_t version;
    uint8_t sd_mode;
    uint8_t drv_enn;
} TMC5160_IOIN_TypeDef;

typedef struct {
    uint8_t stst;
    uint8_t olb;
    uint8_t ola;
    uint8_t s2gb;
    uint8_t s2ga;
    uint8_t otpw;
    uint8_t ot;
    uint8_t stallguard;
} TMC5160_DrvStat_TypeDef;

typedef struct {
    uint8_t reset;
    uint8_t drv_err;
    uint8_t uv_cp;
} TMC5160_GStat_TypeDef;

// --- API ---
// -- Register --
void TMC5160_WriteRegister(TMC5160_TypeDef *htmc, uint8_t reg_address, uint32_t data);
uint32_t TMC5160_ReadRegister(TMC5160_TypeDef *htmc, uint8_t reg_address);

// -- TMC5160 --
// - Set -
void TMC5160_Init(TMC5160_TypeDef *htmc);
void TMC5160_SetEN(TMC5160_TypeDef *htmc, GPIO_PinState state);
TMC5160_Status_TypeDef TMC5160_SetCurrent(TMC5160_TypeDef *htmc, uint16_t current_ma);
TMC5160_Status_TypeDef TMC5160_SetRampMode(TMC5160_TypeDef *htmc, TMC5160_RampMode_TypeDef ramp_mode);
TMC5160_Status_TypeDef TMC5160_SetMicrostep(TMC5160_TypeDef *htmc, uint16_t microstep);
TMC5160_Status_TypeDef TMC5160_SetVelocity(TMC5160_TypeDef *htmc, uint32_t max_velocity);
TMC5160_Status_TypeDef TMC5160_SetAcceleration(TMC5160_TypeDef *htmc, uint32_t max_acceleration);
void TMC5160_MoveTo(TMC5160_TypeDef *htmc, int32_t position);

// - Read -
int32_t TMC5160_GetPosition(TMC5160_TypeDef *htmc);
int32_t TMC5160_GetVelocity(TMC5160_TypeDef *htmc);

// - Debug -
TMC5160_IOIN_TypeDef TMC5160_GetIOIN(TMC5160_TypeDef *htmc);
TMC5160_DrvStat_TypeDef TMC5160_GetDrvStat(TMC5160_TypeDef *htmc);
TMC5160_GStat_TypeDef TMC5160_GetGStat(TMC5160_TypeDef *htmc);

#endif