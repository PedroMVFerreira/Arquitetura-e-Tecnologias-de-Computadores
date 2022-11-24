//Todas as funções que serão usadas como função principal de um estado serão aqui colocadas

//Header files
#include <c8051f380.h>

//Variaveis globais
unsigned int passSplit;
unsigned int splitValue;

void idle (void)
{
	if(contFlagT2 == 40)
		contFlagT2 = 0;
	//Dividir segs por digito e mostrar esse valor no display 
}

void cont (void)
{
	if(contFlagT2 == 40){
		contFlag = 0;
		segs++;
	}
	//Dividir segs por digito e mostrar o seu valor no display
}

void split (void)
{
	//Declaração e inicialização de variaveis
	unsigned char contSplits = 0;
	unsigned int backsegs = segs;
	
	if(contSplits == 0)
		passSplit = 0;
	else 
		passSplit = splitValue;
	trueSplit = backsegs - passSplit;
	//Dividir 'trueSplit' e enviar o seu valor junto com mensagem de contador por comunicação UART
		
}

void reset (void)
{
	segs = 0; //Limpeza da variavel segs permitindo uma nova contagem
}

void end (void)
{
	//Envio de mensgem de tempo maximo atingido
	segs = 0;
}

