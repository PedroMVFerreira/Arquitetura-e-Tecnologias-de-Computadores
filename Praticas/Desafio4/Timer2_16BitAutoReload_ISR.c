/*Autor: Pedro Vale Ferreira (a95699) | Davide Oliveira Peixoto (a93949)
Data: 27 de outubro de 2022
Trabalho: Desafio 4
Versão 2.0
Turno: PL2
Docente: Rui Machado
__________________________________________________________________________________________________________________
Versão que utiliza o timer 2 no modo 16 bits com auto reload e uma main simplista
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Pretende-se alimentar o relogio do timer 2 com uma frequencia de 2 MHz e um periodo de 5us
*/

#include <c8051f380.h>

/*Assinatura das funções*/
void config (void);
void timer2_isr (void) __interrupt (5);
void toogle (void);
/*Fim da assinatura das funções*/

/*Declaração de variaveis globais*/
volatile int reload;
volatile unsigned int contFlagTF2H;
/*Fim da declaração de variaveis globais*/

void main (void)
{
	config(); /*Invoca a função responsavel por todas as configs necessarias*/
	while(1); /*Ciclo infinito para que o nosso programa possa correr no micro*/
}

/*A função config sera responsavel por todas as configurações do micro, bem como do relogio do sistema e do timer*/
void config (void) 
{
	/*Ìnicialiazação de variaveis*/
	reload = -50000;
	contFlagTF2H = 0;
	/*Fim da inicialização de variaveis*/
	
	PCA0MD = 0x00;	/*Desativa o watchdog*/
	XBR1 = 0x40;	/*Ativa o crossbar*/ 
	
	/*Configuração dos clocks*/
	/*A seguinte configuração permite-nos programar o relogio que irá alimentar o timer 2 para uma frequencia
	de 2 MHz (2 000 000) Hz e um periodo respetivo de 0.5 us (0.0000005 s).
	O que fará com que seja necessario obter um total de 40 transbordos de 0xFFFF para 0x0000 começando 
	a contagem quando apenas faltrem 50000 ciclos para este transbordo e respetiva ativação da flag TF2H para
	que se tenha passado 1 segundo desde o inicio da contagem
	Visto estar a ser usado o timer 2 no modo 16 bits com autoreload apenas será necessario configurar os registos
	de paragem uma vez*/
	CLKSEL |= 0x02;/*Configuração do clock do sistema*/
	/*Fim da configuração dos clocks*/
	
	/*Ativação das interupções*/
	ET2 = 1; /*Ativação das interrupções ativadas pelas flags do timer 2 (bit 5 do byte IE)*/
	EA = 1; /*Ativação das interrupções gerais de acordo com as definições de interrupções manuais (bit 7 de IE)*/
	/*As duas linhas de codigo anterior poderiam ser substituidas por IE |= 0xA0*/
	/*Fim da ativação das interrupções*/
	
	TMR2RLL = reload;/*Configuração dos registos de paragem*/
	TMR2RLH = reload >> 8;/*Configuração dos registos de paragem*/
}
/*Fim da função responsavel por todas as configurações*/

/*Rotina de atendimetno à interrupção*/
void timer2_isr (void) __interrupt (5) 
{
	/*A rotina de atendimento à interrupção apenas ira limpar a flag do timer 2 pois esta não é limpa por 
	hardware, ira incrementar a variavel auxiliar responsavel por controlar o numero de transbordos e ira 
	invocar a função responsavel por lidar com o valor da variavel auxiliar*/
	TF2H = 0; /*Limpeza da flag do timer 2 (a unica flag utilizada no modo 16 bit com auto reload é a flag dos
	bits mais significativos*/
	contFlagTF2H++;
	toogle();
}
/*Fim da rotina de atendimento à interrupção*/

void toogle (void)
{
	if(contFlagTF2H == 40){
		contFlagTF2H = 0;
		P2_7 = !P2_7;
	}
}