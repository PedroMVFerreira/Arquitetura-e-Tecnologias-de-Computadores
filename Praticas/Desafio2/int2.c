/*
Autores: Pedro Vale Ferreira (a95699) | Davide Oliveira Peixoto (a93949)
Data: 19 de outubro de 2022 | 17.13 
Versão: 2.4
Desafio 2
Turno: PL2
Docente: Rui Machado 
__________________________________________________________________________________________________________________
Implementaão da maquina de estados referente ao exercicio 
*/
/*Definição dos estados*/
#define DISPLAY 0
#define INC 1
#define DEC 2
/*Fim da definição dos estados*/

#include <c8051f380.h>
void main(void)
{
	/*Faz disahble do watchdog*/
	PCA0MD = 0x00;
	/*Ativa o crossbar*/
	XBR1 = 0x40;
	
	/*Declaração de variaveis*/
	unsigned char digitsOnP2[] = {0xC0/*0*/, 0xF9/*1*/, 0xA4/*2*/, 0xB0/*3*/, 0x99/*4*/, 0x92/*5*/, 0x82/*6*/, 
	0xF8/*7*/, 0x80/*8*/, 0x98/*9*/, 0x88/*A*/, 0x83/*b*/, 0xC6/*C*/, 0xA1/*d*/, 0x86/*E*/, 0x8E/*F*/};
	unsigned char index = 0;
	unsigned char state = DISPLAY;
	/*Fim da declaração das variavies*/
	
	while(1){
		switch(state){
			case DISPLAY:
				P2 = digitsOnP2[index & 0x0F];
				if(P0_6 == 0){
					while(P0_6 == 0);
					state = INC;
				}
				if(P0_7 == 0){
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