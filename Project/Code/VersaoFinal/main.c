//Função main -> responsavel pelo fluxo do programa

//Header files
#include <c8051f380.h>
#include "macros.h"
#include "configs.h"
#include "states.h"
#include "aux.h"
#include "globalVariables.h"
#include "timer2.h"

void main (void)
{
	configs();
	
	//Declaração de variaveis locais
	unsigned char state;
	
	//Inicialização de variaveis globais e locais
	globalInits();
	state = IDLE;
	
	//Inicialização da contagem 
	timer2Start();
	while(1){
		//Implementação da maquina de estados
		switch(state){
			case IDLE:
				idle();
				if(P0_6 == 0){//Botão A pressionado
					while(P0_6 == 0);
					state = CONT;
				}//Fim de botão A pressionado
				if(P0_6 == 1 && P0_7 == 0){//Botão B pressionado
					while(P0_7 == 0);
					state = RESET;
				}//Fim de botão B pressionado

			break;
			case CONT:
				cont();
				if(P0_6 == 0){//Botão A pressionado
					while(P0_6 == 0);
					state = IDLE;
				}//Fim de botão A pressionado
				if(P0_6 == 1 && P0_7 == 0)//Botão B pressionado
					state = SPLIT;
				if(segs == MAXTIME)
					state = END;
			break;
			case SPLIT:
				split();
				while(P0_7 == 0);
				state = CONT;
			break;
			case RESET:
				reset();
				state = IDLE;
			break;
			case END:
				end();
				state = IDLE;
			break;
		}//Fim da maquina de estaoos
	}//Fim do ciclo infinito
}//Fim da função main
