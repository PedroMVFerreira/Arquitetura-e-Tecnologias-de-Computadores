/*
Autor: Pedro Vale Ferreira (a95699)
Data: 25 de outubro de 2022 
Trabalho: Desafio 3
Versão: 3.0
Turno: PL2
Docente: Rui Machado
______________________________________________________________________________________________________________
Versão que utiliza o timer 0  (modo 8 bits com autoreload) - utilizando interrupções e ISR para atender as mesmas
*/

#include <c8051f380.h>
/*Assinatura de funções*/
void timerConfig();
void timer0_isr (void) __interrupt (1);
/*Fim da assinatura das funções*/

/*Declaração de variaveis globais*/
volatile unsigned int contFlagTF0; /*Variavel responsavel por contar o numero de transbordos da flag TF0*/
/*Fim - declaração de variaveis globais*/

void main()
{
	PCA0MD = 0x00;	//Desativa o watchdog
	XBR1 = 0x40;	//Ativa o crossbar 
	
	/*Declaração de variaveis*/
	unsigned char aux = 0;
	/*Fim da declaração de variaveis
		
	/*Invocação da função necesaria para configurar o timer 0*/
	timerConfig();
	
	EA = 1; /*Ativa todas as interrupções de acordo com as interrupções individuais (neste momento apenas as 
	interrupções do timer 0 estão ativadas)*/
	
	TR0 = 1; /*Ativa o timer 0 inicializando a contagem*/
	while(1){
		/*A cada transbordo da flag passaram-se 0.000125 segundos logo teremos de obter 8000 ativações da flag para
		que se tenha passado 1 segundo. Como 8000 não cabe no nosso range de char iremos ativar uma variavel aux 
		sempre que a flag seja ativada 200 vezes um totald de 40 vezes prefazendo assim as 8000 ativaçõe da flag*/
		if(contFlagTF0 == 200){
			contFlagTF0 = 0;
			aux++;
		}
		if(aux == 40){
			aux = 0;
			P2_7 = !P2_7;
		}	
	}
}

void timerConfig()
{
	/*Inicialização de variaveis*/
	contFlagTF0 = 0;/*Inicialização da variavel responsavel pela contagem do numero de transbordos*/
	/*Pretendemos obter alimentar o clock do timer 0 com uma frequencia de 2 MHz e um periodo de 0.5 us para isso 
	necessitamos (sabendo que a frequencia do IHFO é de 48 MHz) necessitamos de:*/
	CLKSEL |= 0x02; /*O clock do sistema ira derivar do IHFO dividido por 2 -> 24 MHz*/
	/*O registo CKCON será deixado por defeito - o clock do timer 0 será alimentado pelo clock do sistema dividido
	por 12 obtemos assim a frequencia de 2 MHz e um periodo de 0.5 us*/
	TMOD |= 0x02; /*Iremos usar o timer 0 no modo 2 (8 bit Counter/Timer com auto-reload)*/
	ET0 = 1; /*Abilitamos as interrupções geradas pela flag do timer 0 (TF0)*/
	PT0 = 1; /*Colocamos as interrupções timer 0 como prioridade alta*/
	
	/*Inicialização dos registos de paragem e de auto-reload*/
	TL0 = -250;
	TH0 = -250;
	/*Fim da inicialização dos registos de paregem e de auto-reload*/
	
	
}

void timer0_isr (void) __interrupt (1)
{
	/*A flag do timer 0 é limpo automaticamente por hardware*/
	/*Apenas sera necessario incrementar a variavel auxiliar responsavel pela contagem do numero de transbordo*/
	contFlagTF0++;
}