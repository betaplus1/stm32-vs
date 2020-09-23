#ifndef __STATE
#define __STATE

#define FilterLength 16
typedef struct state
{
    uint64_t uptime;
    uint8_t timer_1s_flag;

    uint8_t error;
    uint8_t cmd;
    uint8_t cmdLoop;
    uint8_t UART_RX_BUFF[UART_RX_BUFFER_LENGTH];

    uint8_t ADC_Channel;
    uint8_t ADC_Updated;
    uint32_t ADC_Values[16];
    uint32_t ADC_ValuesBuffer[FilterLength][16];
    uint32_t ADC_ValuesFiltered[16];
    uint8_t ADC_Filter_Iteration;
    uint8_t ADC_Filter_Valid;

    //PHASE PIDS

    int PD1_Slope;
    int PD2_Slope;
    int PD3_Slope;
    int PD4_Slope;
    int PD1_PD2_SetPoint;
    int PD1_PD2_Offset;  //offset dac value
    int64_t PD1_PD2_MSE; //MEAN SQUARE ERROR from the middle of the characteristics slope
    int PD3_PD4_SetPoint;
    int PD3_PD4_Offset; //offset dac value
    int64_t PD3_PD4_MSE;

    int PID_352_Output_Fine;
    int PID_352_Output_Coarse;
    int PID_352_P_error;
    int PID_352_I_error;
    int PID_352_LOCK;

    int PID_704_Output_Fine;
    int PID_704_Output_Coarse;
    int PID_704_P_error;
    int PID_704_I_error;
    int PID_704_LOCK;

    //POWER PIDS:

    // uint32_t POWER_PID_352_SetPoint;
    // uint32_t POWER_PID_352_Output;
    // int64_t POWER_PID_352_P;
    // int64_t POWER_PID_352_I;
    // int64_t POWER_PID_352_T;
    // int64_t POWER_PID_352_P_error;
    // int64_t POWER_PID_352_I_error;
    // int8_t POWER_PID_352_LOCK;

    // uint32_t POWER_PID_704_Output;
    // uint32_t POWER_PID_704_OutputFINE;
    // int64_t POWER_PID_704_P;
    // int64_t POWER_PID_704_I;
    // int64_t POWER_PID_704_T;
    // int64_t POWER_PID_704_P_error;
    // int64_t POWER_PID_704_I_error;
    // int8_t POWER_PID_704_LOCK;

    //TEMPERATURE PID:
    int TEMP;
    int TEMP_PID_SetPoint;
    int TEMP_PID_P_Error;
    int TEMP_PID_I_Error;
    int TEMP_PID_PWM;
    int TEMP_PID_DIR;
    int TEMP_PID_LOCK;
    int TEMP_PID_LOCK_count;

} state;

#endif /* __STATE */
