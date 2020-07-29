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
};

//Returns Phase absolute value in [deg e-3] measured by adc channel ADC_CH.
uint64_t Phase(uint8_t ADC_CH)
{
    return(180000 - Voltage(ADC_CH) / 10);
};

//Returns Temperature in [deg C e-3] measured by adc channel ADC_CH.
int64_t Temperature(uint8_t ADC_CH)
{
    return(-45000000 - 175000000 / 8 + 17500 * Voltage(ADC_T_CH) / 264);
};

//Returns Relative Power in [dB e-3] measured by adc channel ADC_CH.
int64_t Relative_Power(uint8_t ADC_CH)
{
    return((Voltage(ADC_CH) / 30) - 30000);
};
//Returns Power in [dBm  e-3] measured by adc channel ADC_CH.
int64_t RMS_Power(uint8_t ADC_CH)
{
    const int64_t cal[16] = {0, 0, 0, 0, 399874, 0, 0, 0, 0, 448454, 427524, 0, 0, 0, 0, 408285}; //offset values
    return (int64_t)(Voltage(ADC_CH) - cal[ADC_CH]) / 20 - 65000;
};

//Determines optimal setpoints of Phase shifters and detectors slope.
void Calib(){
    //704
    DAC_cmd(RF_PS_704_OFFSET + DAC_WRITE + 0); 
    DAC_cmd(RF_PS_704_FINE + DAC_WRITE + 0x7fff);    
    State.PD1_PD2_MSE =16200000000;
    uint64_t prevPhase1 = 0;
    uint64_t prevPhase2 = 0;
    for(int PS = 0;PS<0xffff;PS+=0x0700){
        DAC_cmd(RF_PS_704_OFFSET + DAC_WRITE + PS); 
        HAL_Delay(100);
        uint64_t Phase1 =(180000 - (((uint64_t)State.ADC_Values[ADC_PD1_Phase_CH] * 1800000) / 0xffffff) / 10);
        uint64_t Phase2 =(180000 - (((uint64_t)State.ADC_Values[ADC_PD2_Phase_CH] * 1800000) / 0xffffff) / 10);
        SERIAL_WRITE("Phase1: %lu\n",Phase1);
        SERIAL_WRITE("Phase2: %lu\n",Phase2);
        int64_t error1 = 90000 - Phase1;
        int64_t error2 = 90000 - Phase2;        
        int64_t MSE704 = error1*error1 + error2*error2;
        SERIAL_WRITE("error1: %li\n",error1);
        SERIAL_WRITE("error2: %li\n",error2);
        SERIAL_WRITE("MSE704: %li\n",MSE704);
        if(MSE704<State.PD1_PD2_MSE ){
            State.PD1_PD2_Offset = PS;
            State.PD1_PD2_MSE  = MSE704;
            State.PD1_Slope = prevPhase1 < Phase1 ? 1 : -1;
            State.PD2_Slope =  prevPhase2 < Phase2 ? 1 : -1;
            State.PD1_PD2_SetPoint = (int)State.PD1_Slope * (int)Phase1 +   (int)State.PD2_Slope  * (int)Phase2;

        }
        prevPhase1 = Phase1;
        prevPhase2 = Phase2;
    }
    SERIAL_WRITE("PD1+PD2 SETPOINT: %lu\n", State.PD1_PD2_SetPoint);
    SERIAL_WRITE("PD1 SLOPE: %i\n",State.PD1_Slope);
    SERIAL_WRITE("PD2 SLOPE: %i\n",State.PD2_Slope);
    DAC_cmd(RF_PS_704_OFFSET + DAC_WRITE + State.PD1_PD2_Offset); 
    //352
    DAC_cmd(RF_PS_352_OFFSET + DAC_WRITE + 0); 
    DAC_cmd(RF_PS_352_FINE + DAC_WRITE + 0x7fff);    
    State.PD3_PD4_MSE = 16200000000;
    uint64_t prevPhase3 = 0;
    uint64_t prevPhase4 = 0;
    for(int PS = 0;PS<0xffff;PS+=0x0700){
        DAC_cmd(RF_PS_352_OFFSET + DAC_WRITE + PS); 
        HAL_Delay(100);
        uint64_t Phase3 =(180000 - (((uint64_t)State.ADC_Values[ADC_PD3_Phase_CH] * 1800000) / 0xffffff) / 10);
        uint64_t Phase4 =(180000 - (((uint64_t)State.ADC_Values[ADC_PD4_Phase_CH] * 1800000) / 0xffffff) / 10);
        SERIAL_WRITE("Phase3: %lu\n",Phase3);
        SERIAL_WRITE("Phase4: %lu\n",Phase4);
        int64_t error3 = 90000 - Phase3;
        int64_t error4 = 90000 - Phase4;        
        int64_t MSE352 = error3*error3 + error4*error4;
        SERIAL_WRITE("error3: %li\n",error3);
        SERIAL_WRITE("error3: %li\n",error4);
        SERIAL_WRITE("MSE352: %li\n",MSE352);
        if(MSE352< State.PD3_PD4_MSE ){           
            State.PD3_PD4_Offset = PS; 
            State.PD3_PD4_MSE  = MSE352;
            State.PD3_Slope = prevPhase3 < Phase3 ? 1 : -1;
            State.PD4_Slope =  prevPhase4 < Phase4 ? 1 : -1;
            State.PD3_PD4_SetPoint =  (int)State.PD3_Slope * (int)Phase3 + (int)State.PD4_Slope * (int)Phase4;

        }
        prevPhase3 = Phase3;
        prevPhase4 = Phase4;
    }
    SERIAL_WRITE("PD3+PD4 SETPOINT: %lu\n",State.PD3_PD4_SetPoint);
    SERIAL_WRITE("PD3 SLOPE: %i\n", State.PD3_Slope);
    SERIAL_WRITE("PD4 SLOPE: %i\n", State.PD4_Slope);
    DAC_cmd(RF_PS_352_OFFSET + DAC_WRITE + State.PD3_PD4_Offset); 
}

void PID_Init(){
   State.PID_352_Output = 0;
   State.PID_352_P = -1;
   State.PID_352_I = -1;
   State.PID_352_T = 1;
   State.PID_352_P_error = 0;
   State.PID_352_I_error = 0;

   State.PID_704_Output = 0;
   State.PID_704_P = -1;
   State.PID_704_I = -1;
   State.PID_704_T = 1;
   State.PID_704_P_error = 0;
   State.PID_704_I_error = 0;
}
void PID352()
{
    State.PID_352_P_error = (int)State.PD3_Slope * (int)Phase(ADC_PD3_Phase_CH) + State.PD4_Slope *  (int)Phase(ADC_PD4_Phase_CH) - (int)State.PD3_PD4_SetPoint;
    // State.PID_352_I_error = State.PID_352_I_error + State.PID_352_P_error *  State.PID_704_T;
    State.PID_352_I_error = 0;
    State.PID_352_Output = (State.PID_352_Output +  State.PID_352_P * State.PID_352_P_error + State.PID_704_I *  State.PID_352_I_error) & 0xffff;
    DAC_cmd(RF_PS_352_FINE+ DAC_WRITE + State.PID_352_Output); 
    // p_Error1 = (-(PD1.value_volts_mean100) +  PD2.value_volts_mean100) - (1.019) # PD1+ PD2+
    // i_Error1 = i_Error1 + (p_Error1*dt1)
    // if(time.process_time()<36000): PID_output1 = PID_output1 +(Kp1 * p_Error1 +(Ki1 * i_Error1)) # po 18h  wyÅ‚Ä…cza kompensacje
    // PS704.update(int(PID_output1))
}
void PID704()
{
    State.PID_704_P_error = (int)State.PD1_Slope * (int)Phase(ADC_PD1_Phase_CH) + State.PD2_Slope *  (int)Phase(ADC_PD2_Phase_CH) - (int)State.PD1_PD2_SetPoint;
    // State.PID_704_I_error = State.PID_704_I_error + State.PID_704_P_error *  State.PID_704_T;
    State.PID_704_I_error = 0;
    State.PID_704_Output = (State.PID_704_Output +  State.PID_704_P * State.PID_704_P_error + State.PID_704_I *  State.PID_704_I_error ) & 0xffff;
    DAC_cmd(RF_PS_704_FINE+ DAC_WRITE + State.PID_704_Output); 
}

void PID()
{
    PID352();
    PID704();
}

