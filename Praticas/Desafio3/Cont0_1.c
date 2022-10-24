/*
Autores: Pedro Vale Ferreira (a95699) | Davide Oliveira Peixoto (a93949)
Data: 18 de outubro de 2022 | 01.35
Trabalho: Desafio 3
Versão: 0.1
Turno: PL2
Docente: Rui Machado
______________________________________________________________________________________________________________
Versão que utiliza o timer 0 e 1 (contador de 16 bits sem auto reload)
*/
#include <c8051f380.h>

void main(void)
{
	PCA0MD = 0x00;	//Desativa o watchdog
	XBR1 = 0x40;	//Ativa o crossbar 
	
	/*O oscilador interno de alta frequencia esta por defeito defenido para 48 MHz, colocando o registo 
	CLKSL igual a 010 estamos a dividir a frequencia do oscilador por 2, logo ficando com uma frequencia de 
	24 MHz*/
	CLKSEL |= 0x02; //Mascara para colocar os bits necessarios a 1
	CKCON |= 0x00; /*Divido o relogio do sistema por 12 -> relsultando numa frequencia de 2 000 000 Hz e um 
	periodo  de 0.0000005s <=> 0.5 us*/
	TMOD |= 0x01; //Modo de 16 bits 
	/*1 segundo equivale a 1 000 000 us, sendo que a cada pulso se terão passados 0.0000005s  a cada 50 000 
	ciclos terão se passado 0,025s logo necessitamos que o contador transborde 40 vezes para que se passe 1 
	segundo*/
	int reload = -50000;
	unsigned int contador = 0; /*Variavel auxiliar que ira contar o numero de vezes que o contador ira 
	transbordar*/
	//Inicializa o registo de paragem
	TH0 = reload >> 8; 
	TL0 = reload;
	TR0 = 1; //Inicializa a contagem 
	
	while(1){
		while(!TF0);/*Sendo este o registo responsavel pela ativação da flag enquanto ela não transbordar a 
		contagem continuar*/
		TF0 = 0;/*Volta a definir a flag a 0 para inciar um novo ciclo de contagem*/
		contador++;/*Incrementar a variavel auxiliar de contagem de vezes que a flag foi ativada*/
		if(contador == 40){/*40 vezes é o numero de vezes que necessitam de passar para termos esperado um
		segundo*/
			contador = 0; /*Voltar a colocar a variavel a 0 para obtermos nova contagem*/
			P2_7 = !P2_7;/*Complementar o pino do porto, isto é caso o ponto esteja acesso ira se apagar e
			vice-versa*/
		}
		//Voltar a incializar as contagens
		TH0 = reload >> 8;
		TL0 = reload;	
	}
}