/*														    \
Autores: Pedro Vale Ferreira (a95699)                                       					    \	 
Data: 23 de novembro de 2022 											    \
VersÃ£o: 1.0													    \
Desafio 7 - Resolução depois da aula										    \
Turno: PL2													    \
Docente: Rui Machado 												    \
________________________________________________________________________________________________________________\
Implementação da maquina de estados referente ao exercicio e junção com comunicação UART1, utilizando interrupções lendo apenas um caracter*/

#include <c8051f380.h> 

//Declarações de variaveis globais 
volatile unsigned char TxDado;
volatile unsigned char RxDado; 
volatile unsigned char data; 

//MACROS
//SFR stands for special function register e p stands for position
#define SET(SFR, p) (SFR |= 1 << p) 
#define CLEAR(SFR, p) (SFR &= ~(1 << p))
#define TOOGLE(SFR, p) (SFR ^= 1 << p)
#define IS_SET(SFR, p) (SFR & 1 << p)


//Função de configuração
void config()
{
	PCAOMD = 0x00;  	//Desativar o watchdog
	
	//Crossbar configurations
	XBR1 |= 0x40;		//Ativa o crossbar
	XBR2 |= 0x01;		//Ativa a UART1 nos pinos do porto (TX1 em P0_0 e RX1 em P0_1)
	P0SKIP |= 0x007;	//Re-route de TX1 e RX1 para os pinos P0_4 e P0_5 respetivamente
	
	//Baud rate generator (A UART1 utiliza um timer dedicado) 115200bps
	SBRLL1 |= 0xF9;		//Byte menos significativo do valor de reload
	SBRLH1 |= 0xFF;		//BYte mais significativo do valor de reload
	SCON1 |= 0x10;			//Ativa a receção da UART1
	SBCON1 |= 0x43;		//Byte menos significativo -> baud rate presecaler select definido para SYSCLK / 1 ||Byte mais significativo -> ativa o baud rate generator
	
	//Interrupções
	IE |= 0x80;			//Ativa cada interrupção de acordo com as configurações individuais
	EIE2 |= 0x02;			//Ativa as interrupções geradas pela UART1
	
	//Configuração de flags
	CLEAR(SCON1, 0);		//RI1 = 0
	SET(SCON1, 1);			//TI1 = 1
}

void isr_UART1 (void) __interrupt (16)
{
	if(IS_SET(SCON1, 0)){//Acabou de receber, flag de receção RI1 ativa
		CLEAR(SCON1, 0);//Limpa a flag de receção
		RxDado = 1;
		dado = SBUF1; //Recebe o byte
	}
	if(IS_SET(SCON1, 1)){
		 CLEAR(SCON1, 1);//Limpeza da flag de envio
		 if(TxDado){//Existe algo para enviar
		 	TxDado = 0;//A enviar
		 	SBUF1 = dado;//Envio
		 }
		 else
		 	TxDado = 1;
	}
}

void enviaUART(unsigned char x)
{
	while(!TxDado);
	dado = (x);	//O 0 na tabela ascci corresponde a 0 dai incrementarmos 48 ao numero que queremos enviar, para enviarmos o seu codigo correspondente
	TxDado = 1;
	SET(SCON1, 1);	//TI1 = 1 -> Chamada da ISR
}

unsigned char recebeUART(void)
{
	unsigned char x;
	if(RxDado){//Algo foi recebido
		x = dado;
		RxDado = 0;
	}
	return x;
}

void digitConfiguration(signed char x)
{
	unsigned char d1, d2; //d1 guardara o digito mais significativo ou digito unico e d2 o digito menos significativo
	if(x <= 9){
		d1 = (unsigned) x;
		enviaUART(d1 + 48);
	}
	else(
		d2 = x % 10;
		x = x / 10;
		d1 = x % 10;
		enviaUART(d1 + 48);
		enviaUART(d2 + 48);
	}
}

void errorMessage()
{
	enviaUART('E');
	enviaUART('R');
	enviaUART('R');
	enviaUART('O');
	enviaUART('\r');
	enviaUART('\n');
}

void main (void)
{
	//Exatamente igual ao desafio da semana anterior (Desafio 6)
	config();	//Configurações
	/*Declaração e inicialização de variaveis*/
	__code unsigned char digitsOnP2[] = {0xC0/*0*/, 0xF9/*1*/, 0xA4/*2*/, 0xB0/*3*/, 0x99/*4*/, 0x92/*5*/, 0x82/*6*/, 
	0xF8/*7*/, 0x80/*8*/, 0x98/*9*/, 0x88/*A*/, 0x83/*b*/, 0xC6/*C*/, 0xA1/*d*/, 0x86/*E*/, 0x8E/*F*/};
	index = 0;
	unsigned char state = DISPLAY;
	signed char index = 0;
	/*Fim da declaraÃ§Ã£o das variavies*/
	
	while(1){
		switch(state){
			case DISPLAY:
				P2 = digitsOnP2[index & 0x0F];
				if(RI0 == 1)
					recebeUART();
				if(P0_6 == 0){
					while(P0_6 == 0);
					state = INC;
				}
				if(P0_7 == 0){
					while(P0_7 == 0);
					state = DEC;
				}
				if(letter == 'i' || letter == 'I')
					state = INC;
				else if(letter == 'd' || letter == 'D')
					state = DEC;
				else
					errorMessage();
			break;
			case INC:
				index++;
				if(index > 15)/*Utilizou se o if e não a mascara por causa do envio do valor de indice*/
					index = 0;
				digitConfiguration(index);
				state = DISPLAY;
			break;
			case DEC:
				index--;
					if(index < 0)/*Utilizou se o if e não a mascara por causa do envio do valor de indice*/
						index = 15;
				digitConfiguration(index);
				state = DISPLAY;
			break;
		}/*Fim do switch case*/
	}/*Fim do ciclo infinito*/
}
