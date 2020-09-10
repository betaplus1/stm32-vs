#include "usart.h"
#include <stdio.h>
#include "state.h"
#include "usart_utils.h"
#include "ADC.h"
#include "spi.h"
#include "DAC.h"
extern state State;

void Filter() //Simple moving average filter for all 16 adc channels
{
    State.ADC_Updated = 0;  //reset flag
    uint32_t acc[16] = {0}; //Accumulator for moving average

    //FilterFifo:
    for (int y = FilterLength - 1; y > 0; y--) // enumerate through buffer
    {
        for (int x = 0; x < 16; x++) // enumerate through values
        {
            State.ADC_ValuesBuffer[y][x] = State.ADC_ValuesBuffer[y - 1][x]; //move by +1 index
            acc[x] = acc[x] + State.ADC_ValuesBuffer[y - 1][x];              //accumulate
        }
    }
    for (int x = 0; x < 16; x++) // enumerate through new values
    {
        State.ADC_ValuesBuffer[0][x] = State.ADC_Values[x];  //Insert new values to the front of the buffer
        acc[x] = (acc[x] + State.ADC_Values[x]);             //accumulate new value
        State.ADC_ValuesFiltered[x] = acc[x] / FilterLength; //calculate average
    }

    State.ADC_Filter_Iteration++;                                                                 //count filter passes to know
    State.ADC_Filter_Valid = State.ADC_Filter_Iteration > FilterLength || State.ADC_Filter_Valid; //when data is not corrupted by initial zeros

    //Output:                                                 //Uncomment to test
    // SERIAL_WRITE("Filtered:\n");                              //
    // for (int x = 0; x < 16; x++)                              //
    // {                                                         //
    // SERIAL_WRITE("%u\t", State.ADC_ValuesFiltered[x])     //
    // }                                                         //
    // SERIAL_WRITE("\nRaw:\n");                                 //
    // for (int x = 0; x < 16; x++)                              //
    // {                                                         //
    // SERIAL_WRITE("%u\t", State.ADC_Values[x])             //
    // }                                                         //
    // if (State.ADC_Filter_Valid)                               //
    // {                                                         //
    // SERIAL_WRITE("\n");                                   //
    // SERIAL_WRITE("\n");                                   //
    // }                                                         //
    // else                                                      //
    // {                                                         //
    // SERIAL_WRITE("\nFilter contains initial zeros!\n\n"); //
    // }                                                         //
}
