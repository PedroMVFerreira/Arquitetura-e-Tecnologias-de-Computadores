#include <c8051f380.h> 
#include "macros.h"
#include "globalVariables.h"


void isr_UART1 (void) __interrupt (16)
{
	if(IS_SET(SCON1, TI1){//Flag de envio ativada
		CLEAR(SCON1, TI1);
		if(!EMPTY(head, tail){
			SBUF1 = arrayDeEnvio[tail & ARRAYSIZE];
			tail++;
		}
	}
}

void enviaUART (unsigned char x)
{
	while(FULL(head, tail));
	arrayDeEnvio[head & ARRAYSIZE] = x;
	head++;
	SET(SCON1, TI1);	
}
