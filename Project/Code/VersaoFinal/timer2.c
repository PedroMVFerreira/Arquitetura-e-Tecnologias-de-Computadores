//Este ficheiro .c conterá a rotina de atendimento à interrupção gerada pelo timer2 bem como ...


#include <c8051f380.h>
#include "globalVariables.h"

//Rotina de atendimento à interrupção gerada pelas flags do timer 2
void isr_TIMER2 (void) __interrupt (5)
{
	//As flags do timer 2 apenas são limpas por software (devido às configurações anteriores apenas a flag TF2H gerá interrupções)
	TF2H = 0; //Limpeza da flag, responsavel pela interrupção por software, TF2H
	contFlagT2++; //Incremento da variavel auxiliar responsavel por contar o numero de transbordos em uma unidade
}


//Responsavel por inicializar a contagem de tempo comandada pelo timer2 
void timer2Start (void)
{
	TR2 = 1; 
}
