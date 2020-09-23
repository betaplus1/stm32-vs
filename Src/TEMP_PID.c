#include "usart.h"
#include <stdio.h>
#include "state.h"
#include "usart_utils.h"
#include "ADC.h"
#include "spi.h"
#include "DAC.h"
#include "tim.h"
#include "PID.h"
#include "values.h"
#include <stdlib.h>

extern state State;

void TEMP_PID_Init()
{
    State.TEMP = 0;
    State.TEMP_PID_PWM = 0;
    State.TEMP_PID_DIR = 1;
    State.TEMP_PID_P_Error = 0;
    State.TEMP_PID_I_Error = 0;
    State.TEMP_PID_SetPoint = 35000000;
}

void TEMP_PID()
{

    State.TEMP = Temperature();
    if (State.TEMP > 40000000)
    {
        TIM1->CCR1 = 0;
        return;
    }

    State.TEMP_PID_P_Error = (State.TEMP - State.TEMP_PID_SetPoint) / 1000;
    State.TEMP_PID_I_Error = State.TEMP_PID_I_Error + State.TEMP_PID_P_Error;

    int sat = -(State.TEMP_PID_P_Error / 50 + State.TEMP_PID_I_Error / 400);

    State.TEMP_PID_DIR = sat > 0;

    sat = abs(sat);

    if (sat > 100)
    {
        sat = 100;
    }
    else if (sat < 0)
    {
        sat = 0;
    }
    State.TEMP_PID_PWM = sat;
    TIM1->CCR1 = State.TEMP_PID_PWM;

    HAL_GPIO_WritePin(PEL_DIR_GPIO_Port, PEL_DIR_Pin, State.TEMP_PID_DIR);

    if (abs(State.TEMP_PID_P_Error) < 200)
    {
        if (State.TEMP_PID_LOCK_count < 10)
        {
            State.TEMP_PID_LOCK_count++;
            State.TEMP_PID_LOCK = 0;
        }
        else
        {
            if (State.TEMP_PID_LOCK == 0)
            {
                SERIAL_WRITE_GREEN("Temperature locked\n");
            }
            State.TEMP_PID_LOCK = 1;
        }
    }
    else
    {
        State.TEMP_PID_LOCK_count = 0;
        if (State.TEMP_PID_LOCK == 1)
        {
            SERIAL_WRITE_RED("Temperature unlocked\n");
        }
        State.TEMP_PID_LOCK = 0;
    }
}
