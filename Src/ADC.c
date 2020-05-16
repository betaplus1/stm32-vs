#include "usart.h"
#include <stdio.h>
#include "state.h"
#include "spi.h"
#include "ADC.h"
#include "usart_utils.h"
#include "main.h"

extern state State;

uint8_t ADC_SPI_WRITE_8(uint8_t data)
{
    uint8_t txbuffer[1] = {data};
    uint8_t rxbuffer[1] = {0};
    HAL_SPI_TransmitReceive(&hspi2, txbuffer, rxbuffer, 1, HAL_MAX_DELAY);
    uint8_t answer = rxbuffer[0];
    return answer;
}

uint16_t ADC_SPI_READ_16()
{
    uint8_t rxbuffer[2] = {0};
    HAL_SPI_Receive(&hspi2, rxbuffer, 2, HAL_MAX_DELAY);
    uint16_t answer = (((uint16_t)rxbuffer[1]) << 8) | ((uint16_t)rxbuffer[0]);
    return answer;
}

uint32_t ADC_SPI_READ_24()
{
    uint8_t rxbuffer[3] = {0};
    HAL_SPI_Receive(&hspi2, rxbuffer, 3, HAL_MAX_DELAY);
    uint32_t answer = (((uint32_t)rxbuffer[2]) << 16) | (((uint32_t)rxbuffer[1]) << 8) | ((uint32_t)rxbuffer[0]);
    return answer;
}

uint8_t ADC_SPI_READ_8()
{
    uint8_t rxbuffer[1] = {0};
    HAL_SPI_Receive(&hspi2, rxbuffer, 1, HAL_MAX_DELAY);
    uint8_t answer = rxbuffer[0];
    return answer;
}

uint8_t ADC_CMD(uint8_t read, uint8_t addr)
{
    uint8_t command = read | addr;
    ADC_SPI_WRITE_8(command);
    return command;
}

uint16_t ADC_ID()
{
    ADC_SPI_WRITE_8(0x47);
    // ADC_CMD(ADC_READ, ID_REG);
    uint16_t answer = ADC_SPI_READ_16();
    return answer;
}

void ADC_reset()
{
    uint8_t tx[8] = {0xff};
    HAL_SPI_Transmit(&hspi2, tx, sizeof tx, 100000);
};
