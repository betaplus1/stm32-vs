#include "usart.h"
#include <stdio.h>
#include "state.h"
#include "usart_utils.h"
#include "ADC.h"
#include "spi.h"
#include "DAC.h"

extern state State;

//Returns voltage in [uV] measured by adc channel ADC_CH.
uint64_t Voltage(uint8_t ADC_CH)
{
    return (((uint64_t)State.ADC_ValuesFiltered[ADC_CH] * 1800000) / 0xffffff);
}

//Returns Phase absolute value in [deg e-3] measured by adc channel ADC_CH.
uint64_t Phase(uint8_t ADC_CH)
{
    return 180000 - Voltage(ADC_CH) / 10;
};

//Returns Temperature in [deg C e-3] measured by adc channel ADC_CH.
int64_t Temperature(uint8_t ADC_CH)
{
    return -45000000 - 175000000 / 8 + 17500 * Voltage(ADC_T_CH) / 264;
};

//Returns Relative Power in [dB e-3] measured by adc channel ADC_CH.
int64_t Relative_Power(uint8_t ADC_CH)
{
    return (Voltage(ADC_CH) / 30) - 30000;
};
//Returns Power in [dBm  e-3] measured by adc channel ADC_CH.
int64_t RMS_Power(uint8_t ADC_CH)
{
    const int64_t cal[16] = {0, 0, 0, 0, 399874, 0, 0, 0, 0, 448454, 427524, 0, 0, 0, 0, 408285}; //offset values
    return (int64_t)(Voltage(ADC_CH) - cal[ADC_CH]) / 20 - 65000;
};

void PID()
{
    PID352();
    PID704();
}
void PID352()
{
    int P = 1000;
    int I = 10000;
    int T = 1;
    int output = 0;
    int P_error = 0;
    int I_error = 0;

    // int PD1 = State.

    // p_Error1 = (-(PD1.value_volts_mean100) +  PD2.value_volts_mean100) - (1.019) # PD1+ PD2+
    // i_Error1 = i_Error1 + (p_Error1*dt1)
    // if(time.process_time()<36000): PID_output1 = PID_output1 +(Kp1 * p_Error1 +(Ki1 * i_Error1)) # po 18h  wyÅ‚Ä…cza kompensacje
    // PS704.update(int(PID_output1))
}
void PID704()
{
    int P = 1000;
    int I = 10000;
    int T = 1;
    int output = 0;
    int P_error = 0;
    int I_error = 0;
}
