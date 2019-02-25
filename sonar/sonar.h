/*
	Autor: Lucas Vieira de Jesus
	
	Cabeçalho contendo as definições localizadas em sonar.cpp
*/

#include <Arduino.h>

#ifndef SENSOR_H
#define SENSOR_H

#define METROS	0
#define CENTIMETROS	1

class sonar
{
	public:
		sonar();
		sonar(int echo, int trigger);
		~sonar();
		double obterDistancia(short UNIDADE_DE_MEDIDA);
		void configurarPinos();
		
	private:
		int pinoEcho, pinoTrigger;
		const double velocidade_som = 340.29;	// Velocidade do som em metros por segundo
};

#endif