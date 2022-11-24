/*														    \
Autores: Pedro Vale Ferreira (a95699)                                       					    \	 
Data: 23 de novembro de 2022 											    \
VersÃ£o: 1.0													    \
Desafio 7 - Resolução depois da aula										    \
Turno: PL2													    \
Docente: Rui Machado 												    \
________________________________________________________________________________________________________________\
Implementação da maquina de estados referente ao exercicio e junção com comunicação UART0, utilizando interrupções*/

#include <c8051f380.h> 

//Declarações de variaveis globais 
volatile unsigned char TxDado;
volatile unsigned char RxDado; 
volatile unsigned char data; 

//Função de configuração 
void config()
{
	PCAOMD = 0x00;  //Desativar o watchdog
	XBR1 |= 0x40;	//Ativa o crossbar
	XBR0 |= 0x01;	//Ativa a UART0 no croosbar
	REN0 = 1; 	//Habilita a receção da UART
	
	//Interrupções 
	ES0 = 1;	//Ativação da interrupção da UART0
	EA = 1;	//Atuvação de todas as interrupções configuradas anteriormente
	
	//Baud Rate Generator -> configurado para 115200 (A UART0 utiliza o timer1 no modo 0)
	TMOD |= 0x20; 
	CLKSEL |= 0x02;
	CKCON |= 0x08;
	TH1 |= 0x98;
	
	//Push pull não faz diferença (deveremos apenas ativar para obtermos maior currente e por exemplo um led mais brilhante) 
	P0MDOUT |= 0x10;
}

void isr_UART0 (void) __interrupt(4)
{
	if(RI0){//Acabou de receber
		RI0 = 0; //Limpa a flag de receção 
		RxDado = 1; 
		dado = SBUF0; //Recebe o byte
	}
	if(TI0){
		TI0 = 0; //Limpeza da flag de envio 
		if(TxDado){//Existe algo para enviar
			TxDado = 0; //A enviar
			SBUF0 = dado;	//Envio
		}
		else
			TxDado = 1;
	}
}

void enviaUART(unsigned char x)
{
	while(!TxDado);
	dado = (x+48);	//O 0 na tabela ascci corresponde a 0 dai incrementarmos 48 ao numero que queremos enviar, para enviarmos o seu codigo correspondente
	TxDado = 1;
	TI0 = 1; 	//Chamada da ISR
}

unsigned char recebeUART(void)
{
	unsigned char x;
	if(RxDado){ //Algo foi recebido
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
		enviaUART(d1);
	}
	else(
		d2 = x % 10;
		x = x / 10;
		d1 = x % 10;
		enviaUART(d1);
		enviaUART(d2);
	}
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



/*Receção de strings 
Precisamos de um array global; um apontador de caracteres lidos 


Buffer circular - o tamanho do buffer tem de ser 2^n*/
