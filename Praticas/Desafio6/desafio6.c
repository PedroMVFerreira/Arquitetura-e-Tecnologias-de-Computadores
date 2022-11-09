/*
Autores: Pedro Vale Ferreira (a95699) | Davide Oliveira Peixoto (a93949)
Data: 9 de novembro de 2022 | 17.13 
Versão: 0.1
Desafio 6
Turno: PL2
Docente: Rui Machado 
__________________________________________________________________________________________________________________
Implementaão da maquina de estados referente ao exercicio e comunicação UART
!!!Carece de teste 
*/
/*Definição dos estados*/
#define DISPLAY 0
#define INC 1
#define DEC 2
/*Fim da definição dos estados*/

/*Prototipos*/
void generalConfigs();
void timer1Configs();
void portaSerieConfigs();
unsigned char recebeUART() ;
/*Fim de Prototipos*/

#include <c8051f380.h>
void main(void)
{

    generalConfigs();/*Configurações gerais do micro controlador*/
    timer1Configs();/*Configurações do timer 1*/
    portaSerieConfigs();/*Configurações da porta serie*/

	/*Declaração de variaveis*/
	__code unsigned char digitsOnP2[] = {0xC0/*0*/, 0xF9/*1*/, 0xA4/*2*/, 0xB0/*3*/, 0x99/*4*/, 0x92/*5*/, 0x82/*6*/, 
	0xF8/*7*/, 0x80/*8*/, 0x98/*9*/, 0x88/*A*/, 0x83/*b*/, 0xC6/*C*/, 0xA1/*d*/, 0x86/*E*/, 0x8E/*F*/};
	unsigned char index = 0;
	unsigned char state = DISPLAY;
    unsigned char letter;
	/*Fim da declaração das variavies*/
	
	while(1){
        letter = recebeUART(); /*Receção de 'i' ou 'I' para incremento ou receção de 'd' ou 'D' para decremento*/
		switch(state){
			case DISPLAY:
				P2 = digitsOnP2[index & 0x0F];
				if(P0_6 == 0 || letter == 'i' || letter == 'I'){
					while(P0_6 == 0);
					state = INC;
				}
				if(P0_7 == 0 || letter == 'd' || letter == 'D'){
					while(P0_7 == 0);
					state = DEC;
				}
			break;
			case INC:
				index++;
				state = DISPLAY;
			break;
			case DEC:
				index--;
				state = DISPLAY;
			break;
		}/*Fim do switch case*/
	}/*Fim do ciclo infinito*/
}

void generalConfigs()
{
    /*Faz disahble do watchdog*/
	PCA0MD = 0x00;
	/*Ativa o crossbar*/
	XBR1 = 0x40;
    /*Ativa UART0 no crossbar*/
    XBR0 |= 0x01;  
}

void timer1Configs()
{
    /*Iremos configurar o timer 1 para fornecer um baud rate (igual ao bite rate no nosso mircro)
    de 115200 - segundo a tabela 22.1 do datasheet*/
    CKCON |= 0X08; /*Equivalente a ter T1M = 1*/
    TH1 = 0x98; /*Valor de reload para 115200 e 9600 (devido a este fator se comentarmos a linha
    de cima obtemos um baud rate de 9600 bps em vez de 115200 bps)*/
    TMOD |= 0x20; /*Timer 1 no modo 2 (8bit com auto reload - obrigatorio)*/
    TR1 = 1; /*Ativação do timer 1*/
}

void portaSerieConfigs()
{
    TI0 = 1;    /*Tramiste interrupt flag - não sera usada neste exemplo*/
    RI0 = 0;    /*Receive interrupt flag*/
    REN0 = 1;   /*Ativa a receção*/
}

unsigned char recebeUART() 
{
	unsigned char x;
	while (!RI0);   /*Aguarda a receção do byte*/ 
	x = SBUF0;      /*Recebe o byte e le de SBBUF*/
	RI0=0;          /*limpa a flag de rececao*/ 
	return x;       /*Devolve o byte lido à main*/
}
