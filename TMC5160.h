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

    // - Register data -
    // Keeps a copy of data written to the registers (because some registers are write only)
    uint32_t reg_data[128]; // 512 bytes stored in RAM

    // - Current -
    float r_sense; // TMC5160 board sense resistor (default = 0.075Ω)
    uint16_t max_cur; // Rated current for motor in mA

} TMC5160_TypeDef;

// --- API ---
// -- Register --
void TMC5160_WriteRegister(TMC5160_TypeDef *htmc, uint8_t reg_address, uint32_t data);
uint32_t TMC5160_ReadRegister(TMC5160_TypeDef *htmc, uint8_t reg_address);

// -- TMC5160 --
void TMC5160_Init(TMC5160_TypeDef *htmc);

#endif