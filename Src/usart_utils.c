#include "usart.h"
#include <stdio.h>
#include "state.h"
#include "usart_utils.h"

extern state State;

void TX(char *s)
{
    HAL_UART_Transmit(&huart2, (uint8_t *)s, sizeof s, 100);
}

void TXState()
{
    char buff[100] = {0};
    sprintf(buff, "timeup: %i\n", State.timeup);
    HAL_UART_Transmit(&huart2, buff, sizeof buff, 100);
}