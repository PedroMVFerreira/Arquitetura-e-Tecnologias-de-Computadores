/*
Autores: Pedro Vale Ferreira (a95699) | Davide Oliveira Peixoto (a93949)
Data: 20 de novembro de 2022 
VersÃ£o: 1.0
Desafio 2
Turno: PL2
Docente: Rui Machado 
__________________________________________________________________________________________________________________
ImplementaÃ£o da maquina de estados referente ao exercicio e junção com comunicação UART (UART1) usando interrupções
*/
/*Header-files*/
#include <c8051f380.h>
/*Fim de header-files*/

/*DefiniÃ§Ã£o dos estados*/
#define DISPLAY 0
#define INC 1
#define DEC 2
#define ERROR 3
/*Fim da definiÃ§Ã£o dos estados*/

/*Variaveis globais*/
volatile unsigned char dado; /*Utilizado para registar o input do utilizador */
unsigned char string[10]; /*String que guardara o input do utilizador*/
volatile __bit RxDado; /*Variavel responsavel por controlar a receção de dados*/
volatile __bit TxDado;	/*Variavel responsavel por controlar o envio de dados*/
unsigned char endReception; /*Responsavel por indicar quando uma string foi introduzida*/
/*Fim das variaveis globais*/

/*Prototipos*/
void globalInit();
void UART1_Init();
void Port_IO_Init();
void interruptsInit();
void isr_UART1 (void) __interrupt(16);
void enviaIndexValue(signed char x);
void recebeString(void);
unsigned char validaString(void);
void sendCaracter(unsigned char x);
void message();
void newLine();
/*Fim de prototipos*/

void main(void)
{
	/*Inicialização de variaveis globais*/
	globalInit();
	/*Fim da inicialização de variaveis globais*/
	/*Configurações*/
	PCA0MD |= 0x00;/*Faz disahble do watchdog*/
	UART1_Init();
	Port_IO_Init();
	interruptsInit();
	/*Fim de configurações*/
	
	/*Inicialização de variaveis locais*/
	signed char index = 0;	/*Utilizado para sabermos que numero deveremos mostar*/
	__code unsigned char digitsOnP2[] = {0xC0/*0*/, 0xF9/*1*/, 0xA4/*2*/, 0xB0/*3*/, 0x99/*4*/, 0x92/*5*/, 0x82/*6*/, 
	0xF8/*7*/, 0x80/*8*/, 0x98/*9*/, 0x88/*A*/, 0x83/*b*/, 0xC6/*C*/, 0xA1/*d*/, 0x86/*E*/, 0x8E/*F*/};
	volatile unsigned char state = DISPLAY, incORdec;
	/*Fim da inicialização de variaveis locais*/
	
	/*Implementação da maquina de estados*/
	switch(state){
		case DISPLAY:
			P2 = digitsOnP2[index & 0x0F];/*Display do numero de 0 a F*/
			/*Uma string foi recebido e iremos proceder ao seu tratamento (inc, dec ou erro)*/
			if(endReception = 1)
				incORdec = validaString();
			if(incORdec == INC)/*Incremento por string*/
				state = INC;
			else if(incORdec == DEC)/*Decremento por string*/
				state = DEC;
			else if(incORdec == ERROR){/*String invalida introduzida*/
				/*Mensagem de erro*/
				sendCaracter('E');
				sendCaracter('R');
				sendCaracter('R');
				sendCaracter('O');
				sendCaracter('R');
				newLine();
			}
			if(P0_6 == 0){/*Incremento por botão*/
				while(P0_6 == 0);
				state = INC;
			}
			if(P0_7 == 0){/*Decremento por botão*/
				while(P0_7 == 0);
				state = DEC;
			}
		break;
		case INC:
			index++;/*Incremento do valor de index*/
			if(index > 15)/*Caso o valor de index seja maior que 15 iremos voltar a 0*/
				index = 0;
			message();
			enviaIndexValue(index);/*envio do valor de index pela porta serie*/
			newLine();
		break;
		case DEC:
			index--;/*Decremento do valor de index*/
			if(index < 0)/*Caso o valor de index seja menor que 0 iremos voltar a 15*/
				index = 15;
			message();
			enviaIndexValue(index);/*envio do valor de index pela porta serie*/

		break;
	}
	/*Fim da implementação da maquina de estados*/
}

void globalInit()
{
	dado = 0;
	RxDado = 0; /*Sem dados recebidos*/
	TxDado = 1; /*Disponivel para receber dados*/
	endReception = 0; /*Nenhuma styring lida*/
}

void UART1_Init()
{
	/*O UART1 foi configurado para um baud rate de 115200 bps/s*/
    SBRLL1    |= 0xF9;	/*Byte menos significativo do valor de reload para gerar o baud rate da UART1*/
    SBRLH1    |= 0xFF;	/*Byte mais significativo do valor de reload para gerar o baud rate de UART1*/
    SCON1     |= 0x10;	/*Ativa a receção por parte da UART1 - (REN1 = 1;)*/
    SBCON1    |= 0x43;	/*Ativação do gerador de baud rate e um presacler de 1 (SYSCLk / 1)*/
    RI1 = 0;			/*Estado pronto para receção*/
    TI1 = 1;			/*Estado pronto para envio */
}

void Port_IO_Init()
{
	P0SKIP    |= 0x0F;	/*Ira fazer skip aos pinos do porto P0 até a UART1 estar direcioanda para os pinos P0_4 e
	P0_5, devido as limitações fisicas da placa*/
    XBR1      |= 0x40;	/*Ativação do crossbar*/
    XBR2      |= 0x01;	/*Ativação da UART1 no crossbar*/
}

void interruptsInit()
{
	EIE2 |= 0x02; /*Ativa as interrupções geradas pela UART1*/
	IE |= 0x80; /*Ativação de toas as interrupções definidas a cima*/
}

void isr_UART1 (void) __interrupt(16) /*As interrupções geradas pela UART1 são vetorizadas na posição 16 de 
acordo com o datasheet*/
{
	/*Interrupção gerada pela receção*/
	if(RI1){/*A receção acabou de acontecer*/
		RI1 = 0; /*Limpeza da flag de transmissão*/
		RxDado = 1; /*Receção realizada*/
		dado = SBUF1; /*Receção do byte - char*/
	}
	/*Fim de interrupção gerada pela receção*/
	/*Interrupção gerada por trasmissão*/
	if(TI1){
		TI1 = 0; /*Envio a decorrer*/
		if(TxDado){	/*A flag foi ativada pelo utilizador*/
			SBUF1 = dado;
			TxDado = 0; /*A realizar o envio*/
		}
		else
			TxDado = 1; /*Fim do envio pronto para o proximo*/
	}	
	/*Fim de interrupção gerada por trasmissão*/
}

void enviaIndexValue(signed char x)
{
	if(x < 10){/*Numeros com apenas um digito*/
		while(!TxDado);
		dado = (x + 48);
		TxDado = 1;	/*Prontos para realizar o envio da variavel dado*/
		TI1 = 1; /*Chamada da ISR*/
	}
	else{/*Numeros com 2 digitos*/
		/*Declaração e inicialização de dados*/
		unsigned char d1, d2; /*d1 stands for digito 1 e d2 para digito 2*/
		/*Fim da declaração e inicialização de dados*/
		/*Divisão nos dois diferentes digitos*/
		d2 = x % 10;
		x = x / 10;
		d1 = x % 10;
		/*Fim da divisão nos diferentes digitos*/
		while(!TxDado);
		dado = (d1 + 48);
		TxDado = 1; /*Prontos para realizar o envio da variavel dado*/
		TI1 = 1; /*Chamada da ISR*/
		/*Fim do envio do digito mais significativo*/
		while(TxDado);
		dado =  (d2 + 48);
		TxDado = 1; /*Prontos para realizar o envio da variavel dado*/
		TI1 = 1; /*Chamada da ISR*/
		/*Fim do envio do digito menos significativo*/
	}
}

void recebeString(void)
{
	/*Declaração e inicialização de dados*/
	unsigned char i = 0;/*i stands for index*/
	if(RxDado){/*Receção acabou de acontecer*/
		while(dado != '\n'){/*verificação se o utilizador ainda esta a inserir caracteres validos*/
			string[i] = dado;/*Passagem dos caracteres validos para a string de verificação*/
			i++;/*Incremento do numero de caracteres passados como input*/
			RxDado = 0; /*Pronto para nova receção*/
		}
	}
	endReception = 1;
}

/*Iremos verificar se os caracteres inseridos são validos caracter a caracter*/
unsigned char validaString(void)
{
	if(string[0] == 'i' && string[1] == 'n' && string[2] == 'c')
		return INC;
	else if(string[0] == 'd' && string[1] == 'e' && string[2] == 'c')
		return DEC;
	else
		return ERROR;
}

void sendCaracter(unsigned char x)
{
	while(!TxDado);
	dado = x;
	TxDado = 1;
	TI1 = 1;
}

/*Função utilizada para enviar a mensagem 'Valor' antes do valor do index ser utilizado*/
void message()
{
	sendCaracter('V');
	sendCaracter('A');
	sendCaracter('L');
	sendCaracter('O');
	sendCaracter('R');
	sendCaracter(':');
	sendCaracter(' ');
}

void newLine()
{
	sendCaracter('\r');
	sendCaracter('\n');
}