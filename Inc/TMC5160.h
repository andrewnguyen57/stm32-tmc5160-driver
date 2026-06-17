#ifndef TMC5160_H
#define TMC5160_H

#include "TMC5160_Config.h"
#include "stm32f4xx_hal.h"

// --- TypeDefs ---
typedef struct {
    // - SPI -
    SPI_HandleTypeDef *hspi;
    
    // - CS -
    GPIO_TypeDef *cs_port;
    uint16_t cs_pin;

    // - EN -
    GPIO_TypeDef *en_port;
    uint16_t      en_pin;

    // - Register data -
    // Keeps a copy of data written to the registers (because some registers are write only)
    uint32_t reg_data[128]; // 512 bytes stored in RAM

    // - Current -
    float r_sense; // TMC5160 board sense resistor (default = 0.075Ω)
    uint16_t motor_current; // Operating current for motor in mA
    uint16_t max_current; // Rated current for motor in mA

} TMC5160_TypeDef;

// --- API ---

// -- Register --
void TMC5160_WriteRegister(TMC5160_TypeDef *htmc, uint8_t reg_address, uint32_t data);
uint32_t TMC5160_ReadRegister(TMC5160_TypeDef *htmc, uint8_t reg_address);

// -- TMC5160 --
void TMC5160_Init(TMC5160_TypeDef *htmc);
void TMC5160_SetCurrent(TMC5160_TypeDef *htmc);
void TMC5160_MoveTo(TMC5160_TypeDef *htmc, int32_t pos, UART_HandleTypeDef *huart);
void TMC5160_GetDrv(TMC5160_TypeDef *htmc, UART_HandleTypeDef *huart);
void TMC5160_EN_Enable(TMC5160_TypeDef *htmc);
void TMC5160_EN_Disable(TMC5160_TypeDef *htmc);
void TMC5160_GetDrvStat(TMC5160_TypeDef *htmc, UART_HandleTypeDef *huart);
void TMC5160_GetGStat(TMC5160_TypeDef *htmc, UART_HandleTypeDef *huart);
void TMC5160_SetRampMode(TMC5160_TypeDef *htmc, uint32_t rampmode);

#endif