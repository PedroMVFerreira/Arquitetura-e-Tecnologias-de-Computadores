//Todas as configurações necessarias para o funcionamento correto do programa no micro-controlador serão aqui realizadas

//Header files
#include <c8051f380.h>
#include "macros.h"

void portPinsConfigs (void)
{
	XBR2 = 0x01;	 //Ativa a UART1 nos pinos do porto
	XBR1 = 0x40;	 //Ativa o crossbar
	P0SKIP = 0x0F; //Coloca TX1 em P0_4 e RX1 em PO_5
}

void interruptsConfig (void)
{
	EIE2 = 0x02;	//Ativa as interupções da UART1
	EA = 0xA0;	//Ativa as interrupções do timer 2 bem como cada interrupção indivualmente previamente configurada
}

void UART1_configs (void)
{
	//Configuração para 115200 bps sem receção
	SBRLL1 = 0xF9;	//Valor de reload -> byte menos significativo
	SBRLH1 = 0xFF;	//Valor de reload -> byte mais significativo
	SBCON1 = 0x43; //Byte menos significativo -> baud rate prescaler select (SYSCLK / 1) || Byte mais significativo -> ativa o baud rate generator
}

void timer2Configs (void)
{
	//Configuração para uma frequencia de 2MHz e um periodo respetivo de 0.5 us por transbordo
	//Declaração e inicialização de variaveis
	int reload = -50000; //Faltam 50000 ciclos para o transbordo
	CLKSEL |= 0x02;
	TMR2RLL = reload;	//Valor de reload -> byte menos significativo
	TMR2RLH = reload >> 8; //Valor de reload -> byte mais significativo 
	
}

void configs (void)
{
	PCA0MD = 0x00; //Desativa o watchdog
	portPinsConfigs();
	UART1_configs();
	timer2Configs();
	interruptsConfig();
}
