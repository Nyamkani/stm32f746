/*
 * udprtosserver.cpp
 *
 *  Created on: Aug 25, 2022
 *      Author: kss
 */


#include "ethernet/udp_rtos/udp_rtos.h"

static struct netconn *conn;
static struct netbuf *buf, *txbuf, *rxbuf;
static ip_addr_t *addr;
static unsigned short port;
static char msg[100];
static char smsg[200];

//static int indx = 0;

#define ServerPort (int) 7
#define ClientPort (int) 8

/*-------------------------------------Server----------------------------------------*/
/*-----------------------------------------------------------------------------------*/
/**** Send RESPONSE every time the client sends some data ******/
static void UDPServerThread(void *arg)
{
	err_t err, recv_err;

	/* Create a new connection identifier */
	conn = netconn_new(NETCONN_UDP);

	if (conn!= NULL)
	{
		/* Bind connection to the port 8 */
		err = netconn_bind(conn, IP_ADDR_ANY, ServerPort);

		if (err == ERR_OK)
		{
			/* The while loop will run everytime this Task is executed */
			while (1)
			{
				/* Receive the data from the connection */
				recv_err = netconn_recv(conn, &buf);

				if (recv_err == ERR_OK) // if the data is received
				{
					addr = netbuf_fromaddr(buf);  // get the address of the client
					port = netbuf_fromport(buf);  // get the Port of the client
					strncpy (msg, (char* )buf->p->payload, buf->p->len);  // get the message from the client

					// Or modify the message received, so that we can send it back to the client
					int len = sprintf (smsg, "\"%s\" was sent by the Server\n", msg);

					/* allocate pbuf from RAM*/
					txbuf = netbuf_new();
					txbuf->p = pbuf_alloc(PBUF_TRANSPORT,len, PBUF_RAM);


					/* copy the data into the buffer  */
					pbuf_take(txbuf->p, smsg, len);

					netconn_sendto(conn, txbuf, addr, port);  // send the netbuf to the client

					buf->addr.addr = 0;  // clear the address
					netbuf_delete(txbuf);
					netbuf_delete(buf);  // delete the netbuf

					memset(msg, '\0', sizeof(msg));
					memset(msg, '\0', sizeof(smsg));
				}
			}
		}
		else
		{
			netconn_delete(conn);
		}
	}
}


void UdpServerInit(void)
{
	sys_thread_new("UDPServerThread", UDPServerThread, NULL, DEFAULT_THREAD_STACKSIZE, osPriorityNormal);
}

/*------------------------------------Client-----------------------------------------*/
/*-----------------------------------------------------------------------------------*/

/*-------UDP Send function to send the data to the server-------------*/
void udpsend (char *data)
{
	buf = netbuf_new();   // Create a new netbuf
	netbuf_ref(buf, data, strlen(data));  // refer the netbuf to the data to be sent
	netconn_send(conn,buf);  // send the netbuf to the client
	netbuf_delete(buf);  // delete the netbuf
}



static void UdpClientThread(void *arg)
{
	err_t err, recv_err;
	ip_addr_t dest_addr;
	/* Create a new connection identifier */
	conn = netconn_new(NETCONN_UDP);

	if (conn!= NULL)
	{
		/* Bind connection to the port 7 */
		err = netconn_bind(conn, IP_ADDR_ANY, ClientPort);
		if (err == ERR_OK)
		{
			/* The desination IP adress of the computer */
			IP_ADDR4(&dest_addr, 192, 168, 1, 25);
			/* connect to the destination (server) at port 8 */
			err = netconn_connect(conn, &dest_addr, ServerPort);
			if (err == ERR_OK)
			{
				/* Got into infinite loop after initialisation */
				for (;;)
				{
						/* Receive data from the server */
						recv_err = netconn_recv(conn, &rxbuf);
						if (recv_err == ERR_OK)
						{
							strncpy (msg, (char *) rxbuf->p->payload, rxbuf->p->len);  // get the message from the client
							/* copy the data into our buffer (smsg) */
							sprintf (smsg, "\"%s\" was sent by the Client\n", msg);
							udpsend(smsg);  // send the message to the server
							netbuf_delete(rxbuf);  // delete the netbuf
							memset(msg, '\0', sizeof(msg));
							memset(msg, '\0', sizeof(smsg));
						}
				}
			}
		}
	}

	else
	{
		netconn_delete(conn);
	}
}


/* UDP send Thread will send data every 500ms */
/*
static void udpsend_thread(void *arg)
{
	for (;;)
	{
		sprintf (smsg, "index value = %d\n", indx++);
		udpsend(smsg);
		osDelay(500);
	}
}
*/

void UdpClientInit(void)
{
	//sys_thread_new("udpsend_thread", udpsend_thread, NULL, DEFAULT_THREAD_STACKSIZE,osPriorityNormal);
	sys_thread_new("UdpClientThread", UdpClientThread, NULL, DEFAULT_THREAD_STACKSIZE,osPriorityNormal);
}



