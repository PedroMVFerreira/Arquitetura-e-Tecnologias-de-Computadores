#ifndef UART1_H
#define UART1_H

void isr_UART1 (void) __interrupt (16);
void enviaUART (unsigned char x);

#endif
