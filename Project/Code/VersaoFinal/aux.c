//Header files
#include <c8051f380.h>

void globalInits (void)
{
	segs = 0; 
	contFlagT2 = 0;
	passSplit = 0;
	splitValue = 0;
}
