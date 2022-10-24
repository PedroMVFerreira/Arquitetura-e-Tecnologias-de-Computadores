/*
Autores: Pedro Vale Ferreira (a95699) | Davide Oliveira Peixoto (a93949)
Data: 19 de outubro de 2022 
Trabalho: Versão 1 -> Projeto
Versão: 1.4
Turno: PL2
Docente: Rui Machado
_______________________________________________________________________________________________________________
Implemetação de um contador de tempo incrementado de segundo a segundo, mostrando os seus valores no display de
7 segmentos 

O contador e respetiva mostragem encontram - se a funcionar perfeitamente tendo passado os testes 19 de outrubro
de 2022 (Pedro Vale Ferreira)
*/

#include <c8051f380.h>

void main(void)
{
	PCA0MD = 0x00;	/*Desativa o watchdog*/
	XBR1 = 0x40;	/*Ativa o crossbar*/ 
	
	/*Array que irá guardar a forma de demonstar todos os digitos necessarios no display de 7 segmentos*/
	char digitsOnP2[10] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x98};
	
	/*Declaração de variaveis*/
	unsigned int segundos = 0;	/*Variavel responsavel por contador os segundos passados desde a ativação do 
	contador*/
	unsigned int contadorFlagSegundos = 0; /*Variavel responsavel por contar o numero de vezes que a flag é 
	ativada alterando o nmumero de segundos passados*/
	int reload = -50000; /*Faltam 50000 ciclos para 0xFFFF, em cada transbordo terão se passado 
	0.025 segundos*/
	unsigned int segundosCopy, digito1, digito2, digito3, digito4, digito5; /*Variaveis auxiliares responsavel 
	por guaradar os digitos de cada segundo*/
	/*Fim da declaração de variaveis*/
	
	/*Configuração do timer2 no modo de 16 bits com autoreload utilizando polling para o incremento da contagem*/
	/*Frequencia do OHF -> 48 MHz */
	CLKSEL |= 0x02; /*Divide a frequencia do oscilador por 2 -> 24 MHz*/
	/* Devido as definições predfinidas o timer 2 esta a ser alimentado por um relogio com uma frequencia de
	2 Mhz e um periodo de 0.5 us*/
	/*Fim da configuração do timer 2*/
	
	/*Inicialização dos registos de paragem*/
	TMR2RLL = reload; /*Passagem do bit menos significativo de reload*/
	TMR2RLH = reload >> 8; /*Passagem do bit mais significativo de reload utilizando um shift de 8 bit*/
	/*Fim da inicialização dos registos de paragem*/
	
	TR2 = 1;	/*Inicialização do timer 2*/
	
	while(1){
		while(!TF2H); /*Enquanto a flag não for ativada ficamos a espera que a mesma o seja*/
		TF2H = 0; /*A quando da ativação da flag iremos limpar a mesma*/
		contadorFlagSegundos++;/*Incremento da variavel auxiliar responsavel por aumentar o numero de segundos
		que se passram desde a ativação do timer*/
		if(contadorFlagSegundos == 40){/*Caso a variavel auxliar responsavel por aumentar o numero de segundos 
		tenha chegado a 40 terá se passado 1 segundo e iremos aumentar a variavel que guarda os segundos que se 
		passaram até ao momento, limpando a variavel auxiliar para que um novo segundo possa ser contado*/
			contadorFlagSegundos = 0;
			segundos++;
		}/*Fim do incremento da variavel de segundo*/
		/*Iremos mostar 1 digito por segundo nos 10 primeiros segundos*/
		if(segundos >= 0 && segundos <= 9){
			/*A variavel auxiliar responsavel pelo tempo que cada digito é mostrado não será necessaria, pois
			em numeros de 1 digito podemos utilizar todo o segundo para o mostrar*/
			P2 = digitsOnP2[segundos];
		}/*Fim da mostragem de numeros com apenas 1 digito*/
		/*Iremos mostar 2 digitos por segundo -> 0.5 segundos cada*/
		if(segundos >= 10 && segundos <=99){
			segundosCopy = segundos;/*Copia da variavel segundos para podermos manipular sem perder os valor dos
			segundos que se passaram até ao momento*/
			digito2 = segundosCopy % 10;/*Passar o digito2 do numero de segundos para a variavel que o ira 
			guardar*/
			segundosCopy = segundosCopy / 10;/*Retirar o digito que já foi passado da variavel de backup de 
			segundos*/
			digito1 = segundosCopy % 10;/*Passar o digito1 do numero de segundos para a variavel que o ira 
			guardar*/
			/*Iremos mostar o primerio numero durante 0.5 segundos e o segundo pelo mesmo tempo*/
			if(contadorFlagSegundos <= 20){
				P2 = digitsOnP2[digito1];
			}
			if(contadorFlagSegundos >= 20 && contadorFlagSegundos <= 40){
				P2 = digitsOnP2[digito2];/*Mostrar o segundo digito dos segundos*/
			}
		}/*Fim da mostragem de numeros com 2 digitos*/
		/*Mostragem dos numeros com 3 digitos -> O digito mais significativo ira estar visivel por 0.2 segundos
		enquanto que os digitos menos significativos estarão visiveis por 0.4*/
		if(segundos >= 100 && segundos <= 999){
			segundosCopy = segundos;/*Copia da variavel segundos para podermos manipular sem perder os valor dos
			segundos que se passaram até ao momento*/
			digito3 = segundosCopy % 10;/*Passar o digito3 do numero de segundos para a variavel que o ira 
			guardar*/
			segundosCopy = segundosCopy / 10;/*Retirar o digito que já foi passado da variavel de backup de 
			segundos*/
			digito2 = segundosCopy % 10;/*Passar o digito2 do numero de segundos para a variavel que o ira 
			guardar*/
			segundosCopy = segundosCopy / 10;/*Retirar o digito que já foi passado da variavel de backup de 
			segundos*/
			digito1 = segundosCopy % 10;/*Passar o digito1 do numero de segundos para a variavel que o ira 
			guardar*/
			if(contadorFlagSegundos <= 8)
				P2 = digitsOnP2[digito1];
			if(contadorFlagSegundos >= 9 && contadorFlagSegundos <= 24)
				P2 = digitsOnP2[digito2];
			if(contadorFlagSegundos >= 25 && contadorFlagSegundos <= 40)
				P2 = digitsOnP2[digito3];
		}/*Fim da mostragem de numeros com 3 digitos*/
		/*Mostragem de numeros com 4 digitos -> 0.25 segundos cada digito*/
		if(segundos >= 1000 && segundos <= 9999){
			segundosCopy = segundos;/*Copia da variavel segundos para podermos manipular sem perder os valor dos
			segundos que se passaram até ao momento*/
			digito4 = segundosCopy % 10;/*Passar o digito4 do numero de segundos para a variavel que o ira 
			guardar*/
			segundosCopy = segundosCopy / 10;/*Retirar o digito que já foi passado da variavel de backup de 
			segundos*/
			digito3 = segundosCopy % 10;/*Passar o digito3 do numero de segundos para a variavel que o ira 
			guardar*/
			segundosCopy = segundosCopy / 10;/*Retirar o digito que já foi passado da variavel de backup de 
			segundos*/
			digito2 = segundosCopy % 10;/*Passar o digito2 do numero de segundos para a variavel que o ira 
			guardar*/
			segundosCopy = segundosCopy / 10;/*Retirar o digito que já foi passado da variavel de backup de 
			segundos*/
			digito1 = segundosCopy % 10;/*Passar o digito1 do numero de segundos para a variavel que o ira 
			guardar*/
			if(contadorFlagSegundos <= 10)
				P2 = digitsOnP2[digito1];
			if(contadorFlagSegundos >= 11 && contadorFlagSegundos <= 20)
				P2 = digitsOnP2[digito2];
			if(contadorFlagSegundos >= 21 && contadorFlagSegundos <= 30)
				P2 = digitsOnP2[digito3];
			if(contadorFlagSegundos >= 31 && contadorFlagSegundos <= 40)
				P2 = digitsOnP2[digito4];
		}/*Fim da mostragem de numeros com 4 digitos*/
		/*Mostragem de numeros com 5 digitos -> 0.20 segundos cada digito*/
		if(segundos >= 10000 && segundos <= 43200){
			segundosCopy = segundos;/*Copia da variavel segundos para podermos manipular sem perder os valor dos
			segundos que se passaram até ao momento*/
			digito5 = segundosCopy % 10;/*Passar o digito5 do numero de segundos para a variavel que o ira 
			guardar*/
			segundosCopy = segundosCopy / 10;/*Retirar o digito que já foi passado da variavel de backup de 
			segundos*/
			digito4 = segundosCopy % 10;/*Passar o digito4 do numero de segundos para a variavel que o ira 
			guardar*/
			segundosCopy = segundosCopy / 10;/*Retirar o digito que já foi passado da variavel de backup de 
			segundos*/
			digito3 = segundosCopy % 10;/*Passar o digito3 do numero de segundos para a variavel que o ira 
			guardar*/
			segundosCopy = segundosCopy / 10;/*Retirar o digito que já foi passado da variavel de backup de 
			segundos*/
			digito2 = segundosCopy % 10;/*Passar o digito2 do numero de segundos para a variavel que o ira 
			guardar*/
			segundosCopy = segundosCopy / 10;/*Retirar o digito que já foi passado da variavel de backup de 
			segundos*/
			digito1 = segundosCopy % 10;/*Passar o digito1 do numero de segundos para a variavel que o ira 
			guardar*/
			if(contadorFlagSegundos <= 8)
				P2 = digitsOnP2[digito1];
			if(contadorFlagSegundos >= 9 && contadorFlagSegundos <= 16)
				P2 = digitsOnP2[digito2];
			if(contadorFlagSegundos >= 17 && contadorFlagSegundos <= 24)
				P2 = digitsOnP2[digito3];
			if(contadorFlagSegundos >= 25 && contadorFlagSegundos <= 32)
				P2 = digitsOnP2[digito4];
			if(contadorFlagSegundos >= 26 && contadorFlagSegundos <= 40)
				P2 = digitsOnP2[digito5];
		}/*Fim da mostragem de numeros com 5 digitos*/
	}/*Fim do ciclo infinito*/
}