#include "TMC5160.h"


// --- SPI COMMUNICATION ---
// -- Write to driver using spi --
void tmc_spi_write(uint8_t reg_address, uint32_t data)
{   
    uint8_t tx[5]; // an array of 5 bytes

    tx[0] = reg_address | 0x80 ;
    tx[1] = (data >> 24) & 0xFF; 
    tx[2] = (data >> 16) & 0xFF;
    tx[3] = (data >> 8) & 0xFF;
    tx[4] = (data >> 0) & 0xFF;
    // Set (OR |) with 0x80 is to set the 7th bit to 1 means to set the register to write mode
    // Mask (AND &) with 0xFF means to keep only the last 8 LSB to ensure no garbage

    HAL_GPIO_WritePin(TMC_GPIO_Port, TMC_CS_Pin, GPIO_PIN_RESET); // Pull CS pin low
    HAL_SPI_Transmit(&hspi1, tx, 5, HAL_MAX_DELAY); // Transmit data over SPI
    HAL_GPIO_WritePin(TMC_GPIO_Port, TMC_CS_Pin, GPIO_PIN_SET); // Pull CS pin high
}

// -- Read from driver using spi --
uint32_t tmc_spi_read(uint8_t reg_address)
{
    uint8_t tx[5] = {reg_address & 0x7F, 0, 0, 0, 0}; // an array of 5 bytes with the register address
    // Mask with 0x7F clears 7th bit to make sure it is in read mode
    uint8_t rx[5]; // an array of 5 bytes

    // First transaction - TMC5160 gets the address but returns garbage
    HAL_GPIO_WritePin(TMC_GPIO_Port, TMC_CS_Pin, GPIO_PIN_RESET); // Pull CS pin low
    HAL_SPI_TransmitReceive(&hspi1, tx, rx, 5, HAL_MAX_DELAY); // Transmit and receive data over SPI
    HAL_GPIO_WritePin(TMC_GPIO_Port, TMC_CS_Pin, GPIO_PIN_SET); // Pull CS pin high

    // Second transcation - TMC5160 returns actual data
    HAL_GPIO_WritePin(TMC_GPIO_Port, TMC_CS_Pin, GPIO_PIN_RESET); // Pull CS pin low
    HAL_SPI_TransmitReceive(&hspi1, tx, rx, 5, HAL_MAX_DELAY); // Transmit and receive data over SPI
    HAL_GPIO_WritePin(TMC_GPIO_Port, TMC_CS_Pin, GPIO_PIN_SET); // Pull CS pin high

    // Using the data received from driver to reassemble and return it
    uint32_t data = (uint32_t) rx[1] << 24 | (uint32_t) rx[2] << 16 | (uint32_t) rx[3] << 8 | (uint32_t) rx[4] << 0;
    
    return data;
}


