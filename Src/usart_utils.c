#include "usart.h"
#include <stdio.h>
#include "state.h"
#include "usart_utils.h"

extern state State;
extern uint8_t UART_RX_BUFF[UART_RX_BUFFER_LENGTH];
extern uint8_t COMMAND;

void UART_PARSE(uint8_t buffer[UART_RX_BUFFER_LENGTH])
{
    SERIAL_WRITE("[UART_PARSE]: ");
    SERIAL_WRITE(buffer);
    SERIAL_WRITE(" => ");
    if (strcmp(buffer, "blink") == 0)
    {
        SERIAL_WRITE("ok");
        COMMAND = cmd_blink;
    }
    else if (strcmp(buffer, "adc id") == 0)
    {
        COMMAND = cmd_adc_id;
    }
    else if (strcmp(buffer, "adc reset") == 0)
    {
        COMMAND = cmd_adc_reset;
    }
    else
    {
        SERIAL_WRITE("undefined\n");
    }
}