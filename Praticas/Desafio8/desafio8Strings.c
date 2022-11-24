/*														    \
Autores: Pedro Vale Ferreira (a95699)                                       					    \	 
Data: 23 de novembro de 2022 											    \
VersÃ£o: 1.0													    \
Desafio 7 - Resolução depois da aula										    \
Turno: PL2													    \
Docente: Rui Machado 												    \
________________________________________________________________________________________________________________\
Implementação da maquina de estados referente ao exercicio e junção com comunicação UART1, utilizando interrupções lendo string*/

#include <c8051f380.h> 

//Declarações de variaveis globais 
volatile unsigned char readString[16];
volatile unsigned char read_readPointer;	
volatile unsigned char read_writePointer;
volatile unsigned char writeString[16];
volatile unsigned char write_readPointer;
volatile unsigned char write_writePointer;

//MACROS
//SFR stands for special function register e p stands for position
#define SET(SFR, p) (SFR |= 1 << p) 
#define CLEAR(SFR, p) (SFR &= ~(1 << p))
#define TOOGLE(SFR, p) (SFR ^= 1 << p)
#define IS_SET(SFR, p) (SFR & 1 << p)

//Estados da maquina de estados
#define DISPLAY 0
#define INC 1
#define DEC 2

#define BUFSIZEMASK 0x0F
#define LSB 0x07

//Função de configuração
void config()
{
	PCA0MD = 0x00;  	//Desativar o watchdog
	
	//Crossbar configurations
	XBR1 |= 0x40;		//Ativa o crossbar
	XBR2 |= 0x01;		//Ativa a UART1 nos pinos do porto (TX1 em P0_0 e RX1 em P0_1)
	P0SKIP |= 0x0F;	//Re-route de TX1 e RX1 para os pinos P0_4 e P0_5 respetivamente
	
	//Baud rate generator (A UART1 utiliza um timer dedicado) 115200bps
	SBRLL1 |= 0xF9;	//Byte menos significativo do valor de reload
	SBRLH1 |= 0xFF;	//BYte mais significativo do valor de reload
	SCON1 |= 0x10;		//Ativa a receção da UART1
	SBCON1 |= 0x43;	//Byte menos significativo -> baud rate presecaler select definido para SYSCLK / 1 ||Byte mais significativo -> ativa o baud rate generator
	
	//Interrupções
	IE |= 0x80;		//Ativa cada interrupção de acordo com as configurações individuais
	EIE2 |= 0x02;		//Ativa as interrupções geradas pela UART1
	
	//Configuração de flags
	CLEAR(SCON1, 0);	//RI1 = 0
	SET(SCON1, 1);		//TI1 = 1
}

void isr_UART1 (void) __interrupt (16)
{
	if(IS_SET(SCON1, 0)){//Acabou de receber, flag de receção RI1 ativa
		CLEAR(SCON1, 0);//Limpa a flag de receção
		if(readString[read_readPointer] != '\n'){
			readString[read_readPointer] = SBUF1; //Recebe o byte
			read_readPointer++;
		}
	}
	if(IS_SET(SCON1, 1)){
		 CLEAR(SCON1, 1);//Limpeza da flag de envio
		 if(write_readPointer != write_writePointer){//Existe algo para enviar
		 	SBUF1 = writeString[write_readPointer];//Envio
		 	write_readPointer++;
		 }
	}
}

void enviaUART()
{
	while(());
}
