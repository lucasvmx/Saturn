/*
	Arquivo principal do firmware executado no arduino
	
	Autor: Lucas Vieira de Jesus
*/

#include "comum.h"
#include "sonar.h"
#include "rede.h"

sonar *ultrassom;			// variável para utilizar os métodos da classe sonar
rede *net;					// variável para utilizar os métodos da classe rede
const int echo = D7;		// pino echo do HRC-SR04
const int trigger = D6;		// pino trigger do HRC-SR04
double distancia;			// distância do objeto em relação ao sonar

void setup() 
{
	uint8_t status_conexao;
	char texto_erro[64];
	
	// Inicializa a comunicação serial
	Serial.begin(115200);
	
	// Inicializa as classes necessárias
	ultrassom = new sonar(echo, trigger);
	net = new rede();
	
	// Configura os pinos utilizados pelo sonar
	ultrassom->configurarPinos();
	
	// Realiza a conexão com a rede wifi especificada
	if(!net->conectarRedeWifi("Space", "#counterstrike", &status_conexao))
	{
		snprintf(texto_erro, tam(texto_erro), "Falha ao se conectar à rede wifi: %u", status_conexao); 
		Serial.println(texto_erro);
		for(;;);
	}
}

void loop() 
{
	// 
	distancia = ultrassom->obterDistancia(CENTIMETROS);
	delay(500);
	
	// envia a leitura para um servidor remoto
	while(net->enviarLeituraSonar(distancia) == false)
		delay(500);
}
