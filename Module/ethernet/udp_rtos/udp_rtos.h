/*
 * udprtosserver.h
 *
 *  Created on: Aug 25, 2022
 *      Author: kss
 */

#ifndef MODULE_ETHERNET_UDP_RTOS_UDP_RTOS_H_
#define MODULE_ETHERNET_UDP_RTOS_UDP_RTOS_H_

#include "cmsis_os.h"
#include "lwip/opt.h"
#include "lwip/api.h"
#include "lwip/sys.h"

#include "string.h"


void UdpServerInit(void);
void UdpClientInit(void);


#endif /* MODULE_ETHERNET_UDP_RTOS_UDP_RTOS_H_ */
