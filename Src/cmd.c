#include "usart.h"
#include <stdio.h>
#include "state.h"
#include "usart_utils.h"
#include "ADC.h"

extern uint8_t COMMAND;

uint8_t cmd()
{
    switch (COMMAND)
    {
    case cmd_blink:
    {
        COMMAND = 0;
        SERIAL_WRITE("ok\n");
        for (int i = 0; i < 10; i++)
        {
            HAL_GPIO_TogglePin(LED_PHASE_GPIO_Port, LED_PHASE_Pin);
            HAL_Delay(100);
        }
        return 0;
    }
    case cmd_adc_id:
    {
        SERIAL_WRITE("0x%06x\n", ADC_ID());
        COMMAND = 0;
        return 0;
    }
    case cmd_adc_data:
    {
        ADC_CMD(ADC_READ, ADC_DATA_REG);
        SERIAL_WRITE("0x%06x\n", ADC_SPI_READ_24());
        COMMAND = 0;
        return 0;
    }
    case cmd_adc_reset:
    {
        SERIAL_WRITE("ADC RESET\n");
        ADC_reset();
        COMMAND = 0;
        return 0;
    }
    case cmd_adc_debug:
    {
        SERIAL_WRITE("ADC DEBUG:\n\n");
        ADC_CMD(ADC_READ, ADC_STATUS_REG);
        SERIAL_WRITE("\tADC_STATUS_REG:\t\t0x%02x\n", ADC_SPI_READ_8());
        ADC_CMD(ADC_READ, ADC_MODE_REG);
        SERIAL_WRITE("\tADC_MODE_REG:\t\t0x%04x\n", ADC_SPI_READ_16());
        ADC_CMD(ADC_READ, ADC_INTERFACE_REG);
        SERIAL_WRITE("\tADC_INTERFACE_REG:\t0x%04x\n", ADC_SPI_READ_16());
        ADC_CMD(ADC_READ, ADC_CHECK_REG);
        SERIAL_WRITE("\tADC_CHECK_REG:\t\t0x%06x\n", ADC_SPI_READ_24());
        ADC_CMD(ADC_READ, ADC_DATA_REG);
        SERIAL_WRITE("\tADC_DATA_REG:\t\t0x%06x\n", ADC_SPI_READ_24());
        ADC_CMD(ADC_READ, ADC_GPIO_REG);
        SERIAL_WRITE("\tADC_GPIO_REG:\t\t0x%04x\n", ADC_SPI_READ_16());
        ADC_CMD(ADC_READ, ADC_ID_REG);
        SERIAL_WRITE("\tADC_ID_REG:\t\t0x%04x\n", ADC_SPI_READ_16());
        for (int i = 0; i < 15; i++)
        {
            ADC_CMD(ADC_READ, ADC_CHx_REG(i));
            SERIAL_WRITE("\tADC_CH%i_REG:\t\t0x%04x\n", i, ADC_SPI_READ_16());
        }
        for (int i = 0; i < 7; i++)
        {
            ADC_CMD(ADC_READ, ADC_SETUPCONx_REG(i));
            SERIAL_WRITE("\tADC_SETUPCON%i_REG:\t0x%04x\n", i, ADC_SPI_READ_16());
        }
        for (int i = 0; i < 7; i++)
        {
            ADC_CMD(ADC_READ, ADC_FILTCONx_REG(i));
            SERIAL_WRITE("\tADC_FILTCON%i_REG:\t0x%04x\n", i, ADC_SPI_READ_16());
        }
        for (int i = 0; i < 7; i++)
        {
            ADC_CMD(ADC_READ, ADC_OFFSETx_REG(i));
            SERIAL_WRITE("\tADC_OFFSET%i_REG:\t0x%04x\n", i, ADC_SPI_READ_24());
        }
        for (int i = 0; i < 7; i++)
        {
            ADC_CMD(ADC_READ, ADC_GAINx_REG(i));
            SERIAL_WRITE("\tADC_GAIN%i_REG:\t\t0x%04x\n", i, ADC_SPI_READ_24());
        }
        COMMAND = 0;
        return 0;
    }
    case cmd_undefined:
    {
        COMMAND = 0;
        SERIAL_WRITE("undefined\n");
        HAL_GPIO_TogglePin(LED_PHASE_GPIO_Port, LED_PHASE_Pin);
        HAL_Delay(100);
        HAL_GPIO_TogglePin(LED_PHASE_GPIO_Port, LED_PHASE_Pin);
        HAL_Delay(100);
        return 0;
    }
    default:
    {
        return 0;
    }
    }
}
