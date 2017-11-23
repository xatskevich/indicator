
#ifndef __MAIN_H
#define __MAIN_H

#include "stm32f0xx.h"                  // Device header
#include "stm32f0xx_rcc.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_can.h"
#include "stm32f0xx_misc.h"
#include "stm32f0xx_iwdg.h"
#include "stm32f0xx_adc.h"

#include "CAN.h"
#include "GPIO.h"
#include "rcc.h"
#include "timer.h"
#include "ADC.h"

#define koef_k 17400
#define koef_c 25600

#define min_address 0x08007800
#define max_address 0x08007804

void flash_erase(void);
void flash_prog_all(void);


extern int16_t level, min, max, koeff, koef_perc, analog_tot;
extern uint8_t lvl_out, can_ctrl, can_error, can_error_mask, morg, press, w_morg, tmr;
extern int8_t percent, analog_count;

extern CanTxMsg TxMessage;
extern GPIO_InitTypeDef GPIO_InitStructure;
extern NVIC_InitTypeDef NVIC_InitStructure;
extern CAN_InitTypeDef CAN_InitStructure;
extern CAN_FilterInitTypeDef CAN_FilterInitStructure;
extern ADC_InitTypeDef ADC_InitStructure;

#endif
