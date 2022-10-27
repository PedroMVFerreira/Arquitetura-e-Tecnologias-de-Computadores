/*
Autor: Pedro Vale Ferreira (a95699)| Davide Oliveira Peixoto (a93949)
Data: 26 de outubro de 2022 
Trabalho: Desafio 4
Vers�o: 1.0
Turno: PL2
Docente: Rui Machado
______________________________________________________________________________________________________________
Vers�o que utiliza o timer 0  (modo 8 bits com autoreload) - utilizando interrup��es e ISR para atender as mesmas
e a main simplista
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Pretende se alimentar o relogio do timer 0 no modo 2 com uma frequencia de 2 MHz e um periodo de 0.5us
*/

#include <c8051f380.h>
/*Assinatura de fun��es*/
void config (void) __using (1);
void timer0_isr (void) __interrupt (1) __using (1);
void toggle (void) __using (1);
/*Fim da assinatura das fun��es*/
/*Iremos utilizar o banco de registos 0 para a fun��o main (definido por defeito) e o banco de registo 1 
(definido manualmente) para que n�o seja necesario fazer copias de seguran�a dos dados da fun��o main
(Mesmo que neste trecho de codigo n�o existam dados na fun��o main apenas invoca��es de outras fun��es usaremos
bancos de dados para praticarmos o seu uso e melhorar o nosso codigo*/

/*Declara��o de variaveis globais*/
volatile unsigned char contFlagTF0; /*Variavel responsavel por contar o numero de transbordos da flag TF0*/
volatile int reload;

/*Fim - declara��o de variaveis globais*/

void main (void)
{
	config();/*Invoca��o da fun��o responsavel por todas as configura��es necessarias*/
	while(1);/*Ciclo infinito para que o nosso programa possa correr no micro*/
}

/*A fun��o config sera responsavel por todas as configura��es do micro, bem como o do relogio do sistema e ainda 
do timer do relogio do timer utilizado*/
void config (void) __using (1)
{
	/*Declara��o e inicializa��o de variaveis*/
	contFlagTF0 = 0;
	/*Fim - da declara��o e inicializa��o das variaveis*/
	
	PCA0MD = 0x00;	//Desativa o watchdog
	XBR1 = 0x40;	//Ativa o crossbar
	 
	/*Configura��o do relogio do sistema e do relogio do timer e modo do timer 0
	Sabendo que a frequencia do IHFO � de 48 MHz*/
	CLKSEL |= 0x02; /*O relogio do sistema ser� alimentado por uma frequencia de 24 MHz (frequencia do IHFO / 2)*/
	/*O registo CKCON ser� deixado por defeito (CKCON |= 0x00;) o relogio do timer 0 sera alimentado por uma 
	frequencia de 2 MHz, obtendo um periodo de 0.5 us (frequencia do relogio do sistema / 12)*/
	TMOD |= 0x01; /*Iremos usar o timer 0 no modo 1 (16bits sem autoreaload)*/
	/*Fim das configura��es do relogio do sistema e do timer e do modo do timer 0*/
	
	/*Configura��o das interrup��es*/
	ET0 = 1; /*Abilitamos as interrup��es geradas pela flag do timer 0 (TF0)*/
	EA = 1; /*Abilitamos as interrup��es gerais de acordo com as defini��es das interrup��es individuais*/
	/*As duas linhas de codigo anteriores pode ser substituidas por IE |= 0x82, pois os registos ET0 e EA s�o 
	respetivamente o bit 1 e o bit 7 do SFR (byte) IE*/
	/*Fim da configura��o das interrup��es*/
	
	/*Inicializa��o dos registos de paragem e de auto reload*/
	reload = -50000;
	TL0 = reload; 
	TH0 = reload >> 8; 
	/*Fim da inicializa��o dos registos de paragem e de auto reload*/
	
	TR0 = 1; /*Ativa��o do timer 0, inicializando a contagem*/
}
/*Fim da fun��o responsavel por todas as configs*/

/*Rotina de atendimento � interrup��o*/
void timer0_isr (void) __interrupt (1) __using (1)
{
	/*A rotina de atendimento � interrup��o apenas incrementar a variavel auxiliar responsavel por guardar o 
	numero de vezes que existiu transbordo e ira depois invocar a fun��o que lidara com o valor registado nessa
	variavel para que o codigo da ISR seja o mais curto possivel e assim n�o bloquearmos o programa por tempo 
	desnecessario
	A flag do timer 0 � limpa automaticamente por hardware
	Visto que estamos num modo sem auto reload sera necessario reload manual*/
	contFlagTF0++;
	TL0 = reload; 
	TH0 = reload >> 8;
	toggle();
}
/*Fim da rotina de atendimento � interrup��o*/

/*Fun��o responsavel por lidar com o valor da variavel 'contFlagTF0' e por fazer toggle ao pino 7 do porto 2*/
/*Sendo que o relogio do timer0 esta a ser alimentado com um periodo de 0.5 us a cada ciclo de clock ter�o se 
passado 0.5 us, logo a cada transbordo (ativa��o da flag) ter�o se passado 250 ciclos e um total de 0.025s
Logo para obtermos 1 seegundo teremos de 40 transbordos da flag, pois 40 * 0.025s = 1s*/
void toggle (void) __using (1)
{
	if(contFlagTF0 == 40){/*Passagem de 1 segundo*/
		contFlagTF0 = 0;/*Limpeza da variavel auxiliar para que seja possivel contar mais 1 segundo*/
		P2_7 = !P2_7;/*Toogle ao pino 7 do porto 2*/
	}
}
/*Fim da fun��o responsavel por lidar com o valor da variavel 'contFlagTF0' e por fazer toggle ao pino 7 do 
porto 2*/
