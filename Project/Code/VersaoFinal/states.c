//Todas as funções que serão usadas como função principal de um estado serão aqui colocadas

//Header files
#include <c8051f380.h>
#include "globalVariables.h"
#include "auxiliar.h"

void idle (void)
{
	//Declaração e inicialização de variaveis locais
	unsigned char numberOFdigits = 0;
	if(contFlagT2 == 40)
		contFlagT2 = 0;

	numberOFdigits = digitsDivider(segs);//Divisão de 'segs' numero nos seus diferentes digitos
	showOnDisplay(numberOFdigits);//Display desses diferentes digitos 
}

void cont (void)
{
	//Declaração de variaveis locais
	unsigned char numberOFdigits = 0;
	
	if(contFlagT2 == 40){
		contFlagT2 = 0;
		segs++;
	}
	
	numberOFdigits = digitsDivider(segs);//Divisão de 'segs' nos seus diferentes digitos
	showOnDisplay(numberOFdigits);//Display desses diferentes digitos 
}

void split (void)
{
	//Declaração e inicialização de variaveis locais
	unsigned char numberOFdigits = 0;
	unsigned char contSplits = 0;
	unsigned int backsegs = segs;
	
	if(contSplits == 0)
		passSplit = 0;
	else 
		passSplit = splitValue;
	splitValue = backsegs - passSplit;
	
	numberOFdigits = digitsDivider(splitValue);//Divisão do valor de 'trueSplit' nos seus diferentes digitos
	
	splitMessage(numberOFdigits); //Enviar o valor de split por comunicação UART
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

