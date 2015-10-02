#include "uip.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static int handle_connection(struct simple_httpd_state *s);

#define CONNECT_PORT 23
#define LISTEN_PORT 80

static struct simple_httpd_state simplestate;

void flags()
{
	uart_putc('\n');
	if(uip_connected()) uart_puts_P("uip_connected");
	if(uip_rexmit()) uart_puts_P("uip_rexmit");
	if(uip_newdata()) uart_puts_P("uip_newdata()");
	if(uip_acked()) uart_puts_P("uip_acked()");
	if(uip_poll()) uart_puts_P("uip_poll()");
	if(uip_closed()) uart_puts_P("uip_closed()");
	if(uip_aborted()) uart_puts_P("uip_aborted()");
	if(uip_timedout()) uart_puts_P("uip_timedout()");

};


void simple_httpd_init(void)
{
	uip_ipaddr_t ipaddr;
	uip_ipaddr(&ipaddr, 198,168,122,21);


	if(uip_connect(&ipaddr,HTONS(CONNECT_PORT)) ==NULL)
		uart_puts_P("Cant allocate Conn\n");
	else 
		uart_puts_P("\nConnecting...\n");
	PSOCK_INIT(&simplestate.p, NULL, 0);

	uip_listen(HTONS(LISTEN_PORT));
}

void simple_httpd_appcall(void)
{
	struct simple_httpd_state *s = &(uip_conn->appstate);

	flags();

	if(uip_connected()) 
	{
		PSOCK_INIT(&s->p, NULL, 0);
		return;
	}

	if(uip_conn->lport == HTONS(LISTEN_PORT))
	{
		handle_connection(s);
	}
	else if(uip_conn->rport == HTONS(CONNECT_PORT))
	{
		uart_puts("-");

		if(uip_closed()||uip_aborted()||uip_timedout()) return;

		uip_send("Hello is there someone ?",25);

	}
	else 
		uart_puts("\n\tUnhandled Conn\n");
}

static int handle_connection(struct simple_httpd_state *s)
{
	PSOCK_BEGIN(&s->p);
	PSOCK_SEND_STR(&s->p, "HTTP/1.0 200 OK\r\n");
	PSOCK_SEND_STR(&s->p, "Content-Type: text/plain\r\n");
	PSOCK_SEND_STR(&s->p, "\r\n");
	PSOCK_SEND_STR(&s->p, "Hello World, From a simple httpd.");
	//PSOCK_SEND_STR(&s->p, "HTTP/1.1 200 OK\r\n Content-Type: text/plain\r\n\r\n Hello World !!!!");
	PSOCK_CLOSE(&s->p);
	PSOCK_END(&s->p);
}


void resolv_found(char *name, u16_t *ipaddr)
{
	uart_puts("R-Found\n");
}
