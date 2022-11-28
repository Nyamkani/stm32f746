/*
 * tcprtosserver.cpp
 *
 *  Created on: Sep 6, 2022
 *      Author: kss
 */
#include <ethernet/tcp_rtos/tcp_rtos.h>



static struct netconn *conn, *newconn;
static struct netbuf *buf;
static char msg[100];  //receiving msg buffer
static char smsg[200];	//to send temp buffer

static ip_addr_t *addr, dest_addr;
static unsigned short port, dest_port;

#define ServerPort (int) 10
#define ClientPort (int) 11

char char_ethernet[100];


//static int indx = 0;

/*------------------------------------Server-----------------------------------------*/
/*-----------------------------------------------------------------------------------*/


/**** Send RESPONSE every time the client sends some data ******/


static int TcpServerInitSeq()
{
	err_t err;

	/* Create a new connection identifier. */
	conn = netconn_new(NETCONN_TCP);

	if (conn == NULL)
	{
		netconn_delete(conn);
		return -1;
	}

	/* Bind connection to the port number 8. */
	err = netconn_bind(conn, IP_ADDR_ANY, ServerPort);

	if (err != ERR_OK) return -1;

	/* Tell connection to go into listening mode. */
	netconn_listen(conn);

	/*Successfully Initialized*/
	return 0;
}

static int TcpServerDriveSeq()
{
	err_t accept_err;

	/* Grab new connection. */
	accept_err = netconn_accept(conn, &newconn);

	/* Process the new connection. */
	if(accept_err == ERR_OK)
	{
		/* receive the data from the client */
		while (netconn_recv(newconn, &buf) == ERR_OK)
		{
			/* If there is some data remaining to be sent, the following process will continue */
			do
			{
				strncpy (msg, (char* )buf->p->payload, buf->p->len);   // get the message from the client

				//for test debug
				strncpy (char_ethernet, (char* )buf->p->payload, buf->p->len);   // get the message from the client

				//enter Queue and parsing
				/*


				TcpServerParsingSeq();


				*/
				// Or modify the message received, so that we can send it back to the client
				//response
				int len = sprintf (smsg, "\"%s\" was sent by the Client\n", msg);

				netconn_write(newconn, smsg, len, NETCONN_COPY);  // send the message back to the client
				memset (msg, '\0', 100);  // clear the buffer
			}
			while (netbuf_next(buf) >0);

			netbuf_delete(buf);
		}
		/* Close connection and discard connection identifier. */
		netconn_close(newconn);
		netconn_delete(newconn);
	}
	return 0;
}







static void TcpServerThread(void *arg)
{
	TcpServerInitSeq();

	TcpServerDriveSeq();

	//if out of TcpServerDriveSeq(); it means error occur
}



void TcpServerInit(void)
{
  sys_thread_new("TcpServerThread", TcpServerThread, NULL, DEFAULT_THREAD_STACKSIZE,osPriorityNormal);
}



/*------------------------------------Client-----------------------------------------*/
/*-----------------------------------------------------------------------------------*/

// tcpsem is the binary semaphore to prevent the access to tcpsend
//sys_sem_t tcpsem;

void tcpsend (char *data)
{
	// send the data to the connected connection
	netconn_write(conn, data, strlen(data), NETCONN_COPY);
	// relaese the semaphore
	//sys_sem_signal(&tcpsem);
}

static void TcpClientThread(void *arg)
{
	err_t err, connect_error;

	/* Create a new connection identifier. */
	conn = netconn_new(NETCONN_TCP);

	if (conn!=NULL)
	{
		/* Bind connection to the port number 8(port of the Client). */
		err = netconn_bind(conn, IP_ADDR_ANY, ClientPort);

		if (err == ERR_OK)
		{
			/* The desination IP adress of the computer */
			IP_ADDR4(&dest_addr, 192, 168, 1, 24);
			dest_port = ServerPort;  // server port 10

			/* Connect to the TCP Server */
			connect_error = netconn_connect(conn, &dest_addr, dest_port);

			// If the connection to the server is established, the following will continue, else delete the connection
			if (connect_error == ERR_OK)
			{
				// Release the semaphore once the connection is successful
				//sys_sem_signal(&tcpsem);
				while (1)
				{
					/* wait until the data is sent by the server */
					if (netconn_recv(conn, &buf) == ERR_OK)
					{
						/* Extract the address and port in case they are required */
						addr = netbuf_fromaddr(buf);  // get the address of the client
						port = netbuf_fromport(buf);  // get the Port of the client

						/* If there is some data remaining to be sent, the following process will continue */
						do
						{

							strncpy (msg, (char*)buf->p->payload, buf->p->len);   // get the message from the server

							// Or modify the message received, so that we can send it back to the server
							sprintf (smsg, "\"%s\" was sent by the Server\n", msg);

							// semaphore must be taken before accessing the tcpsend function
							//sys_arch_sem_wait(&tcpsem, 500);

							// send the data to the TCP Server
							tcpsend(smsg);

							memset (msg, '\0', 100);  // clear the buffer
						}
						while (netbuf_next(buf) >0);

						netbuf_delete(buf);
					}
				}
			}

			else
			{
				/* Close connection and discard connection identifier. */
				netconn_close(conn);
				netconn_delete(conn);
			}
		}
		else
		{
			// if the binding wasn't successful, delete the netconn connection
			netconn_delete(conn);
		}
	}
}


/*
static void tcpsend_thread (void *arg)
{
	for (;;)
	{
		sprintf (smsg, "index value = %d\n", indx++);
		// semaphore must be taken before accessing the tcpsend function
		//sys_arch_sem_wait(&tcpsem, 500);
		// send the data to the server
		tcpsend(smsg);
		osDelay(500);
	}
}
*/

void TcpClientInit(void)
{
	//sys_sem_new(&tcpsem, 0);  // the semaphore would prevent simultaneous access to tcpsend
	sys_thread_new("TcpClientThread", TcpClientThread, NULL, DEFAULT_THREAD_STACKSIZE,osPriorityNormal);
	//sys_thread_new("tcpsend_thread", tcpsend_thread, NULL, DEFAULT_THREAD_STACKSIZE,osPriorityNormal);
}


