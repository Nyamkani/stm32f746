/*
 * api_init.cpp
 *
 *  Created on: Jul 5, 2022
 *      Author: studio3s
 */

#ifndef INC_API_INIT_H__
#define INC_API_INIT_H_

#pragma once

#include "main.h"
#include "stm32f7xx_hal.h"
#include <string.h>



#ifdef __cplusplus
extern "C" {
#endif





/* Private function prototypes -----------------------------------------------*/
void init_stm32f746();


void MPU_Config(void);
void SystemClock_Config(void);
void MX_GPIO_Init(void);
void MX_CAN1_Init(void);
void MX_FMC_Init(void);
void MX_UART4_Init(void);
void MX_USART1_UART_Init(void);
void MX_USART3_UART_Init(void);
void MX_USART6_UART_Init(void);
void MX_UART5_Init(void);
void MX_ETH_Init(void);
void MX_TIM7_Init(void);






#ifdef __cplusplus
}
#endif



#endif /* INC_API_INIT_CPP_ */
