//Função main -> responsavel pelo fluxo do programa

//Header files
#include <c8051f380.h>
#include "macros.h"
#include "configs.h"

//Variaveis globais
unsigned int segs;

void main (void)
{
	configs();
	
	//Declaração de variaveis locais
	unsigned char state;
	
	//Inicialização de variaveis globais e locais
	segs = 0; 
	state = IDLE;
	
	while(1){
		//Implementação da maquina de estados
		switch(state){
			case IDLE:
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
				while(P0_7 == 0);
				state = CONT;
			break;
			case RESET:
				state = IDLE;
			break;
			case END:
				state = IDLE;
			break;
		}//Fim da maquina de estaoos
	}//Fim do ciclo infinito
}//Fim da função main
