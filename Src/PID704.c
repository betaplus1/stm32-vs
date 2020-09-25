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
void Calib704()
{
    SERIAL_WRITE("[CALIBRATION] ...")
    DAC_cmd(RF_PS_704_OFFSET + DAC_WRITE + 0);
    DAC_cmd(RF_PS_704_FINE + DAC_WRITE + 0x7fff);
    State.PD1_PD2_MSE = 16200000000;
    int64_t prevPhase1 = 0;
    int64_t prevPhase2 = 0;
    for (unsigned PS = 0; PS < 0xffff; PS += 0x0700)
    {
        DAC_cmd(RF_PS_704_OFFSET + DAC_WRITE + PS);
        HAL_Delay(100);
        int64_t Phase1 = (1800000 - (((uint64_t)State.ADC_Values[ADC_PD1_Phase_CH] * 1800000) / 0xffffff)) / 10;
        int64_t Phase2 = (1800000 - (((uint64_t)State.ADC_Values[ADC_PD2_Phase_CH] * 1800000) / 0xffffff)) / 10;

        int error1 = 90 - Phase1 / 1000;
        int error2 = 90 - Phase2 / 1000;
        int MSE704 = error1 * error1 + error2 * error2;
        // SERIAL_WRITE("PS: %u\n", PS);
        // SERIAL_WRITE("Phase3: %li\n", Phase1);
        // SERIAL_WRITE("Phase4: %li\n", Phase2);
        // SERIAL_WRITE("MSE: %i\n", MSE704);
        if (abs(MSE704) < abs(State.PD1_PD2_MSE))
        {
            State.PD1_PD2_Offset = PS;
            State.PD1_PD2_MSE = MSE704;
            State.PD1_Slope = prevPhase1 < Phase1 ? 1 : -1;
            State.PD2_Slope = prevPhase2 < Phase2 ? 1 : -1;
            State.PD1_PD2_SetPoint = State.PD1_Slope * Phase1 + State.PD2_Slope * Phase2;
        }
        prevPhase1 = Phase1;
        prevPhase2 = Phase2;
    }
    SERIAL_WRITE("\b\b\bPD1+PD2 SETPOINT: %i\n", State.PD1_PD2_SetPoint);
    SERIAL_WRITE("[CALIBRATION] PD1 SLOPE: %i\n", State.PD1_Slope);
    SERIAL_WRITE("[CALIBRATION] PD2 SLOPE: %i\n", State.PD2_Slope);
    SERIAL_WRITE("[CALIBRATION] PS704 OFFSET: %i\n", State.PD1_PD2_Offset);
    DAC_cmd(RF_PS_704_OFFSET + DAC_WRITE + State.PD1_PD2_Offset);
}

void PID704_Init()
{
    State.PID_704_Output_Fine = 0x7fff;
    State.PID_704_Output_Coarse = State.PD1_PD2_Offset;
    State.PID_704_P_error = 0;
    State.PID_704_I_error = 0;
    State.PID_704_LOCK = 0;
};

void PID704()
{

    State.PID_704_P_error = (State.PD1_Slope * Phase_u(ADC_PD1_Phase_CH)) + (State.PD2_Slope * Phase_u(ADC_PD2_Phase_CH)) - (State.PD1_PD2_SetPoint * 1000);
    State.PID_704_I_error = State.PID_704_I_error + State.PID_704_P_error;

    int sat = State.PID_704_Output_Fine - State.PID_704_P_error / 5000;

    sat = abs(sat);

    if (sat > 0xffff)
    {
        sat = 0xffff;
    }
    else if (sat < 0)
    {
        sat = 0;
    }

    State.PID_704_Output_Fine = sat;
    DAC_cmd(RF_PS_704_FINE + DAC_WRITE + State.PID_704_Output_Fine);

    sat = State.PID_704_Output_Coarse - State.PID_704_I_error / 1000000;

    if (sat > 0xffff)
    {
        sat = 0xffff;
    }
    else if (sat < 0)
    {
        sat = 0;
    }
    State.PID_704_Output_Coarse = sat;
    DAC_cmd(RF_PS_704_OFFSET + DAC_WRITE + State.PID_704_Output_Coarse);
};
