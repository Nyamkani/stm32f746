/*
 * tcprtosserver.h
 *
 *  Created on: Sep 6, 2022
 *      Author: kss
 */

#ifndef MODULE_ETHERNET_TCP_RTOS_TCP_RTOS_H_
#define MODULE_ETHERNET_TCP_RTOS_TCP_RTOS_H_

#include "lwip/opt.h"
#include "lwip/api.h"
#include "lwip/sys.h"

#include "string.h"
#include <vector>

void TcpServerInit(void);
void TcpClientInit(void);

#endif /* MODULE_ETHERNET_TCP_RTOS_TCP_RTOS_H_ */
