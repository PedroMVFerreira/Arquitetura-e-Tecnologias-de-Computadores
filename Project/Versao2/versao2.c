/*
Autores: Pedro Vale Ferreira (a95699) | Davide Oliveira Peixoto (a93949)
Data: 22 de outubro de 2022 - 23 de outubro de 2022
Versão: 2.2.0
Projeto
Turno: PL2
Docente: Rui Machado 
__________________________________________________________________________________________________________________
Implementaão da maquina de estados referente ao projeto juntamente com o contador da versão 1 
2.2.0. -> teste da maquina de estados

A maquina de estados encontra-se a funcionar complementamente tendo passado nos testes desta versão 23 de 
outubro de 2022 -> 02:03 (Pedro Vale Ferreira)
*/
/*Definição dos estados*/
#define IDLE 0
#define CONT_SEG 1
#define SPLIT 2
#define RESET 3
/*Fim da definição dos estados*/

/*Incio da declaração das variaveis globais*/
unsigned char contFlagTF2H;
unsigned int segs;
/*Fim da declaração das variaveis globais*/

/*Inicio dos prototipos das funções (que não a main)*/
void TimerConfig();
/*Fim dos prototipos das funções (que não a main)*/

#include <c8051f380.h>
/*Função responsavel pelo fluxo do programa*/
void main()
{
	PCA0MD = 0x00;	/*Desativa o watchdog*/
	XBR1 = 0x40;	/*Ativa o crossbar*/ 
	
	TimerConfig();/*Invocação da função necessaria para configurar e inicializar o timer 2*/
	
	/*Declaração e inicialização de variaveis*/
	unsigned char state = IDLE; /*Variavel responsavel por guardar o estado atual da maquina de estados e saber para 
	onde o utilizar quer saltar de acordo com os seus inputs*/
	segs = 0; /*Sempre que o programa é inciado o cronometro é "ligado" a contagem devera começar no segundo 0*/
	/*Fim da declaração e inicialização de variaveis*/
	
	/*A maquina de estados desenvolvida para este projeto será implementada utilizando um switch case
	O botão P0_6 será utilizado como o botão A do enunciado, enquanto que o botão P0_7 será utilizado como botão
	B
	A troca de estados apenas ocorrera quando o valor da variavel responsavel pela contagem do numero de vez que 
	a flag TF2H é ativada for igual a 40 (numero necessario para obter 1 segundo) para que não aja desfaçamento
	nos segundos contados (colocar exemplo detalhado do que poderia acontecer no relatorio)*/
	while(1){/*Inicio do ciclo infinito*/
		switch(state){/*Inicio do switch case -> maquina de estados*/
			
			/*Case IDLE -> o nosso programa irá apenas mostrar o numero guardado na variaveol segundos*/
			case IDLE:
				/*code - Para efeitos de teste sempre que o programa estiver em idle iremos mostar 0 no display*/
				P2 = 0xC0; /*Representação do numero 0 no display*/
				
				/*Caso o botão A seja pressionado o programa alterara o seu estado para CONT_SEG*/
				if(P0_6 == 0){/*Botão A pressionado*/
					while(P0_6 == 0);
					state = CONT_SEG;/*Alteração para o estado pretendido -> CONT_SEG*/
				}/*Fim do if -> botão A pressionado*/
				
				/*Caso o botão B seja pressionado o programa alterara o seu estado para RESET
				Como definido na maquina de estados no caso de o programa receber o input do botão A e do botão
				B em simultaneo o input do botão A teria prefencia dai apenas saltarmos para o estado de RESET
				se o botão A não for pressionado ao mesmo tempo que o botão B*/
				if(P0_7 == 0 && P0_6 != 0){/*Botão B pressioando e botão A não pressionado*/
					/*while(P0_7 == 0); Linha comentada devido a natureaza do teste do case RESET*/
					state = RESET;/*Alteração para o estado pretendido -> RESET*/
				}/*Fim do if -> botão B pressionado e botão A não pressionado*/
			break;
			/*Fim do Case IDLE*/
			
			/*Case CONT_SEG -> o nosso programa irá mostrar o numero guardado na variavel segundos, e ira também
			incrementar a mesma 1 unidade sempre que 1 segundo se passar (controlo feito pelo timer 2)*/
			case CONT_SEG:
				/*Code - Para efeitos de teste sempre que o programa estiver em modo CONT-SEGS iremos mostrar 1
				no display*/
				P2 = 0xF9; /*Representação do numero 1 no display*/
				
				/*Caso o botão A seja pressionado o programa alterara o seu estado para IDLE*/
				if(P0_6 == 0){/*Botão A pressionado*/
					while(P0_6 == 0);
					state = IDLE;/*Alteração para o estado pretendido -> IDLE*/
				}/*Fim do if -> botão A pressionado*/
				
				/*Caso o botão B seja pressionado o programa alterara o seu estado para SPLIT
				Como definido na maquina de estados no caso de o programa receber o input do botão A e do botão
				B em simultaneo o input do botão A teria prefencia dai apenas saltarmos para o estado de RESET
				se o botão A não for pressionado ao mesmo tempo que o botão B*/
				if(P0_7 == 0 && P0_6 != 0){/*Botão B pressioando e botão A não pressionado*/
					state = SPLIT;/*Alteração para o estado pretendido -> SPLIT*/
				}/*Fim do if -> botão B pressionado e botão A não pressionado*/
			break;
			/*Fim do Case CONT_SEGS*/
			
			/*Case SPLIT -> o nosso programa irá mostrar o numero guardado na variavel segundos e passar o seu 
			valor para a variavel splitValue para que possa ser consultada posteriormente*/
			case SPLIT:
				/*Code - Para efeitos de teste sempre que o programa estiver em modo SPLIT iremos msotrar 2 no
				display*/
				P2 = 0xA4; /*Representação do numero 2 no display*/
				
				/*Enquanto o botão B estiver pressionado iremos nos manter no estado split caso o botão B seja 
				largado voltaremos automaticamente para o estado IDLE*/
				while(P0_7 == 0){/*O botão B permanece pressionado */
					state = SPLIT;
				}/*Fim de o botão B permanece pressionado*/
				/*O botão B foi largado*/
				state = IDLE;
			break;
			/*Fim do Case SPLIT*/
			
			/*Case RESET -> o  nosso programa irá colocar o valor guardado na variavel segs a 0 (valor inicial),
			enviado depois o programa para o case IDLE permitindo que um novo ciclo de contagens possa ser 
			realizado se o utilizador assim o desejar*/
			case RESET:
				/*Code - Para efeitos de teste sempre que o programa estiver em modo SPLIT iremos mostrar 3 no
				display enquanto o botão B estiver pressionado e iremos alterar o estado quando este for 
				largado, semelhante ao mecanismo do estado SPLIT*/
				P2 = 0xB0; /*Representação do numero 3 no display*/
				
				/*Teste*/
				while(P0_7 == 0)
					state = RESET;
				/*O programa voltara automaticamente ao modo IDLE depois de limpar a variavel segs*/
				state = IDLE;
			break;
			/*Fim do Case RESET*/
			
		}/*Fim do switch case -> maquina de estados*/
	}/*Fim do ciclo infintio*/
}/*Fim da função main*/

/*Função responsavel pelas configurações e inicializações do timer*/
void TimerConfig()
{
	/*A seguinte configuração permite-nos programar o relogio que irá alimentar o timer 2 para uma frequencia
	de 2 MHz (2 000 000) Hz e um periodo respetivo de 0.5 us (0.0000005 s).
	O que fará com que seja necessario obter um total de 40 transbordos de 0xFFFF para 0x0000 começando 
	a contagem quando apenas faltrem 50000 ciclos para este transbordo e respetiva ativação da flag TF2H para
	que se tenha passado 1 segundo desde o inicio da contagem
	Visto estar a ser usado o timer 2 no modo 16 bits com autoreload apenas será necessario configurar os registos
	de paragem uma vez*/
	int reload = -50000; /*Faltam 50000*/
	CLKSEL |= 0x02;/*Configuração do clock*/
	TMR2RLL = reload;/*Configuração dos registos de paragem*/
	TMR2RLH = reload >> 8;/*Configuração dos registos de paragem*/
	TR2 = 1;/*Inicialização do timer 2*/
}