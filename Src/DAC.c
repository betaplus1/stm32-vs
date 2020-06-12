#include "usart.h"
#include <stdio.h>
#include "state.h"
#include "spi.h"
#include "DAC.h"
#include "usart_utils.h"
#include "main.h"

extern state State;

void DAC_SPI_WRITE_24(uint32_t data)
{
    uint8_t txbuffer[3] = {
        (uint8_t)((data & 0xff0000) >> 16),
        (uint8_t)((data & 0xff00) >> 8),
        (uint8_t)((data & 0xff))};
    HAL_SPI_Transmit(&hspi1, txbuffer, 3, HAL_MAX_DELAY);
}

void DAC_TEST()
{
    HAL_GPIO_WritePin(DAC_nLOAD_GPIO_Port, DAC_nLOAD_Pin, 1);
    for (int i = 0; i <= 0xffff; i += 1000)
    {

        HAL_GPIO_WritePin(DAC_nSYNC_GPIO_Port, DAC_nSYNC_Pin, 1);
        HAL_Delay(1);
        HAL_GPIO_WritePin(DAC_nSYNC_GPIO_Port, DAC_nSYNC_Pin, 0);
        DAC_SPI_WRITE_24(DAC_WRITE_AND_UPDATE + i);
        HAL_GPIO_WritePin(DAC_nLOAD_GPIO_Port, DAC_nLOAD_Pin, 0);
        HAL_Delay(1);
        HAL_GPIO_WritePin(DAC_nLOAD_GPIO_Port, DAC_nLOAD_Pin, 1);
        SERIAL_WRITE("%06x\n", DAC_WRITE_AND_UPDATE + i)
        // HAL_GPIO_WritePin(DAC_nLOAD_GPIO_Port, DAC_nLOAD_Pin, 1);
        HAL_Delay(100);
    }
}