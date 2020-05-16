#ifndef __ADC_H
#define __ADC_H
#include "main.h"

#define ADC_WRITE 0
#define ADC_READ 0x40

#define ID_REG 0x07

void ADC_reset();
uint8_t ADC_SPI_READ_8();
uint16_t ADC_SPI_READ_16();
uint32_t ADC_SPI_READ_24();
uint8_t ADC_SPI_WRITE_8(uint8_t data);
uint16_t ADC_ID();
uint8_t ADC_CMD(uint8_t read, uint8_t addr);
// TO DO:

// uint16_t ADC_CH_CONFIG(bool CH_EN, uint8_t SETUP, uint8_t INPUT_P, uint8_t INPUT_N);

#endif /* __ADC_H */