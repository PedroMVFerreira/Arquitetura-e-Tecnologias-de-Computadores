/*
Autores: Pedro Vale Ferreira (a95699) | Davide Oliveira Peixoto (a93949)
Data: 24 de outubro de 2022 a 25 de outubro de 2022
Versão: 2.2.1
Projeto
Turno: PL2
Docente: Rui Machado 
__________________________________________________________________________________________________________________
Implementaão da maquina de estados referente ao projeto juntamente com o contador da versão 1 
2.2.0. -> teste da maquina de estados

A maquina de estados encontra-se a funcionar complementamente tendo passado nos testes desta versão - 23 de 
outubro de 2022 -> 02:03 (Pedro Vale Ferreira)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
2.2.1  -> teste da maquina de estados juntamente com o cronometro desenvolvido na versão 1 

A junção da maquina de estados com o cronometro da versão anterior encontram-se a funcionar perfeitametne - sendo
aogra necessario implmentar o uso de portas series, limitar o range superior do cronometro (12 horas) e modelar
o codigo - 25 de outubro de 2022 -> 15:15 (Pedro Vale Ferreira)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
*/
/*Definição dos estados*/
#define IDLE 0
#define CONT_SEG 1
#define SPLIT 2
#define RESET 3
/*Fim da definição dos estados*/

/*Incio da declaração das variaveis globais*/
/*Variavel auxiliar que ira contar o numero de vezes que a flag TF2H transbordou*/
unsigned char contFlagTF2H;
/*Variavel auxiliar que ira contar o numero de segundos passados desde a ativação atual do cronometro*/
unsigned int segs;
/*Array que irá guardar a forma de demonstar todos os digitos necessarios no display de 7 segmentos*/
char digitsOnP2[10] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x98};
/*Variavel auxiliar que ira guardar o valor de segs sempre que for feito split */
unsigned int splitValue;
/*Fim da declaração das variaveis globais*/

#include <c8051f380.h>
/*Função responsavel pelo fluxo do programa*/
void main()
{
	PCA0MD = 0x00;	/*Desativa o watchdog*/
	XBR1 = 0x40;	/*Ativa o crossbar*/ 
	
	/*Declaração e inicialização de variaveis*/
	/*Devido à natureza do enunciado de limitar o uso do contador a 12 horas continuas apenas temos de trabalhar
	com valores a variar de 0 segundos (1digito) até 43 200 segundos (5 digitos)*/
	unsigned char digito1, digito2, digito3, digito4, digito5;
	int segsBackup; /*Iremos usar uma variavel de backup do valor de segundos para podermos manipular este valor
	sem nunca perder o seu valor original*/
	/*Fim da declaração e inicialização de variaveis*/
	
	/*Configuração do timer2*/
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
	/*Fim da cofiguração do timer 2*/
	
	/*Declaração e inicialização de variaveis*/
	unsigned char state = IDLE; /*Variavel responsavel por guardar o estado atual da maquina de estados e saber para 
	onde o utilizar quer saltar de acordo com os seus inputs*/
	int segs = 0; /*Sempre que o programa é inciado o cronometro é "ligado" a contagem devera começar no segundo 0*/
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
				/*Caso exista o transbordo da flag TF2H iremos incrementar a variavel auxiliar responsavel pelo 
				contagem do numero de transbordos limpado a mesma sempre que esta atingir 40 (passagem de 1 segundo)*/
				while(!TF2H);
				TF2H = 0;
				contFlagTF2H++;
				if(contFlagTF2H == 40)
					contFlagTF2H = 0;
				/*Fim do caso transbordo da falg TF2H -> utilização do metodo de polling para */
				segsBackup = segs;
				/*code - Iremos mostar o numero guardado na variavel segs (de acordo com o tempo definido para 
				os numeros com varios digitos) sem incremento da variavel seg*/
				/*Numeros com apenas 1 digito*/
				if(segs <= 9)
					P2 = digitsOnP2[segsBackup];
				/*FIM - numeros com apenas 1 digitos*/
				/*Numeros com 2 digitos*/
				if(segs >= 10 && segs <= 99){
					/*Separação de segundos nos seus diferentes digitos*/
					digito2 = segsBackup % 10;
					segsBackup = segsBackup / 10;
					digito1 = segsBackup % 10;
					/*FIM - Separação de segundos nos seus diferentes digitos*/
					/*Colocação dos diferentes digitos no display de 7 segmentos de acordo com os tempos definidos*/
					if(contFlagTF2H <= 20)
						P2 = digitsOnP2[digito1];
					if(contFlagTF2H >= 21 && contFlagTF2H <= 40)
						P2 = digitsOnP2[digito2];
					/*FIM - Colocação*/
				}//FIM - numeros com 2 digitos*/
				/*Numeros com 3 digitos*/
				if(segs >= 100 && segs <= 999){
					/*Separação de segundos nos seus diferentes digitos*/
					digito3 = segsBackup % 10;
					segsBackup = segsBackup / 10;
					digito2 = segsBackup % 10;
					segsBackup = segsBackup / 10;
					digito1 = segsBackup % 10;
					/*FIM - Separação de segundos nos seus diferentes digitos*/
					/*Colocação dos diferentes digitos no display de 7 segmentos de acordo com os tempos definidos*/
					if(contFlagTF2H <= 8)
						P2 = digitsOnP2[digito1];
					if(contFlagTF2H >= 9 && contFlagTF2H <= 24)
						P2 = digitsOnP2[digito2];
					if(contFlagTF2H >= 25 && contFlagTF2H <= 40)
						P2 = digitsOnP2[digito3];
					/*FIM - Colocação*/
				}/*FIM - numeros com 3 digitos*/
				/*Numeros com 4 digitos*/
				if(segs >= 1000 && segs <= 9999){
					/*Separação de segundos nos seus diferentes digitos*/
					digito4 = segsBackup % 10;
					segsBackup = segsBackup / 10;
					digito3 = segsBackup % 10;
					segsBackup = segsBackup / 10;
					digito2 = segsBackup % 10;
					segsBackup = segsBackup / 10;
					digito1 = segsBackup % 10;
					/*FIM - Separação de segundos nos seus diferentes digitos*/
					/*Colocação dos diferentes digitos no display de 7 segmentos de acordo com os tempos definidos*/
					if(contFlagTF2H <= 10)
						P2 = digitsOnP2[digito1];
					if(contFlagTF2H >= 11 && contFlagTF2H <= 20)
						P2 = digitsOnP2[digito2];
					if(contFlagTF2H >= 21 && contFlagTF2H <= 30)
						P2 = digitsOnP2[digito3];
					if(contFlagTF2H >= 31 && contFlagTF2H <= 40)
						P2 = digitsOnP2[digito4];
					/*FIM - Colocação*/
				}/*FIM - numeros com 4 digitos*/
				/*Numeros com 5 digitos*/
				if(segs >= 10000 && segs <= 43200){
					/*Separação de segundos nos seus diferentes digitos*/
					digito5 = segsBackup % 10;
					segsBackup = segsBackup / 10;
					digito4 = segsBackup % 10;
					segsBackup = segsBackup / 10;
					digito3 = segsBackup % 10;
					segsBackup = segsBackup / 10;
					digito2 = segsBackup % 10;
					segsBackup = segsBackup / 10;
					digito1 = segsBackup % 10;
					/*FIM - Separação de segundos nos seus diferentes digitos*/
					/*Colocação dos diferentes digitos no display de 7 segmentos de acordo com os tempos definidos*/
					if(contFlagTF2H <= 8)
						P2 = digitsOnP2[digito1];
					if(contFlagTF2H >= 9 && contFlagTF2H <= 16)
						P2 = digitsOnP2[digito2];
					if(contFlagTF2H >= 17 && contFlagTF2H <= 24)
						P2 = digitsOnP2[digito3];
					if(contFlagTF2H >= 25 && contFlagTF2H <= 32)
						P2 = digitsOnP2[digito4];
					if(contFlagTF2H >= 33 && contFlagTF2H <= 40)
						P2 = digitsOnP2[digito4];
					/*FIM - Colocação*/
				}/*FIM - numeros com 5 digitos*/
				/*Fim do trecho code*/
				
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
					while(P0_7 == 0);
					state = RESET;/*Alteração para o estado pretendido -> RESET*/
				}/*Fim do if -> botão B pressionado e botão A não pressionado*/
			break;
			/*Fim do Case IDLE*/
			
			/*Case CONT_SEG -> o nosso programa irá mostrar o numero guardado na variavel segundos, e ira também
			incrementar a mesma 1 unidade sempre que 1 segundo se passar (controlo feito pelo timer 2)*/
			case CONT_SEG:
				/*Caso exista o transbordo da flag TF2H iremos incrementar a variavel auxiliar responsavel pelo 
				contagem do numero de transbordos limpado a mesma sempre que esta atingir 40 (passagem de 1 segundo)*/
				while(!TF2H);
				TF2H = 0;
				contFlagTF2H++;
				if(contFlagTF2H == 40){
					contFlagTF2H = 0;
					segs++;
				}
					segsBackup = segs;
				/*Fim do caso transbordo da falg TF2H -> utilização do metodo de polling para */
				/*Code - Para alem de mostrarmos tal como no estado 'IDLE' o valor de segs iremos tambem 
				incrementar esta variavel uma unidade sempre que efetivamente 1 segundo se passar (40 ativações
				da flag TF2H)*/
				if(contFlagTF2H == 40)
					segs++;
				/*Display da variavel segundos*/
				/*Numeros com apenas 1 digito*/
				if(segs <= 9)
					P2 = digitsOnP2[segsBackup];
				/*FIM - numeros com apenas 1 digitos*/
				/*Numeros com 2 digitos*/
				if(segs >= 10 && segs <= 99){
					/*Separação de segundos nos seus diferentes digitos*/
					digito2 = segsBackup % 10;
					segsBackup = segsBackup / 10;
					digito1 = segsBackup % 10;
					/*FIM - Separação de segundos nos seus diferentes digitos*/
					/*Colocação dos diferentes digitos no display de 7 segmentos de acordo com os tempos definidos*/
					if(contFlagTF2H <= 20)
						P2 = digitsOnP2[digito1];
					if(contFlagTF2H >= 21 && contFlagTF2H <= 40)
						P2 = digitsOnP2[digito2];
					/*FIM - Colocação*/
				}//FIM - numeros com 2 digitos*/
				/*Numeros com 3 digitos*/
				if(segs >= 100 && segs <= 999){
					/*Separação de segundos nos seus diferentes digitos*/
					digito3 = segsBackup % 10;
					segsBackup = segsBackup / 10;
					digito2 = segsBackup % 10;
					segsBackup = segsBackup / 10;
					digito1 = segsBackup % 10;
					/*FIM - Separação de segundos nos seus diferentes digitos*/
					/*Colocação dos diferentes digitos no display de 7 segmentos de acordo com os tempos definidos*/
					if(contFlagTF2H <= 8)
						P2 = digitsOnP2[digito1];
					if(contFlagTF2H >= 9 && contFlagTF2H <= 24)
						P2 = digitsOnP2[digito2];
					if(contFlagTF2H >= 25 && contFlagTF2H <= 40)
						P2 = digitsOnP2[digito3];
					/*FIM - Colocação*/
				}/*FIM - numeros com 3 digitos*/
				/*Numeros com 4 digitos*/
				if(segs >= 1000 && segs <= 9999){
					/*Separação de segundos nos seus diferentes digitos*/
					digito4 = segsBackup % 10;
					segsBackup = segsBackup / 10;
					digito3 = segsBackup % 10;
					segsBackup = segsBackup / 10;
					digito2 = segsBackup % 10;
					segsBackup = segsBackup / 10;
					digito1 = segsBackup % 10;
					/*FIM - Separação de segundos nos seus diferentes digitos*/
					/*Colocação dos diferentes digitos no display de 7 segmentos de acordo com os tempos definidos*/
					if(contFlagTF2H <= 10)
						P2 = digitsOnP2[digito1];
					if(contFlagTF2H >= 11 && contFlagTF2H <= 20)
						P2 = digitsOnP2[digito2];
					if(contFlagTF2H >= 21 && contFlagTF2H <= 30)
						P2 = digitsOnP2[digito3];
					if(contFlagTF2H >= 31 && contFlagTF2H <= 40)
						P2 = digitsOnP2[digito4];
					/*FIM - Colocação*/
				}/*FIM - numeros com 4 digitos*/
				/*Numeros com 5 digitos*/
				if(segs >= 10000 && segs <= 43200){
					/*Separação de segundos nos seus diferentes digitos*/
					digito5 = segsBackup % 10;
					segsBackup = segsBackup / 10;
					digito4 = segsBackup % 10;
					segsBackup = segsBackup / 10;
					digito3 = segsBackup % 10;
					segsBackup = segsBackup / 10;
					digito2 = segsBackup % 10;
					segsBackup = segsBackup / 10;
					digito1 = segsBackup % 10;
					/*FIM - Separação de segundos nos seus diferentes digitos*/
					/*Colocação dos diferentes digitos no display de 7 segmentos de acordo com os tempos definidos*/
					if(contFlagTF2H <= 8)
						P2 = digitsOnP2[digito1];
					if(contFlagTF2H >= 9 && contFlagTF2H <= 16)
						P2 = digitsOnP2[digito2];
					if(contFlagTF2H >= 17 && contFlagTF2H <= 24)
						P2 = digitsOnP2[digito3];
					if(contFlagTF2H >= 25 && contFlagTF2H <= 32)
						P2 = digitsOnP2[digito4];
					if(contFlagTF2H >= 33 && contFlagTF2H <= 40)
						P2 = digitsOnP2[digito4];
					/*FIM - Colocação*/
				}/*FIM - numeros com 5 digitos*/
				/*Fim do display da variavel segundos*/
				
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
				/*Code - O valor de segundos será guardado na variavel auxiliar de split (Em versões futuras este
				valor deverá ser configurado em horas minutos e segundos (utilizar exercicio de MP1) e enviado
				de volta atraves da porta serie) na versão atual apenas pode ser acesso por debbug de hardware*/
				splitValue = segs;
				/*Enquanto estivermos em modo split iremos fazer com que apenas o ponto do display de 7 segmentos
				esteja visivel*/
				P2 = 0x7F;
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
				/*Code - Quando o utilizador escolhe o estado de reset a variavel de contagem de segundos sera
				limpa e o programa passara imediatamente para o modo idle esperando nova decisão do utilizador
				(Em versões futuras seria interessante enviar o ultimo valor antes de reset da variavel segundo
				de volta atraves da porta serie)*/
				segs = 0;
				state = IDLE;
			break;
			/*Fim do Case RESET*/
			
		}/*Fim do switch case -> maquina de estados*/
	}/*Fim do ciclo infintio*/
}/*Fim da função main*/

