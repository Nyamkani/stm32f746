/*
 * api_debug.h
 *
 *  Created on: Jul 26, 2022
 *      Author: studio3s
 */

#ifndef MODULE_API_DEBUG_API_DEBUG_H_
#define MODULE_API_DEBUG_API_DEBUG_H_


#include <optical_sensor/pgv100.h>
#include <sensor_manager/sensor_manager.h>
#include <transmit_tools/transmit_tools.h>


enum uart_comm_callback
{
	loopback = 0,

};

//use this below functions
void Debug_Uart_Init();
void Dprintf(const char *fmt, ...); // custom printf() function
void debug_Command();


void DebugDrive();
void BTDebugDrive();


#endif /* MODULE_API_DEBUG_API_DEBUG_H_ */
