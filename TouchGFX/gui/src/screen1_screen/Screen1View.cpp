#include <gui/screen1_screen/Screen1View.hpp>

extern char msg_udp[128];
extern int msg_udp_ready;
extern uint8_t uart_recv;
uint8_t old_uart_recv;
extern uint8_t udp_send_trigger;

extern uint8_t uart_send;
extern uint8_t uart_send_trigger;

Screen1View::Screen1View()
{

}

void Screen1View::setupScreen()
{
    Screen1ViewBase::setupScreen();
}

void Screen1View::tearDownScreen()
{
    Screen1ViewBase::tearDownScreen();
}

void Screen1View::interval_update()
{
	if(msg_udp_ready)
	{
		msg_udp_ready = 0;
		for(int i = 0; i < TEXTAREAUDP_SIZE; i++)
		{
			textAreaUDPBuffer[i] = msg_udp[i];
		}
		textAreaUDP.invalidate();
	}

	if(uart_recv != old_uart_recv)
	{
		old_uart_recv= uart_recv;
		textAreaUARTBuffer[0] = uart_recv;
		textAreaUARTBuffer[1] = 0;	// end line

		textAreaUART.invalidate();
	}
}

void Screen1View::btn_send_uart_click()
{
	uart_send = 'a';
	uart_send_trigger = 1;
}

void Screen1View::btn_udp_send_click()
{
	udp_send_trigger = 1;
}
