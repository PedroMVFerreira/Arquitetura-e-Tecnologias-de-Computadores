/*														    \
Autores: Pedro Vale Ferreira (a95699) | Davide Oliveira Peixoto (a93949)                                        \ 
Data: 09 de novembro de 2022 											    \
VersÃ£o: 0.4													    \
Desafio 2													    \
Turno: PL2													    \
Docente: Rui Machado 												    \
________________________________________________________________________________________________________________\
ImplementaÃ£o da maquina de estados referente ao exercicio e junção com comunicação UART
*/
/*DefiniÃ§Ã£o dos estados*/
#define DISPLAY 0
#define INC 1
#define DEC 2
/*Fim da definiÃ§Ã£o dos estados*/

/*Variaveis globais*/
signed char index;
unsigned char letter;

/*Prototipos*/
void generalConfigs();
void timerConfigsForUART0();
void portaSerieConfigs();
void receiveUART();
void sendUART();
/*Fim de prototipos*/

#include <c8051f380.h>
void main(void)
{
	generalConfigs();
	timerConfigsForUART0();
	portaSerieConfigs();
	/*Declaração e inicialização de variaveis*/
	__code unsigned char digitsOnP2[] = {0xC0/*0*/, 0xF9/*1*/, 0xA4/*2*/, 0xB0/*3*/, 0x99/*4*/, 0x92/*5*/, 0x82/*6*/, 
	0xF8/*7*/, 0x80/*8*/, 0x98/*9*/, 0x88/*A*/, 0x83/*b*/, 0xC6/*C*/, 0xA1/*d*/, 0x86/*E*/, 0x8E/*F*/};
	index = 0;
	unsigned char state = DISPLAY;
	/*Fim da declaraÃ§Ã£o das variavies*/
	
	while(1){
		switch(state){
			case DISPLAY:
				P2 = digitsOnP2[index & 0x0F];
				if(RI0 == 1)
					receiveUART();
				if(P0_6 == 0){
					while(P0_6 == 0);
					state = INC;
				}
				if(letter == 'i' || letter == 'I')
					state = INC;
				if(P0_7 == 0){
					while(P0_7 == 0);
					state = DEC;
				}
				if(letter == 'd' || letter == 'D')
					state = DEC;
			break;
			case INC:
				index++;
				if(index > 15)/*Utilizou se o if e não a mascara por causa do envio do valor de indice*/
					index = 0;
				sendUART(index);
				state = DISPLAY;
			break;
			case DEC:
				index--;
					if(index < 0)/*Utilizou se o if e não a mascara por causa do envio do valor de indice*/
						index = 15;
				sendUART();
				state = DISPLAY;
			break;
		}/*Fim do switch case*/
	}/*Fim do ciclo infinito*/
}

void generalConfigs()
{
	/*Faz disahble do watchdog*/
	PCA0MD = 0x00;
	/*Ativa o crossbar*/
	XBR1 = 0x40;
	/*Ativa UART0 no crossbar*/
	XBR0 |= 0x01;
}

void timerConfigsForUART0()
{
	/*Iremos configurar o timer 1 responsavel pela alimentação da UART 0 para forncere um baud rate (igual ao bit 
	rate no nosso micro) de 115200*/
	CKCON |= 0x08; /*Equivalente a ter T1M  = 1*/
	TH1 = 0x98; /*Valor de reload para 115200 (o valor de reload para 9600 é igual, logo poderemos comentar a linha
	de codigo referente ao CKCON e obtemos um baud rate de 9600*/
	TMOD |= 0x20; /*Modo 2 (8 bit com auto reload) obrigatorio no uso da UART0*/
	TR1 = 1; /*Ativiação do timer 1*/
}

void portaSerieConfigs()
{
	TI0 = 1; /*Transmite interrupt flag, colocada a 1 para assinalar que estamos prontos para um envio */
	RI0 = 0; /*Recive interrupt flag*/
	REN0 = 1; /*Ativa a receção (por defeito desativada)*/
}

void receiveUART()
{
	letter = SBUF0; /*Le o byte recebido no fim da comunicação guardado temporariamente no buffer da UART0*/
	RI0 = 0; /*Limpa a flag de receção para uma nova receção*/ 
}

void sendUART()
{
	/*Visto apenas enviarmos valores entre 0 e 15 um byte é mais do que suficiente para envio dos nossos dados*/
	while(!TI0); /*Aguarada pelo envio anterior*/
	TI0 = 0; /*Limpa a flag de envio*/
	SBUF0 = index; /*Envio do valor de index*/
}
