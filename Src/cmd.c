#include "usart.h"
#include <stdio.h>
#include "state.h"
#include "usart_utils.h"
#include "ADC.h"
#include "spi.h"
#include "DAC.h"
#include "PID.h"
#include <stdlib.h>
extern state State;

void cmd()
{
    HAL_GPIO_TogglePin(LED_PHASE_GPIO_Port, LED_PHASE_Pin);
    switch (State.cmd)
    {
    case cmd_blink:
    {
        State.cmd = 0;
        SERIAL_WRITE("ok\n");
        for (int i = 0; i < 10; i++)
        {
            HAL_GPIO_TogglePin(LED_PHASE_GPIO_Port, LED_PHASE_Pin);
            HAL_Delay(100);
        }
        break;
    }
    case cmd_adc_id:
    {
        SERIAL_WRITE("0x%06x\n", ADC_ID());
        State.cmd = 0;
        break;
    }
    // case cmd_adc_data:
    // {
    //     ADC_CMD(ADC_READ, ADC_DATA_REG);
    //     SERIAL_WRITE("0x%06x\n", ADC_SPI_READ_24());
    //     State.cmd = 0;
    //     break;
    // }
    // case cmd_adc_reset:
    // {
    //     SERIAL_WRITE("ADC RESET\n");
    //     ADC_reset();
    //     State.cmd = 0;
    //     break;
    // }
    case cmd_adc_debug:
    {
        ADC_DATA_READY_EXTI_deInit();
        HAL_SPI_MspInit(&hspi2);
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
        State.cmd = 0;
        HAL_SPI_MspDeInit(&hspi2);
        ADC_DATA_READY_EXTI_Init();
        break;
    }
    case cmd_adc_values:
    {
        for (int i = 0; i < 16; i++)
        {
            uint64_t voltage_uV = (((uint64_t)State.ADC_Values[i] * 1800000) / 0xffffff);
            SERIAL_WRITE("[%i]\t", i);
            SERIAL_WRITE("%10i uV\t\t", voltage_uV);
            SERIAL_WRITE("0x%06x\n", State.ADC_Values[i]);
        }
        State.cmd = 0;
        break;
    }
    case cmd_temperature:
    {
        SERIAL_WRITE("%i.", Temperature() / 1000000);             //-66.875 to +52.443 C
        SERIAL_WRITE("%03u *C\n", (Temperature() / 1000) % 1000); //-66.875 to +52.443 C
        State.cmd = 0;
        break;
    }
    case cmd_dac_test:
    {
        SERIAL_WRITE("\n");
        DAC_test();
        State.cmd = 0;
        break;
    }
    case cmd_rf_state:
    {
        SERIAL_WRITE("\n\n\n\n\n")
        SERIAL_WRITE("PD0 Phase:\t\t");
        SERIAL_WRITE("%3i.", Phase(ADC_PD0_Phase_CH) / 1000);
        SERIAL_WRITE("%03lu deg\t\t\n", Phase(ADC_PD0_Phase_CH) % 1000);

        SERIAL_WRITE("PD1 Phase:\t\t");
        SERIAL_WRITE("%3i.", Phase(ADC_PD1_Phase_CH) / 1000);
        SERIAL_WRITE("%03lu deg\t\t\n", Phase(ADC_PD1_Phase_CH) % 1000);

        SERIAL_WRITE("PD2 Phase:\t\t");
        SERIAL_WRITE("%3i.", Phase(ADC_PD2_Phase_CH) / 1000);
        SERIAL_WRITE("%03lu deg\t\t\n", Phase(ADC_PD2_Phase_CH) % 1000);

        SERIAL_WRITE("PD3 Phase:\t\t");
        SERIAL_WRITE("%3i.", Phase(ADC_PD3_Phase_CH) / 1000);
        SERIAL_WRITE("%03lu deg\t\t\n", Phase(ADC_PD3_Phase_CH) % 1000);

        SERIAL_WRITE("PD4 Phase:\t\t");
        SERIAL_WRITE("%3i.", Phase(ADC_PD4_Phase_CH) / 1000);
        SERIAL_WRITE("%03lu deg\t\t\n", Phase(ADC_PD4_Phase_CH) % 1000);

        SERIAL_WRITE("PD0 Rel Power:\t\t");
        SERIAL_WRITE("%3li.", Relative_Power(ADC_PD0_Power_CH) / 1000);
        SERIAL_WRITE("%03u dB\t\t\n", abs(Relative_Power(ADC_PD0_Power_CH)) % 1000);

        SERIAL_WRITE("PD1 Rel Power:\t\t");
        SERIAL_WRITE("%3li.", Relative_Power(ADC_PD1_Power_CH) / 1000);
        SERIAL_WRITE("%03u dB\t\t\n", abs(Relative_Power(ADC_PD1_Power_CH)) % 1000);

        SERIAL_WRITE("PD2 Rel Power:\t\t");
        SERIAL_WRITE("%3li.", Relative_Power(ADC_PD2_Power_CH) / 1000);
        SERIAL_WRITE("%03u dB\t\t\n", abs(Relative_Power(ADC_PD2_Power_CH)) % 1000);

        SERIAL_WRITE("PD3 Rel Power:\t\t");
        SERIAL_WRITE("%3li.", Relative_Power(ADC_PD3_Power_CH) / 1000);
        SERIAL_WRITE("%03u dB\t\t\n", abs(Relative_Power(ADC_PD3_Power_CH)) % 1000);

        SERIAL_WRITE("PD4 Rel Power:\t\t");
        SERIAL_WRITE("%3li.", Relative_Power(ADC_PD4_Power_CH) / 1000);
        SERIAL_WRITE("%03u dB\t\t\n", abs(Relative_Power(ADC_PD4_Power_CH)) % 1000);

        SERIAL_WRITE("PWD0 Power:\t\t");
        SERIAL_WRITE("%3li.", RMS_Power(ADC_PWD0_Power_CH) / 1000);
        SERIAL_WRITE("%03u dBm\t\t\n", abs(RMS_Power(ADC_PWD0_Power_CH)) % 1000);

        SERIAL_WRITE("PWD1 Power:\t\t");
        SERIAL_WRITE("%3li.", RMS_Power(ADC_PWD1_Power_CH) / 1000);
        SERIAL_WRITE("%03u dBm\t\t\n", abs(RMS_Power(ADC_PWD1_Power_CH)) % 1000);

        SERIAL_WRITE("PWD2 Power:\t\t");
        SERIAL_WRITE("%3li.", RMS_Power(ADC_PWD2_Power_CH) / 1000);
        SERIAL_WRITE("%03u dBm\t\t\n", abs(RMS_Power(ADC_PWD2_Power_CH)) % 1000);

        SERIAL_WRITE("PWD3 Power:\t\t");
        SERIAL_WRITE("%3li.", RMS_Power(ADC_PWD3_Power_CH) / 1000);
        SERIAL_WRITE("%03u dBm\t\t\n", abs(RMS_Power(ADC_PWD3_Power_CH)) % 1000);
        State.cmd = 0;

        break;
    }
    case cmd_undefined:
    {
        State.cmd = 0;
        SERIAL_WRITE("undefined\n");
        break;
    }
    default:
    {
        break;
    }
    }
}
