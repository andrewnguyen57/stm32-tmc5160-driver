#ifndef TMC5160_SPI_H
#define TMC5160_SPI_H

#include "TMC5160.h"

// --- API ---
// -- SPI --
void TMC5160_SPI_Write(TMC5160_TypeDef *htmc, uint8_t reg_address, uint32_t data);
uint32_t TMC5160_SPI_Read(TMC5160_TypeDef *htmc, uint8_t reg_address);

#endif