/*
	Autor: Lucas Vieira de Jesus
	
	Código para gerenciar o sensor de presença conectado ao arduino
*/

#include "comum.h"
#include "sonar.h"

using namespace std;

// Para possíveis usos futuramente
sonar::sonar()
{
	
}

// Para possíveis usos futuramente
sonar::~sonar()
{
	
}

/* Inicializa as variáveis contendo os pinos utilizados pelo sonar */
sonar::sonar(int echo, int trigger)
{
	/* Salva as variáveis na própria classe, para possíveis utilizações no futuro */
	this->pinoEcho = echo;
	this->pinoTrigger = trigger;
}

/* Altera o modo de uso dos pinos echo e trigger */
void sonar::configurarPinos()
{
	pinMode(this->pinoEcho, INPUT);
	pinMode(this->pinoTrigger, OUTPUT);
}

/* Utiliza o sonar e retorna a distância do objeto localizado (em metros)*/
double sonar::obterDistancia(short UNIDADE_DE_MEDIDA)
{
	unsigned long duracao_us;	// Duração em microssegundos
	double duracao_ms;			// Duração em segundos
	double distancia_m;		// Distância em metros
	double distancia_cm;	// distância em centímetros
	
	// Primeiro deve-se enviar o pulso do sonar
	digitalWrite(pinoTrigger, LOW);
	delayMicroseconds(10);
	digitalWrite(pinoTrigger, HIGH);
	delayMicroseconds(10);
	digitalWrite(pinoTrigger, LOW);
	
	// Aguarda o som enviado retornar e armazena o tempo de espera na variável duracao
	duracao_us = pulseIn(pinoEcho, HIGH);
	
	// Converte a duração para milissegundos
	duracao_ms = (double)duracao_us / (pow(10,3));
	
	// A partir da duração, podemos saber a distância em que o objeto se encontra
	distancia_m = this->velocidade_som * (duracao_ms / pow(10,3));
	distancia_m /= 2.0;
	distancia_cm = distancia_m * pow(10,2);
	
	return (UNIDADE_DE_MEDIDA == METROS) ? distancia_m:distancia_cm;
}
