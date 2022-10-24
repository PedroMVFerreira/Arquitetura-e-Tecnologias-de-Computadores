/*
Autores: Pedro Vale Ferreira (a95699) | Davide Oliveira Peixoto (a93949)
Data: 18 de outubro de 2022 | 01.35
Trabalho: Desafio 3
Versão: 2.0
Turno: PL2
Docente: Rui Machado
______________________________________________________________________________________________________________
Versão que utiliza o timer 2 (16 bit com auto reload) utilizando polling
*/

#include <c8051f380.h>
//Variavel global que será utilizada para incrementar o numero de vezes que a flag foi ativada na rotina ISR 
unsigned char contFlag = 0;

void main(void)
{
	PCA0MD = 0x00;	//Desativa o watchdog
	XBR1 = 0x40;	//Ativa o crossbar 
	
	//Frequencia do OHF -> 48 MHz 
	CLKSEL |= 0x02; //Divide a frequencia do oscilador por 2 -> 24 MHz 
	CKCON |= 0x00;	//Permite que o valor do clock do timer seja posteriormente dividido de novo 
	
	T2SPLIT = 0;	//Define que iremos usar o timer2 no modo 16-bit com auto-reload 
	T2XCLK = 0;		/*Divide o relogio do sistema por 12 obtendo assim o relogio do timer com uma
	frquencia de 2 Mhz e um periodo de 0.5 us*/
	
	int reload = -50000; /*Faltam 50000 ciclos para 0xFFFF, em cada transbordo terão se passado 
	0.025 segundos*/
	
	//Inicialização dos registos de paragem 
	TMR2RLL = reload; //Passagem do bit menos significativo de reload
	TMR2RLH = reload >> 8; //Passagem do bit mais significativo de reload utilizando um shift de 8 bit
	
	TR2 = 1;	//Inicialização do timer 2 
	
	while(1){
		while(!TF2H);	//Enquanto a flag não for ativada ficamos a espera que a mesma o seja
		TF2H = 0;	/*Caso a flag tenha sido ativada contagem terminada iremos limpara a flag para 
		iniciar nova contagem*/
		contFlag++;	//Incrementar o numero de vezes que o transbordo ocorreu 
		if(contFlag == 40){	/*O numero de vezes que a flag transbordou é suficiente para se ter passado
		um segundo*/
			P2_7 = !P2_7;	/*Fazer toggle ao pino do porto*/
			contFlag = 0;	/*Voltar a iniciar o contador de transbordos totais para que mais um segundo
			se possa passar*/
		}
	}
}

