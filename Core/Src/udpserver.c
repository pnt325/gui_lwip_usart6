/*
 * udpserver.c
 *
 *  Created on: Dec 18, 2022
 *      Author: phatn
 */

#include "cmsis_os.h"
#include "lwip/opt.h"

#include "lwip/api.h"
#include "lwip/sys.h"

#include "udpserver.h"
#include "string.h"

#define UDP_SERVER_PORT	7000

static struct netconn *conn;
static struct netbuf *buf;
static ip_addr_t addr;
static unsigned short port;
char msg[128];
char smsg[200];

char msg_udp[128];
int msg_udp_ready = 0;

/*-----------------------------------------------------------------------------------*/
/**** Send RESPONSE every time the client sends some data ******/
static void udp_thread(void *arg)
{
	err_t err, recv_err;
	struct pbuf *txBuf;

	/* Create a new connection identifier */
	conn = netconn_new(NETCONN_UDP);


	if (conn!= NULL)
	{
		/* Bind connection to the port 7 */
		err = netconn_bind(conn, IP_ADDR_ANY, UDP_SERVER_PORT);

		if (err == ERR_OK)
		{
			/* The while loop will run everytime this Task is executed */
			while (1)
			{
				/* Receive the data from the connection */
				recv_err = netconn_recv(conn, &buf);

				if (recv_err == ERR_OK) // if the data is received
				{
					addr = *netbuf_fromaddr(buf);  // get the address of the client
					port = netbuf_fromport(buf);  // get the Port of the client
					strcpy (msg, buf->p->payload);   // get the message from the client

					for(int i = 0; i < 32; i++)
					{
						msg_udp[i] = msg[i];
					}
					msg_udp_ready = 1;

					//osMessageQueuePut(send_queueHandle, msg,0, 1000);

					// Or modify the message received, so that we can send it back to the client
					int len = sprintf (smsg, "Received:\"%s\"\r\n", (char *) buf->p->payload);

					/* allocate pbuf from RAM*/
					txBuf = pbuf_alloc(PBUF_TRANSPORT,len, PBUF_RAM);

					/* copy the data into the buffer  */
					pbuf_take(txBuf, smsg, len);

					// refer the nebuf->pbuf to our pbuf
					buf->p = txBuf;

					netconn_connect(conn, &addr, port);  // connect to the destination address and port

					netconn_send(conn,buf);  // send the netbuf to the client

					buf->addr.addr = 0;  // clear the address
					pbuf_free(txBuf);   // clear the pbuf
					netbuf_delete(buf);  // delete the netbuf
				}
			}
		}
		else
		{
			netconn_delete(conn);
		}
	}
}


void udpserver_init(void)
{
  sys_thread_new("udp_thread", udp_thread, NULL, DEFAULT_THREAD_STACKSIZE,osPriorityNormal);
}

void udpsend(const char* msg)
{
    struct netbuf *_buf;
    ip_addr_t _addr;
    char text[] = "Hello world\r\n";

	_buf = netbuf_new();   					// Create a new netbuf
	netbuf_ref(_buf, text, strlen(text));  	// refer the netbuf to the data to be sent
	netconn_send(conn,_buf);  				// send the netbuf to the client
	netbuf_delete(_buf);  					// delete the netbuf
}
