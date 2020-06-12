#ifndef __STATE
#define __STATE

typedef struct state
{
    int uptime;
    long int temperature;
    uint8_t ADC_Channel;
    uint32_t ADC_Values[16];
    uint8_t error;
    uint8_t cmd;
    uint8_t UART_RX_BUFF[UART_RX_BUFFER_LENGTH];
} state;

#endif /* __STATE */
