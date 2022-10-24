/*
Autores: Pedro Vale Ferreira (a95699) | Davide Oliveira Peixoto (a93949)
Data: 2 de outubro de 2022 | 17.40 
Versão: 3
Turno: PL2
Docente: Rui Machado 
*/
#include <c8051f380.h>

void main(void)
{
	/*Faz disahble do watchdog*/
	PCA0MD = 0x00;
	/*Ativa o crossbar*/
	XBR1 = 0x40;
	/*Declaração e inicialização de variaveis*/
	int cont = 0;/*Variavel que ira contar o numero de vezes que o botão P0_6 é pressionado*/
	while(1){
		if(P0_6 == 0){/*Sempre que o botao é pressionado ira a 0*/
			while(P0_6 == 0);
			cont++;/*Incrementamos a variavel que faz a contagem do numero de pressões*/
		}
		/*Visto que o nosso programa devera iniciar mostrando o numero 3 e a variavel que faz a contagem é 
		inicializada a 0 (um numero par) sempre que o numero da variavel de contagem for um numero par iremos
		mostrar o numero 3, em caso contrario todos os leds do display se apagaram*/
		if((cont % 2) == 0){
			P2 = 0xB0;
		}
		else{
			P2= 0xFF;
		}
	}/*Fim do ciclo infinito do programa */	
}/*Fim da função main*/

/*NOTA:. O programa podera ser potencialmente melhorado para uma melhor otimização de memoria */
