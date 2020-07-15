#ifndef __STATE
#define __STATE

#define FilterLength 16
typedef struct state
{
    int uptime;
    long int temperature;
    uint8_t ADC_Channel;
    uint8_t ADC_Updated;
    uint32_t ADC_Values[16];
    uint32_t ADC_ValuesBuffer[FilterLength][16];
    uint32_t ADC_ValuesFiltered[16];
    uint8_t ADC_Filter_Iteration;
    uint8_t ADC_Filter_Valid;
    uint8_t error;
    uint8_t cmd;
    uint8_t UART_RX_BUFF[UART_RX_BUFFER_LENGTH];
} state;

#endif /* __STATE */
