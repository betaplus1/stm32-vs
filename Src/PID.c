#include "usart.h"
#include <stdio.h>
#include "state.h"
#include "usart_utils.h"
#include "ADC.h"
#include "spi.h"
#include "DAC.h"

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

//Returns Temperature in [deg C e-3] measured by adc thermometer channel.
int64_t Temperature()
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

//Determines optimal setpoints of Phase shifters and detectors slope.
void Calib()
{
    //704
    DAC_cmd(RF_PS_704_OFFSET + DAC_WRITE + 0);
    DAC_cmd(RF_PS_704_FINE + DAC_WRITE + 0x7fff);
    State.PD1_PD2_MSE = 16200000000;
    uint64_t prevPhase1 = 0;
    uint64_t prevPhase2 = 0;
    for (uint32_t PS = 0; PS < 0xffff; PS += 0x0700)
    {
        DAC_cmd(RF_PS_704_OFFSET + DAC_WRITE + PS);
        HAL_Delay(100);
        uint64_t Phase1 = (1800000 - (((uint64_t)State.ADC_Values[ADC_PD1_Phase_CH] * 1800000) / 0xffffff)) / 10;
        uint64_t Phase2 = (1800000 - (((uint64_t)State.ADC_Values[ADC_PD2_Phase_CH] * 1800000) / 0xffffff)) / 10;

        int64_t error1 = 90000 - Phase1;
        int64_t error2 = 90000 - Phase2;
        int64_t MSE704 = error1 * error1 + error2 * error2;

        if (MSE704 < State.PD1_PD2_MSE)
        {
            SERIAL_WRITE("Phase1: %lu\n", Phase1);
            SERIAL_WRITE("Phase2: %lu\n", Phase2);
            SERIAL_WRITE("error1: %li\n", error1);
            SERIAL_WRITE("error2: %li\n", error2);
            SERIAL_WRITE("MSE704: %li\n\n", MSE704);
            State.PD1_PD2_Offset = PS;
            State.PD1_PD2_MSE = MSE704;
            State.PD1_Slope = prevPhase1 < Phase1 ? 1 : -1;
            State.PD2_Slope = prevPhase2 < Phase2 ? 1 : -1;
            State.PD1_PD2_SetPoint = State.PD1_Slope * Phase1 + State.PD2_Slope * Phase2;
        }
        prevPhase1 = Phase1;
        prevPhase2 = Phase2;
    }
    SERIAL_WRITE("PD1+PD2 SETPOINT: %li\n", State.PD1_PD2_SetPoint);
    SERIAL_WRITE("PD1 SLOPE: %i\n", State.PD1_Slope);
    SERIAL_WRITE("PD2 SLOPE: %i\n", State.PD2_Slope);
    SERIAL_WRITE("PS704 OFFSET: %u\n", State.PD1_PD2_Offset);
    DAC_cmd(RF_PS_704_OFFSET + DAC_WRITE + State.PD1_PD2_Offset);

    //352
    DAC_cmd(RF_PS_352_OFFSET + DAC_WRITE + 0);
    DAC_cmd(RF_PS_352_FINE + DAC_WRITE + 0x7fff);
    State.PD3_PD4_MSE = 16200000000;
    uint64_t prevPhase3 = 0;
    uint64_t prevPhase4 = 0;
    for (uint32_t PS = 0; PS < 0xffff; PS += 0x0700)
    {
        DAC_cmd(RF_PS_352_OFFSET + DAC_WRITE + PS);
        HAL_Delay(100);
        uint64_t Phase3 = (1800000 - (((uint64_t)State.ADC_Values[ADC_PD3_Phase_CH] * 1800000) / 0xffffff)) / 10;
        uint64_t Phase4 = (1800000 - (((uint64_t)State.ADC_Values[ADC_PD4_Phase_CH] * 1800000) / 0xffffff)) / 10;

        int64_t error3 = 90000 - Phase3;
        int64_t error4 = 90000 - Phase4;
        int64_t MSE352 = error3 * error3 + error4 * error4;

        if (MSE352 < State.PD3_PD4_MSE)
        {
            SERIAL_WRITE("Phase3: %lu\n", Phase3);
            SERIAL_WRITE("Phase4: %lu\n", Phase4);
            SERIAL_WRITE("error3: %li\n", error3);
            SERIAL_WRITE("error3: %li\n", error4);
            SERIAL_WRITE("MSE352: %li\n\n", MSE352);

            State.PD3_PD4_Offset = PS;
            State.PD3_PD4_MSE = MSE352;
            State.PD3_Slope = prevPhase3 < Phase3 ? 1 : -1;
            State.PD4_Slope = prevPhase4 < Phase4 ? 1 : -1;
            State.PD3_PD4_SetPoint = State.PD3_Slope * Phase3 + State.PD4_Slope * Phase4;
        }
        prevPhase3 = Phase3;
        prevPhase4 = Phase4;
    }
    SERIAL_WRITE("PD3+PD4 SETPOINT: %li\n", State.PD3_PD4_SetPoint);
    SERIAL_WRITE("PD3 SLOPE: %i\n", State.PD3_Slope);
    SERIAL_WRITE("PD4 SLOPE: %i\n", State.PD4_Slope);
    SERIAL_WRITE("PS352 OFFSET: %u\n", State.PD3_PD4_Offset);

    DAC_cmd(RF_PS_352_OFFSET + DAC_WRITE + State.PD3_PD4_Offset);
}

void PID_Init()
{
    State.PID_352_Output = 0x7fff;
    State.PID_352_P = -1;
    State.PID_352_I = -1;
    State.PID_352_T = 1000000;
    State.PID_352_P_error = 0;
    State.PID_352_I_error = 0;

    State.PID_704_Output = 0x7fff;
    State.PID_704_P = -1;
    State.PID_704_I = -1;
    State.PID_704_T = 1000000;
    State.PID_704_P_error = 0;
    State.PID_704_I_error = 0;
}
void PID352()
{
    State.PID_352_P_error = (State.PD3_Slope * Phase_u(ADC_PD3_Phase_CH)) + (State.PD4_Slope * Phase_u(ADC_PD4_Phase_CH)) - (State.PD3_PD4_SetPoint * 1000);
    State.PID_352_I_error = (State.PID_352_I_error + State.PID_352_P_error) / State.PID_352_T;
    State.PID_352_Output = State.PID_352_Output + (State.PID_352_P * State.PID_352_P_error) / 1000 + (State.PID_352_I * State.PID_352_I_error);
    if (State.PID_352_Output >= 0xffff)
    {
        State.PID_352_Output = 0xffff;
    }

    DAC_cmd(RF_PS_352_FINE + DAC_WRITE + State.PID_352_Output);
}
void PID704()
{
    State.PID_704_P_error = (State.PD1_Slope * Phase_u(ADC_PD1_Phase_CH)) + (State.PD2_Slope * Phase_u(ADC_PD2_Phase_CH)) - (State.PD1_PD2_SetPoint * 1000);
    State.PID_704_I_error = (State.PID_704_I_error + State.PID_704_P_error) / State.PID_704_T;
    State.PID_704_Output = State.PID_704_Output + (State.PID_704_P * State.PID_704_P_error) / 1000 + (State.PID_704_I * State.PID_704_I_error);

    if (State.PID_704_Output >= 0xffff)
    {
        State.PID_704_Output = 0xffff;
    }
    DAC_cmd(RF_PS_704_FINE + DAC_WRITE + State.PID_704_Output);
}

void PID()
{
    PID352();
    PID704();

    SERIAL_WRITE("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n")
    SERIAL_WRITE("PD1+PD2 SETPOINT: %li\n", State.PD1_PD2_SetPoint * 1000);
    SERIAL_WRITE("PID 704 Output: %li\n", State.PID_704_Output);
    SERIAL_WRITE("PID_704_P_error %i\n", State.PID_704_P_error);

    SERIAL_WRITE("PD1 Phase:\t\t");
    SERIAL_WRITE("%3i.", Phase_u(ADC_PD1_Phase_CH) / 1000000);
    SERIAL_WRITE("%06li deg\t\t\n", Phase_u(ADC_PD1_Phase_CH) % 1000000);

    SERIAL_WRITE("PD2 Phase:\t\t");
    SERIAL_WRITE("%3i.", Phase_u(ADC_PD2_Phase_CH) / 1000000);
    SERIAL_WRITE("%03li deg\t\t\n\n", Phase_u(ADC_PD2_Phase_CH) % 1000000);

    SERIAL_WRITE("PD3+PD4 SETPOINT: %li\n", State.PD3_PD4_SetPoint * 1000);
    SERIAL_WRITE("PID 352 Output: %li\n", State.PID_352_Output);
    SERIAL_WRITE("PID_352_P_error %i\n", State.PID_352_P_error);

    SERIAL_WRITE("PD3 Phase:\t\t");
    SERIAL_WRITE("%3i.", Phase_u(ADC_PD3_Phase_CH) / 1000000);
    SERIAL_WRITE("%06li deg\t\t\n", Phase_u(ADC_PD3_Phase_CH) % 1000000);

    SERIAL_WRITE("PD4 Phase:\t\t");
    SERIAL_WRITE("%3i.", Phase_u(ADC_PD4_Phase_CH) / 1000000);
    SERIAL_WRITE("%06li deg\t\t\n", Phase_u(ADC_PD4_Phase_CH) % 1000000);
}
