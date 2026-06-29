/** 
* @file TMC5160_SPI.c
* @brief TMC5160 SPI Communication
*
* Author: Andrew Nguyen
* Date: June 2026
*
* References:
* Trinamic TMC5160 Datasheet (see docs)
* Relevant datasheet page numbers are cited throughout the source code.
*/

#include "TMC5160_SPI.h"

// --- SPI COMMUNICATION ---
// -- Write to driver using spi --
void TMC5160_SPI_Write(TMC5160_TypeDef *htmc, uint8_t reg_address, uint32_t data)
{   
    if (htmc == NULL || htmc->hspi == NULL || htmc->cs.port == NULL) {return;}

    uint8_t tx_buff[5]; // an array of 5 bytes

    tx_buff[0] = reg_address | 0x80 ;
    tx_buff[1] = (data >> 24) & 0xFF; 
    tx_buff[2] = (data >> 16) & 0xFF;
    tx_buff[3] = (data >> 8) & 0xFF;
    tx_buff[4] = (data >> 0) & 0xFF;
    // Set (OR |) with 0x80 is to set the 7th bit to 1 means to set the register to write mode (pg 31)
    // Mask (AND &) with 0xFF means to keep only the last 8 LSB to ensure no garbage

    HAL_GPIO_WritePin(htmc->cs.port, htmc->cs.pin, GPIO_PIN_RESET); // Pull CS pin low
    HAL_SPI_Transmit(htmc->hspi, tx_buff, 5, HAL_MAX_DELAY); // Transmit data over SPI
    HAL_GPIO_WritePin(htmc->cs.port, htmc->cs.pin, GPIO_PIN_SET); // Pull CS pin high
}

// -- Read from driver using spi --
uint32_t TMC5160_SPI_Read(TMC5160_TypeDef *htmc, uint8_t reg_address)
{
    if (htmc == NULL || htmc->hspi == NULL || htmc->cs.port == NULL) {return 0;}

    uint8_t tx_buff[5] = {reg_address & 0x7F, 0, 0, 0, 0}; // an array of 5 bytes with the register address
    // Mask with 0x7F clears 7th bit to make sure it is in read mode
    uint8_t rx_buff[5] = {0}; // an array of 5 bytes

    // First transaction - TMC5160 gets the address but returns garbage
    HAL_GPIO_WritePin(htmc->cs.port, htmc->cs.pin, GPIO_PIN_RESET); // Pull CS pin low
    HAL_SPI_TransmitReceive(htmc->hspi, tx_buff, rx_buff, 5, HAL_MAX_DELAY); // Transmit and receive data over SPI
    HAL_GPIO_WritePin(htmc->cs.port, htmc->cs.pin, GPIO_PIN_SET); // Pull CS pin high

    // Second transcation - TMC5160 returns actual data
    HAL_GPIO_WritePin(htmc->cs.port, htmc->cs.pin, GPIO_PIN_RESET); // Pull CS pin low
    HAL_SPI_TransmitReceive(htmc->hspi, tx_buff, rx_buff, 5, HAL_MAX_DELAY); // Transmit and receive data over SPI
    HAL_GPIO_WritePin(htmc->cs.port, htmc->cs.pin, GPIO_PIN_SET); // Pull CS pin high

    // Using the data received from driver to reassemble and return it
    uint32_t data = (uint32_t) rx_buff[1] << 24 | 
                    (uint32_t) rx_buff[2] << 16 | 
                    (uint32_t) rx_buff[3] << 8 | 
                    (uint32_t) rx_buff[4] << 0;
    
    return data;
}


