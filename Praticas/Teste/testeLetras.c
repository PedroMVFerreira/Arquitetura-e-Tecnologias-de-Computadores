#include <c8051f380.h>

void main(void)
{
	/* Disable do watchdog */		
	PCA0MD = 0x00;		

	/* Port/Crossbar init */
	XBR1 = 0x40;
	while(1){
		P2 =  0x82;
	}
}