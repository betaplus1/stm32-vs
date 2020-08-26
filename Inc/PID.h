#ifndef __PID_H
#define __PID_H
#include "main.h"

void PID();
void PID_Init();
void PID352();
void PID704();
void POWER_PID352();
void POWER_PID704();

void Calib();
void Calib352();
void Calib704();
void POWER_Calib352();
void POWER_Calib704();

uint64_t Phase(uint8_t);
uint64_t Phase_u(uint8_t);
int64_t Relative_Power(uint8_t);
int64_t RMS_Power(uint8_t);
int64_t Temperature();
uint64_t Voltage(uint8_t);
uint64_t Voltage_nV(uint8_t);
#endif /* __PID_H */
