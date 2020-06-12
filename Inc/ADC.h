#ifndef __ADC_H
#define __ADC_H
#include "main.h"
#include "gpio.h"

#define ADC_DATA_READY !HAL_GPIO_ReadPin(ADC_MISO_GPIO_Port, ADC_MISO_Pin)

#define ADC_WRITE 0
#define ADC_READ 0x40

#define ADC_CH_EN 0x8000
#define ADC_AINPOS(i) ((0 <= i <= 15) ? (i << 5) : NULL)
#define ADC_AINNEG 0b10110

#define ADC_SETUP_BI_UNIPOLAR0 0
#define ADC_SETUP_REF_BUF 0xC00
#define ADC_SETUP_AIN_BUF 0x300
#define ADC_STATUS_REG 0x00
#define ADC_MODE_REG 0x01
#define ADC_INTERFACE_REG 0x02
#define ADC_CHECK_REG 0x03
#define ADC_DATA_REG 0x04
#define ADC_GPIO_REG 0x06
#define ADC_ID_REG 0x07
#define ADC_CHx_REG(i) ((0 <= i <= 15) ? (0x10 + i) : NULL)
#define ADC_SETUPCONx_REG(i) ((0 <= i <= 7) ? (0x20 + i) : NULL)
#define ADC_FILTCONx_REG(i) ((0 <= i <= 7) ? (0x28 + i) : NULL)
#define ADC_OFFSETx_REG(i) ((0 <= i <= 7) ? (0x30 + i) : NULL)
#define ADC_GAINx_REG(i) ((0 <= i <= 7) ? (0x38 + i) : NULL)

#define ADC_DEFAULT_CH 16
#define ADC_T_CH 7
typedef enum
{
    ADC_SPS_31250 = 0, //  31250 Sps
    // ADC_SPS_31250 = 1 ,//  31250 Sps
    // ADC_SPS_31250 = 2 ,//  31250 Sps
    // ADC_SPS_31250 = 3 ,//  31250 Sps
    // ADC_SPS_31250 = 4 ,//  31250 Sps
    // ADC_SPS_31250 = 5 ,//  31250 Sps
    ADC_SPS_15625 = 6, //  15625 Sps
    ADC_SPS_10417 = 7, //  10417 Sps
    ADC_SPS_5208 = 8,  //  5208 Sps
    ADC_SPS_2597 = 9,  //  2597 Sps
    ADC_SPS_1007 = 10, //  1007 Sps
    ADC_SPS_503 = 11,  //  503.8 Sps
    ADC_SPS_381 = 12,  //  381 Sps
    ADC_SPS_200 = 13,  //  200.3 Sps
    ADC_SPS_100 = 14,  //  100.5 Sps
    ADC_SPS_59 = 15,   //  59.52 Sps
    ADC_SPS_49 = 16,   //  49.68 Sps
    ADC_SPS_20 = 17,   //  20.01 Sps
    ADC_SPS_16 = 18,   //  16.63 Sps
    ADC_SPS_10 = 19,   //  10 Sps
    ADC_SPS_5 = 20,    //  5 Sps
    ADC_SPS_2_5 = 21,  //  2.5 Sps
    ADC_SPS_1_25 = 22, //  1.25 Sps
} ADC_SPS;

void ADC_reset();
void ADC_config();
void ADC_update();

uint8_t ADC_SPI_READ_8();
uint16_t ADC_SPI_READ_16();
uint32_t ADC_SPI_READ_24();
uint8_t ADC_SPI_WRITE_8(uint8_t data);
uint16_t ADC_ID();
uint32_t ADC_DATA();
uint8_t ADC_CMD(uint8_t read, uint8_t addr);
#endif /* __ADC_H */