#ifndef __USART_UTILS_H
#define __USART_UTILS_H

#define RESET "\r<<<<<<<<<<RESET>>>>>>>>\n"

#define cmd_undefined 255
#define cmd_blink 1
#define cmd_adc_id 2
#define cmd_adc_reset 3
#define cmd_adc_debug 4
#define cmd_adc_data 5
#define cmd_adc_values 6
#define cmd_temperature 7
#define cmd_dac_test 8

void UART_PARSE(uint8_t buffer[UART_RX_BUFFER_LENGTH]);
#endif /* __USART_UTILS_H */