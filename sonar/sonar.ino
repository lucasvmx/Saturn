/*
	Arquivo principal do firmware executado no arduino
	
	Autor: Lucas Vieira de Jesus
*/

#include "comum.h"
#include "sonar.h"
#include "rede.h"
#include <ESP8266WiFi.h>

sonar *ultrassom;			// variável para utilizar os métodos da classe sonar
rede *net;					// variável para utilizar os métodos da classe rede
const int echo = D7;		// pino echo do HRC-SR04
const int trigger = D6;		// pino trigger do HRC-SR04
double distancia;			// distância do objeto em relação ao sonar

void aguardarDadosNaPortaSerial()
{
	while(!Serial.available());
}

void piscarLed()
{
	digitalWrite(LED_BUILTIN, LOW);
	delay(500);
	digitalWrite(LED_BUILTIN, HIGH);
	delay(500);
}

void setup() 
{
	uint8_t status_conexao;		// utilizada para verificar 
	String msg;
	String IP;					// Endereço de IP do servidor
	String redeWifi;			// Nome da rede Wifi
	String senhaWifi;			// Senha da rede wifi

	// Inicializa a comunicação serial
	Serial.begin(115200);

	//Recebe o endereço de IP e realiza a sua validação (parcial)
	for(;;)
	{
		Serial.println( "Insira o endereço de IP: ");
		aguardarDadosNaPortaSerial(); // aguarda até que o usuário insira algum dado

		IP = Serial.readString();
		IP.setCharAt(IP.length() - 1, '\0'); // Remove o '\n' do final da string

		Serial.println( "IP inserido: " + IP);

		// Tenta validar o IP da melhor forma possível
		if(IPAddress::isValid(IP) == false)
			continue;

		// Se chegarmos até aqui, então o IP está em um formato aceitável
		Serial.println( "Insira o nome da rede wifi: ");
		aguardarDadosNaPortaSerial();
		redeWifi = Serial.readString();
		Serial.println( "Insira a senha da rede: ");
		aguardarDadosNaPortaSerial();
		senhaWifi = Serial.readString();

		break;
	}

	pinMode(LED_BUILTIN, OUTPUT);

	// Inicializa as classes necessárias
	Serial.println( "IP do servidor: " + IP);

	ultrassom = new sonar(echo, trigger);
	net = new rede(IP.c_str());
	
	// Configura os pinos utilizados pelo sonar
	Serial.println( "Configurando pinos ...");
	ultrassom->configurarPinos();
	
	// Realiza a conexão com a rede wifi especificada
	msg = "Conectando-se com a rede: " + redeWifi;
	Serial.println(msg);

	while(!net->conectarRedeWifi(redeWifi.c_str(), senhaWifi.c_str(), &status_conexao))
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
