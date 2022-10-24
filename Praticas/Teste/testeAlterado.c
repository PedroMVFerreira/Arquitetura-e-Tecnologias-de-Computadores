#include <c8051f380.h>

void main (void) {

	__bit bitVar;

	/* Disable do watchdog */		
	PCA0MD = 0x00;		

	/* Port/Crossbar init */
	XBR1 = 0x40;				
	
	
	while (1) {					
		bitVar = (!(P0_6) && !(P0_7));					

 		P2_7 = bitVar;
   }
}
