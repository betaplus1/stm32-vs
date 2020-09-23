#include "usart.h"
#include <stdio.h>
#include "state.h"
#include "usart_utils.h"
#include "ADC.h"
#include "spi.h"
#include "DAC.h"
#include "tim.h"
#include <stdlib.h>

extern state State;

//Returns voltage in [nV] measured by adc channel ADC_CH.
uint64_t Voltage_nV(uint8_t ADC_CH)
{
    return (((uint64_t)State.ADC_ValuesFiltered[ADC_CH] * 1800000000) / 0xffffff);
};

//Returns voltage in [uV] measured by adc channel ADC_CH.
uint64_t Voltage(uint8_t ADC_CH)
{
    return (((uint64_t)State.ADC_ValuesFiltered[ADC_CH] * 1800000) / 0xffffff);
};

//Returns Phase absolute value in [deg e-3] measured by adc channel ADC_CH.
uint64_t Phase(uint8_t ADC_CH)
{
    return (1800000000 - Voltage_nV(ADC_CH)) / 10000;
};

//Returns Phase absolute value in [deg e-6] measured by adc channel ADC_CH.
uint64_t Phase_u(uint8_t ADC_CH)
{
    return (1800000000 - Voltage_nV(ADC_CH)) / 10;
};

//Returns Temperature in [deg C e-6] measured by adc thermometer channel.
int Temperature()
{
    return (-45000000 - 175000000 / 8 + 17500 * Voltage(ADC_T_CH) / 264);
};

//Returns Relative Power in [dB e-3] measured by adc channel ADC_CH.
int64_t Relative_Power(uint8_t ADC_CH)
{
    return ((Voltage(ADC_CH) / 30) - 30000);
};
//Returns Power in [dBm  e-3] measured by adc channel ADC_CH.
int64_t RMS_Power(uint8_t ADC_CH)
{
    const int64_t cal[16] = {0, 0, 0, 0, 399874, 0, 0, 0, 0, 448454, 427524, 0, 0, 0, 0, 408285}; //offset values
    return (int64_t)(Voltage(ADC_CH) - cal[ADC_CH]) / 20 - 65000;
};
