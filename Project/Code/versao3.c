/*
Autores: Pedro Vale Ferreira (a95699) | Davide Oliveira Peixoto (a93949)
Data: 31 de outubro de 2022 
Versão 3.0.0
Projeto
Turno: PL2
Docente: Rui Machado
____________________________________________________________________________________________________________________
2.2.0 -> Implementação da maquina de estados referente ao projeto juntamente com o contador da versão 1 do projeto

A maquina de estados encontra-se a funcionar corretamente - 23 de outubro de 2022 -> 02:03 (Pedro Vale Ferreira)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
2.2.1 -> Teste da maquina de estados referente ao projeto juntamente com o contador da versão 1 do projeto

A junção da maquina de estados (versão anterior) com o cronometro (versão 1) encontra-se a funcionar perfeitamente -
25 de outubro de 2022 - 15:15 (Pedro Vale Ferreira)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
3.0.0 -> Divisão do codigo fonte da versão anterior em diferentes funções (seguindo o  fluxograma para a func main)

A divisão do codigo fonte em diferentes funções encontra-se a funcionar corretamente - 31 de outubro de 2022 -> 
12:45 (Pedro Vale Ferreia)
*/

/*Definição dos estados*/
#define IDLE 0
#define CONT_SEGS 1
#define SPLIT 2
#define RESET 3
/*Fim da definição dos estados*/

/*Definição do tempo maximo de contagem do cronometro*/
#define MAXTIME 432000 /*12 horas = 720 minutos = 43200 segundos*/
/*Fim da definição do tempo maximo de contagem do cronometro*/

/*Declaração de variaveis globais*/
unsigned char contFlagT2; /*Variavel auxiliar responsavel por contador o numero de transbordos da flag do timer 2*/
unsigned int segs = 0;
/*Fim da declaração de variaveis globais*/

/*Header files*/
#include <c8051f380.h>
/*Fim de header files*/

/*Prototipo de funções*/
void globalInit(void);
void generalConfig(void);
void tempConfig(void);
void timer2Start(void);
void timer2_isr(void) __interrupt (5);
void idle(void);
void digitos (void);
void contSegs(void);
void reset(void);
/*Fim do prototipo de funções*/

/*Função 'main' -> responsavel pelo fluxo do programa*/
void main(void)
{
	/*Declaração e inicialização de dados*/
	unsigned char state = IDLE;
	/*Fim da declaração e inicialização de dados*/
	
	globalInit();	 /*Invocação da função responsavel pela inicialização das variaveis globais*/
	generalConfig(); /*Invocação da função responsavel pelas configurações gerais do micro-controlador*/
	tempConfig();	 /*Invocação da função responsavel pelas configurações dos clocks e timers utilizados*/
	timer2Start();	 /*Invocação da função responsavel pelo inicio da contagem comandada pelo timer 2*/
	
	/*Ciclo infinito que nos permite correr o nosso programa no micro-controlador*/
	while(1){
		/*Caso o valor de contagem (valor guardado na variavel 'segs' seja igual ao definido com o valor maximo
		de contagem ('MAXTIME') o programa ira enviar o seu valor de volta para o terminal e entrar no estado de
		reset*/
		if(segs == MAXTIME){
			/*Envio da variavel 'segs' de volta para o terminal*/
			state = RESET;
		}
		/*Fim do caso fim de contagem por tempo excedido*/
		
		/*Execução normal da contagem de tempo*/
		else{
			/*Implementação da maquina de estados pensada para o projeto utilizando um switch case*/
			switch(state){
				case IDLE:
					idle();	/*Invocação da função responsavel pelo tratamento de 'contFlagT2' no estado 'IDLE'*/
					digitos();	/*Invocação da função responsavel por mostar os diferentes digitos de segs*/
					
					/*Botão A pressionado e alteração de estado de acordo com a maquina de estados*/
					if(P0_6 == 0){
						while(P0_6 == 0);
						state = CONT_SEGS;
					}
					/*Fim de botão A pressionado
					
					/*Botão B pressioando e botão A não pressionado, pois definimos a prioridade no botão A e 
					alteração de estado de acordo com a maquina de estados*/
					if(P0_6 ==  1 && P0_7 == 0){
						while(P0_7 == 0);
						state = RESET;
					}
					/*Fim de botão B pressionado*/
				break;
				
				case CONT_SEGS:
					contSegs();	/*Invocação da função responsavel pelo tratamento de 'contFlagT2' no 
					estado 'CONT_SEGS'*/
					digitos();	/*Invocação da função responsavel por mostar os diferentes digitos de segs*/
					
					/*Botão A pressionado e alteração de estado de acordo com a maquina de estados*/
					if(P0_6 == 0){
						while(P0_6 == 0);
						state = IDLE;
					}
					/*Fim de botão A pressionado
					
					/*Botão B pressionado e botão A não pressioando, pois definimos a prioridade no botão A e 
					alteração de estado de acordo com a maquina de estados (estaremos no estado 'SPLIT' enquanto
					o botão B estiver pressionado*/
					if(P0_6 == 1 && P0_7 == 0)
						state = SPLIT;
					/*Fim de botão B pressionado*/
				break;
				
				case SPLIT:
					P2_7 = 0; /*Teste*/
					/*Enquanto o botão B estiver pressionado manteremos nos no estado 'SPLIT', caso o botão B
					seja largado iremos automaticamente para o modo 'IDLE'*/
					while(P0_7 == 0)
						state = SPLIT;
					state = IDLE;
				break;
				
				case RESET:
					reset();	/*Função responsavel pelo estado de 'reset'*/
					state = IDLE;
				break;
			}
			/*Fim da implementação da maquina de estados*/
		}
		/*Fim do caso fim da execução normal de contagem*/
	}
	/*Fim do ciclo infinito*/
}
/*Fim da função 'main'*/

/*Função 'globalInit' -> responsavel pela inicialização das variaveis globais*/
void globalInit(void)
{
	contFlagT2 = 0;
}
/*Fim da função 'globalInit*/

/*Função 'generalConfig -> responsavel pelas configurações gerais do micro-controlador*/
void generalConfig(void)
{
	PCA0MD = 0x00;	/*Desativa o watchdog*/
	XBR1 = 0x40;	/*Ativa o crossbar*/
	ET2 = 1;		/*Ativação das interrupções geradas pelas flags do timer 2*/
	EA = 1;			/*Ativação das interrupções de acordo com as definições individuais*/
}
/*Fim da função 'generalConfig'*/

/*Função 'tempConfig' -> reponsavel pelas configurações dos clocks e timers usados*/
void tempConfig(void)
{
	/*Declaração e inicialização de dados*/
	int reload = -50000;	/*Faltam 50000 ciclos para o transbordo*/
	/*Fim da declaração e inicialização de dados*/
	
	/*Configuração timer 2
	A seguinte configuração permite-nos configurar o relogio que ira configurar o timer 2 para uma frequencia de
	2MHz e um periodo respetivo de 0.5 us
	Como iremos utilizar o timer 2 no modo 16 bits com auto reload os registo CKCON (datasheet pagina 225) e 
	TMR2CN (datasheet pagina 239) serão deixados por defeito - o relogio do sistema sera dividido por 12 e as 
	interrupções geradas pela flag do byte menos significativo do timer 2 estaram desativadas apesar de as
	interrupções do timer 2 estarem ativadas*/
	CLKSEL |= 0x02; /*Define que o relogio do sistema sera alimentado pela frequencia do IHFO divida por 2*/
	
	/*Configuração dos registos de inicio de contagem*/
	TMR2RLL = reload;
	TMR2RLH = reload >> 8;
	/*Fim da configuração dos registos de inicio de contagem*/
}
/*Fim da função 'tempConfig'*/

/*Função 'timer2Start' -> responsavel pelo inicio da contagem comandada pelo timer 2*/
void timer2Start(void)
{
	TR2 = 1;
}
/*Fim da função 'timer2Start'*/

/*Função 'timer2_isr' -> responsavel pela rotina de atendimento à interrupção gerada pelas flags do timer 2*/
void timer2_isr(void) __interrupt (5)
{
	/*As flags do timer 2 apenas são limpas por software (devido às configurações anteriores apenas a flag TF2H
	gerara interrupções). A variavel auxiliar responsavel por contar o numero de transbordos será incrementada*/
	TF2H = 0;		/*Limpeza da flag responsavel pela interrupção*/
	contFlagT2++;	/*incremento da variavel auxiliar em uma unidade*/
}
/*Fim da função 'timer2_isr'*/

/*Função 'idle' -> responsavel pelo tratamento da variavel 'contFlagT2' no estado 'IDLE'*/
void idle(void)
{
	/*Sempre que a variavel auxiliar 'contFlagT2' atingir o valor 40 (numero de transbordos do timer 2 de 0xFFFF
	para 0x0000) terá se passado 1 segundo, logo a variavel sera limpara para que a forma como mostramos os 
	diferentes digitos da variavel 'segs' seja precisa*/
	if(contFlagT2 == 40)
		contFlagT2 = 0;
}
/*Fim da função 'idle'*/

/*Função 'digitos' -> responsavel por apresentar os diferentes digitos da variavel 'segs' no display 
de 7 segmentos*/
void digitos (void)
{
	/*Declaração e inicialização de dados*/
	__code unsigned char rep[10] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x98};	/*Array 
	responsavel por guardar a representação dos diferentes digitos no display de 7 segmetnos*/
	unsigned int backup = segs;	/*variavel utilizada para manipular o valor de segundos*/
	unsigned char digito1, digito2, digito3, digito4, digito5; 
	/*Fim da declaração e inicialização de dodos*/
	
	/*Segundos com apenas 1 digito
	O unico digito da variavel 'segundos' ira ser representado durante todo o segundo disponivel*/
	if(backup <= 9)
		P2 = rep[backup];
	/*Fim de segundos com apenas 1 digito*/
	
	/*Segundos com 2 digitos
	É necessario proceder à divisão do valor de 'segundos' nos seus diferentes digitos e mostrar cada um deles
	por 0.5s em cada segundo disponivel*/
	if(backup >= 10 && backup <= 99){
		/*Divisão nos seus diferente digitos*/
		digito2 = backup % 10;
		backup = backup / 10;
		digito1 = backup % 10;
		/*Fim da divisão nos seus diferentes digitos*/
		/*Representação no display de 7 segmentos*/
		if(contFlagT2 <= 20)
			P2 = rep[digito1];
		if(contFlagT2 >= 20 && contFlagT2 <= 40)
			P2 = rep[digito2];
		/*Fim da representação no display de 7 segmentos*/
	}
	/*Fim de segundos com 2 digitos*/
	
	/*Segundos com 3 digitos
	É necessario proceder à divisão do valor de 'segundos' nos seus difrentes digitos e mostrar o primeiro por 
	0.2s e os dois restantes por 0.4s em cada segundo disponivel*/
	if(backup >= 100 && backup <= 999){
		/*Divisão nos seus diferentes digitos*/
		digito3 = backup % 10;
		backup = backup / 10;
		digito2 = backup % 10;
		backup = backup / 10;
		digito1 = backup % 10;
		/*Fim da divisão nos seus diferentes digitos*/
		/*Representação no display de 7 segmentos*/
		if(contFlagT2 <= 8)
			P2 = rep[digito1];
		if(contFlagT2 >= 9 && contFlagT2 <= 24)
			P2 = rep[digito2];
		if(contFlagT2 >= 25 && contFlagT2 <= 40)
			P2 = rep[digito3];
		/*Fim da representação no display de 7 segmetnos*/
	}
	/*Fim de segundos com 3 digitos*/
	
	/*Segundos com 4 digitos
	É necesario proceder à divisão do valor de 'segundos' nos seus diferentes digitos e mostar todos por 0.25s 
	em cada segundo disponivel*/
	if(backup >= 1000 && backup <= 9999){
		/*Divisão nos seus diferentes digitos*/
		digito4 = backup % 10;
		backup = backup / 10;
		digito3 = backup % 10;
		backup = backup / 10;
		digito2 = backup % 10;
		backup = backup / 10;
		digito1 = backup % 10;
		/*Fim da divisão nos seus diferentes digitos*/
		/*Representação no display de 7 segmentos*/
		if(contFlagT2 <= 10)
			P2 = rep[digito1];
		if(contFlagT2 >= 11 && contFlagT2 <= 20)
			P2 = rep[digito2];
		if(contFlagT2 >= 21 && contFlagT2 <= 30)
			P2 = rep[digito3];
		if(contFlagT2 >= 31 && contFlagT2 <= 40)
			P2 = rep[digito4];
		/*Fim da representação no display de 7 segmetnos*/
	}
	/*Fim de segundos com 4 digitos*/
	
	/*Segundos com 5 digitos
	É necessario proceder à divisão do valor de 'segundos' no seus difernetes digitos e mostrar todos por 0.2s 
	em cada segundo disponivel*/
	if(backup >= 10000 && backup <= 43200){/*43200 valor maximo permitido pelo enunciado*/
		/*Divisão nos seus diferentes digitos*/
		digito5 = backup % 10;
		backup = backup / 10;
		digito4 = backup % 10;
		backup = backup / 10;
		digito3 = backup % 10;
		backup = backup / 10;
		digito2 = backup % 10;
		backup = backup / 10;
		digito1 = backup % 10;
		/*Fim da divisão nos seus diferentes digitos*/
		/*Representação no display de 7 segmentos*/
		if(contFlagT2 <= 8)
			P2 = rep[digito1];
		if(contFlagT2 >= 9 && contFlagT2 <= 16)
			P2 = rep[digito2];
		if(contFlagT2 >= 17 && contFlagT2 <= 24)
			P2 = rep[digito3];
		if(contFlagT2 >= 25 && contFlagT2 <= 32)
			P2 = rep[digito4];
		if(contFlagT2 >= 33 && contFlagT2 <= 40)
			P2 = rep[digito5];
		/*Fim da representação no display de 7 segmetnos*/
	}
}
/*Fim da função 'digitos'*/

/*Função 'contSegs' -> responsavel pelo tratamento da variavel contFlagT2 no estado 'CONT_SEGS'*/
void contSegs(void)
{
	/*Sempre que a variavel auxiliar 'contFlagT2' atingir o valor 40 (numero de transbordos do timer 2 de 0xFFFF
	para 0x0000) terá se passado 1 segundo, logo a variavel sera limpara para que a forma como mostramos os 
	diferentes digitos da variavel 'segs' seja precisa, iremos ainda incrementar a variavel segundos em uma 
	unidade*/
	if(contFlagT2 == 40){
		contFlagT2 = 0;
		segs++;
	}
}
/*Fim da função 'contSegs'*/

/*Função 'reset' -> responsavel pelo estado de 'RESET'*/
void reset(void)
{
	/*Iremos limpar a variavel 'segs' (fazendo reset à mesma)*/
	segs = 0;
}
/*Fim da função 'reset'*/

/*IHFO -> Internal High-Frequency Oscilator*/