#include "usart.h"
#include <stdio.h>
#include "state.h"
#include "usart_utils.h"
#include "ADC.h"
#include "spi.h"
#include "DAC.h"
#include "tim.h"
#include <stdlib.h>
#include "values.h"

extern state State;

//Determines optimal setpoints of Phase shifters, detectors slopes
void Calib352()
{
    SERIAL_WRITE("[CALIBRATION] ...")
    DAC_cmd(RF_PS_352_OFFSET + DAC_WRITE + 0);
    DAC_cmd(RF_PS_352_FINE + DAC_WRITE + 0x7fff);
    State.PD3_PD4_MSE = 16200000000;
    int64_t prevPhase3 = 0;
    int64_t prevPhase4 = 0;
    for (unsigned PS = 0; PS < 0xffff; PS += 0x0700)
    {
        DAC_cmd(RF_PS_352_OFFSET + DAC_WRITE + PS);
        HAL_Delay(100);
        int64_t Phase3 = (1800000 - (((uint64_t)State.ADC_Values[ADC_PD3_Phase_CH] * 1800000) / 0xffffff)) / 10;
        int64_t Phase4 = (1800000 - (((uint64_t)State.ADC_Values[ADC_PD4_Phase_CH] * 1800000) / 0xffffff)) / 10;

        int error3 = 90 - Phase3 / 1000;
        int error4 = 90 - Phase4 / 1000;
        int MSE352 = error3 * error3 + error4 * error4;
        // SERIAL_WRITE("PS: %u\n", PS);
        // SERIAL_WRITE("Phase3: %li\n", Phase1);
        // SERIAL_WRITE("Phase4: %li\n", Phase2);
        // SERIAL_WRITE("MSE: %i\n", MSE352);
        if (abs(MSE352) < abs(State.PD3_PD4_MSE))
        {
            State.PD3_PD4_Offset = PS;
            State.PD3_PD4_MSE = MSE352;
            State.PD3_Slope = prevPhase3 < Phase3 ? 1 : -1;
            State.PD4_Slope = prevPhase4 < Phase4 ? 1 : -1;
            State.PD3_PD4_SetPoint = State.PD3_Slope * Phase3 + State.PD4_Slope * Phase4;
        }
        prevPhase3 = Phase3;
        prevPhase4 = Phase4;
    }
    SERIAL_WRITE("\b\b\bPD3+PD4 SETPOINT: %i\n", State.PD3_PD4_SetPoint);
    SERIAL_WRITE("[CALIBRATION] PD3 SLOPE: %i\n", State.PD3_Slope);
    SERIAL_WRITE("[CALIBRATION] PD4 SLOPE: %i\n", State.PD4_Slope);
    SERIAL_WRITE("[CALIBRATION] PS352 OFFSET: %i\n", State.PD3_PD4_Offset);
    DAC_cmd(RF_PS_352_OFFSET + DAC_WRITE + State.PD3_PD4_Offset);
}

void PID352_Init()
{
    State.PID_352_Output_Fine = 0x7fff;
    State.PID_352_Output_Coarse = State.PD3_PD4_Offset;
    State.PID_352_P_error = 0;
    State.PID_352_I_error = 0;
    State.PID_352_LOCK = 0;
};

void PID352()
{

    State.PID_352_P_error = (State.PD3_Slope * Phase_u(ADC_PD3_Phase_CH)) + (State.PD4_Slope * Phase_u(ADC_PD4_Phase_CH)) - (State.PD3_PD4_SetPoint * 1000);
    State.PID_352_I_error = State.PID_352_I_error + State.PID_352_P_error;

    int sat = State.PID_352_Output_Fine - State.PID_352_P_error / 1000;

    sat = abs(sat);

    if (sat > 0xffff)
    {
        sat = 0xffff;
    }
    else if (sat < 0)
    {
        sat = 0;
    }

    State.PID_352_Output_Fine = sat;
    DAC_cmd(RF_PS_352_FINE + DAC_WRITE + State.PID_352_Output_Fine);

    sat = State.PID_352_Output_Coarse - State.PID_352_I_error / 100000;

    if (sat > 0xffff)
    {
        sat = 0xffff;
    }
    else if (sat < 0)
    {
        sat = 0;
    }
    State.PID_352_Output_Coarse = sat;
    DAC_cmd(RF_PS_352_OFFSET + DAC_WRITE + State.PID_352_Output_Coarse);
};
