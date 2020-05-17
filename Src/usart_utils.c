#include "usart.h"
#include <stdio.h>
#include "state.h"
#include "usart_utils.h"

extern state State;
extern uint8_t UART_RX_BUFF[UART_RX_BUFFER_LENGTH];

void RX()
{
    SERIAL_WRITE("COMMAND:\n");
    SERIAL_WRITE(UART_RX_BUFF);
    SERIAL_WRITE("\n");
}