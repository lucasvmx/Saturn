/*
	Arquivo principal do firmware executado no arduino
	
	Autor: Lucas Vieira de Jesus
*/

#include "comum.h"
#include "sonar.h"
#include "rede.h"
#include <ESP8266WiFi.h>

const char *nome_rede = "Space";
const char *senha_rede = "#counterstrike";

sonar *ultrassom;			// variável para utilizar os métodos da classe sonar
rede *net;					// variável para utilizar os métodos da classe rede
const int echo = D7;		// pino echo do HRC-SR04
const int trigger = D6;		// pino trigger do HRC-SR04
double distancia;			// distância do objeto em relação ao sonar

void piscarLed()
{
	digitalWrite(LED_BUILTIN, LOW);
	delay(500);
	digitalWrite(LED_BUILTIN, HIGH);
	delay(500);
}

void setup() 
{
	uint8_t status_conexao;
	String msg;

	// Inicializa a comunicação serial
	Serial.begin(115200);
	
	pinMode(LED_BUILTIN, OUTPUT);

	// Inicializa as classes necessárias
	Serial.println( "Inicializando ...");
	ultrassom = new sonar(echo, trigger);
	net = new rede();
	
	// Configura os pinos utilizados pelo sonar
	Serial.println( "Configurando pinos ...");
	ultrassom->configurarPinos();
	
	// Realiza a conexão com a rede wifi especificada
	msg = "Conectando-se com a rede: " + String(nome_rede);
	Serial.println(msg);

	while(!net->conectarRedeWifi(nome_rede, senha_rede, &status_conexao))
	{
		piscarLed();
		Serial.print( "." );
	}
	
	Serial.println( " [CONECTADO]");
	digitalWrite(LED_BUILTIN, LOW);

	Serial.println( "IP: " + WiFi.localIP().toString());
}

void loop() 
{
	distancia = ultrassom->obterDistancia(CENTIMETROS);
	delay(500);
	
	Serial.print( "Enviando dados do sonar");

	// envia a leitura para um servidor remoto
	while(!net->enviarLeituraSonar(distancia))
		delay(500);

	Serial.println( " [OK]");
}
