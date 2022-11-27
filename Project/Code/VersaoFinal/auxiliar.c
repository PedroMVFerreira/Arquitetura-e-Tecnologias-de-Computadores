//Header files
#include <c8051f380.h>
#include "macros.h"

//Variaveis globais declaração
unsigned int passSplit;
unsigned int splitValue;
unsigned int segs;
unsigned char contFlagT2;
unsigned char digitsARRAY[5];

void globalInits (void)
{
	//Declaração de variaveis locais 
	unsigned int index;
	
	//Declaração de variaveis globais 
	segs = 0; 
	contFlagT2 = 0;
	passSplit = 0;
	splitValue = 0;
	for(index = 0; index != 5; index++)
		digitsARRAY[index] = 0;
	
}


unsigned char digitsDivider (unsigned int tempo)
{
	//Declaração e inicialização de variaveis 
	int backup = tempo; //Utilizado para guardar o valor de segundos para manipulção 
	
	if(backup <= 9){ //Apenas um digito 
		digitsARRAY[1] = backup;
		return 1;
	}
	else if(backup >= 10 && backup <= 99){//Numeros com 2 digitos
		digitsARRAY[2] = backup % 10;
		backup = backup % 10;
		digitsARRAY[1] = backup % 10;
		return 2;
	}
	else if(backup >= 100 && backup <= 999){//Numeros com 3 digitos
		digitsARRAY[3] = backup % 10;
		backup = backup % 10;
		digitsARRAY[2] = backup % 10;
		backup = backup % 10;
		digitsARRAY[1] = backup % 10;
		return 3;
	}
	else if(backup >= 1000 && backup <= 9999){//Numeros com 4 digitos
		digitsARRAY[4] = backup % 10;
		backup = backup % 10;
		digitsARRAY[3] = backup % 10;
		backup = backup % 10;
		digitsARRAY[2] = backup % 10;
		backup = backup % 10;
		digitsARRAY[1] = backup % 10;
		return 4;
	}
	else if(backup >= 10000 && backup <= MAXTIME){//Numeros com 4 digitos
		digitsARRAY[5] = backup % 10;
		backup = backup % 10;
		digitsARRAY[4] = backup % 10;
		backup = backup % 10;
		digitsARRAY[3] = backup % 10;
		backup = backup % 10;
		digitsARRAY[2] = backup % 10;
		backup = backup % 10;
		digitsARRAY[1] = backup % 10;
		return 5;
	}
	return 0;
}

void showOnDisplay (unsigned char nOn)//nOn stans for number of Numbers (neste caso em particular o numero de numeros a serem mostrados no display) 
{
	//Declaração e inicialização de variaveis locais 
	__code unsigned char rep[10] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x98};
	
	
	switch(nOn){
		case 1:
			P2 = rep[digitsARRAY[1]];
		break;
		case 2:
			if(contFlagT2 <= 20)
				P2 = rep[digitsARRAY[1]];
			else if(contFlagT2 >= 20 && contFlagT2 <= 40)
				P2 = rep[digitsARRAY[2]];
		break;
		case 3:
			if(contFlagT2 <= 8)
				P2 = rep[digitsARRAY[1]];
			else if(contFlagT2 >= 9 && contFlagT2 <= 24)
				P2 = rep[digitsARRAY[2]];
			else if(contFlagT2 >= 25 && contFlagT2 <= 40)
				P2 = rep[digitsARRAY[3]];
		break;
		case 4:
			if(contFlagT2 <= 10)
				P2 = rep[digitsARRAY[1]];
			else if(contFlagT2 >= 11 && contFlagT2 <= 20)
				P2 = rep[digitsARRAY[2]];
			else if(contFlagT2 >= 21 && contFlagT2 <= 30)
				P2 = rep[digitsARRAY[3]];
			else if(contFlagT2 >= 31 && contFlagT2 <= 40)
				P2 = rep[digitsARRAY[4]];
		break;
		case 5:
			if(contFlagT2 <= 8)
				P2 = rep[digitsARRAY[1]];
			else if(contFlagT2 >= 9 && contFlagT2 <= 16)
				P2 = rep[digitsARRAY[2]];
			else if(contFlagT2 >= 17 && contFlagT2 <= 24)
				P2 = rep[digitsARRAY[3]];
			else if(contFlagT2 >= 25 && contFlagT2 <= 32)
				P2 = rep[digitsARRAY[4]];
			else if(contFlagT2 >= 33 && contFlagT2 <= 40)
				P2 = rep[digito5];
		break;
		case 0://Erro o display sera apagado
			P2 = 0xFF;
		break;
	}
}
