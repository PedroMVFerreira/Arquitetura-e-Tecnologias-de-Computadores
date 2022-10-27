/*
Autor: Pedro Vale Ferreira (a95699)| Davide Oliveira Peixoto (a93949)
Data: 26 de outubro de 2022 
Trabalho: Desafio 4
Versão: 1.0
Turno: PL2
Docente: Rui Machado
______________________________________________________________________________________________________________
Versão que utiliza o timer 0  (modo 8 bits com autoreload) - utilizando interrupções e ISR para atender as mesmas
e a main simplista
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Pretende se alimentar o relogio do timer 0 no modo 2 com uma frequencia de 2 MHz e um periodo de 0.5us
*/

#include <c8051f380.h>
/*Assinatura de funções*/
void config (void) __using (1);
void timer0_isr (void) __interrupt (1) __using (1);
void toggle (void) __using (1);
/*Fim da assinatura das funções*/
/*Iremos utilizar o banco de registos 0 para a função main (definido por defeito) e o banco de registo 1 
(definido manualmente) para que não seja necesario fazer copias de segurança dos dados da função main
(Mesmo que neste trecho de codigo não existam dados na função main apenas invocações de outras funções usaremos
bancos de dados para praticarmos o seu uso e melhorar o nosso codigo*/

/*Declaração de variaveis globais*/
volatile unsigned int contFlagTF0; /*Variavel responsavel por contar o numero de transbordos da flag TF0*/

/*Fim - declaração de variaveis globais*/

void main (void)
{
	config();/*Invocação da função responsavel por todas as configurações necessarias*/
	while(1);/*Ciclo infinito para que o nosso programa possa correr no micro*/
}

/*A função config sera responsavel por todas as configurações do micro, bem como o do relogio do sistema e ainda 
do timer do relogio do timer utilizado*/
void config (void) __using (1)
{
	/*Declaração e inicialização de variaveis*/
	contFlagTF0 = 0;
	/*Fim - da declaração e inicialização das variaveis*/
	
	PCA0MD = 0x00;	//Desativa o watchdog
	XBR1 = 0x40;	//Ativa o crossbar
	 
	/*Configuração do relogio do sistema e do relogio do timer e modo do timer 0
	Sabendo que a frequencia do IHFO é de 48 MHz*/
	CLKSEL |= 0x02; /*O relogio do sistema será alimentado por uma frequencia de 24 MHz (frequencia do IHFO / 2)*/
	/*O registo CKCON será deixado por defeito (CKCON |= 0x00;) o relogio do timer 0 sera alimentado por uma 
	frequencia de 2 MHz, obtendo um periodo de 0.5 us (frequencia do relogio do sistema / 12)*/
	TMOD |= 0x01; /*Iremos usar o timer 0 no modo 1 (16bits sem autoreaload)*/
	/*Fim das configurações do relogio do sistema e do timer e do modo do timer 0*/
	
	/*Configuração das interrupções*/
	ET0 = 1; /*Abilitamos as interrupções geradas pela flag do timer 0 (TF0)*/
	EA = 1; /*Abilitamos as interrupções gerais de acordo com as definições das interrupções individuais*/
	/*As duas linhas de codigo anteriores pode ser substituidas por IE |= 0x82, pois os registos ET0 e EA são 
	respetivamente o bit 1 e o bit 7 do SFR (byte) IE*/
	/*Fim da configuração das interrupções*/
	
	/*Inicialização dos registos de paragem e de auto reload*/
	int reload = -50000;
	TL0 = reload; //8 bits menos significativos
	TH0 = reload>>8; //shift de 8 posições para termos os 8 bits mais significativos devido ao registo ser de 8 bits e a nossa varável de 16
	/*Fim da inicialização dos registos de paragem e de auto reload*/
	
	TR0 = 1; /*Ativação do timer 0, inicializando a contagem*/
}
/*Fim da função responsavel por todas as configs*/

/*Rotina de atendimento à interrupção*/
void timer0_isr (void) __interrupt (1) __using (1)
{
	/*A rotina de atendimento à interrupção apenas incrementar a variavel auxiliar responsavel por guardar o 
	numero de vezes que existiu transbordo e ira depois invocar a função que lidara com o valor registado nessa
	variavel para que o codigo da ISR seja o mais curto possivel e assim não bloquearmos o programa por tempo 
	desnecessario
	A flag do timer 0 é limpa automaticamente por hardware*/
	contFlagTF0++;
	toggle();
}
/*Fim da rotina de atendimento à interrupção*/

/*Função responsavel por lidar com o valor da variavel 'contFlagTF0' e por fazer toggle ao pino 7 do porto 2*/
/*Sendo que o relogio do timer0 esta a ser alimentado com um periodo de 0.5 us a cada ciclo de clock terão se 
passado 0.5 us, logo a cada transbordo (ativação da flag) terão se passado 250 ciclos e um total de 0.025s
Logo para obtermos 1 seegundo teremos de 40 transbordos da flag, pois 40 * 0.025s = 1s*/
void toggle (void) __using (1)
{
	if(contFlagTF0 == 40){/*Passagem de 1 segundo*/
		contFlagTF0 = 0;/*Limpeza da variavel auxiliar para que seja possivel contar mais 1 segundo*/
		P2_7 = !P2_7;/*Toogle ao pino 7 do porto 2*/
	}
}
/*Fim da função responsavel por lidar com o valor da variavel 'contFlagTF0' e por fazer toggle ao pino 7 do 
porto 2*/
