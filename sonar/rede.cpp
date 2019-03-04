/*
	Código contendo todo o gerenciamento de rede do projeto
	
	Autor: Lucas Vieira de Jesus
*/

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include "comum.h"
#include "rede.h"

rede::rede()
{
	
}

rede::rede(const char *ip)
{
	this->endereco_ip = ip;
}

rede::~rede()
{
	
}

/*
	bool conectarRedeWifi(const char *r, const char *s, uint8_t *t)
	
	Tenta se conectar com a rede especificada e armazena o status
	da última tentativa no ponteiro t.
	
	Returna true em caso de sucesso e falso caso contrário
*/

bool rede::conectarRedeWifi(const char *nome_rede, const char *senha, uint8_t *status)
{
	int statusConexao;
	ESP8266WiFiClass wifi;
	ESP8266WiFiMulti multi;

	if(wifi.status() == WL_NO_SHIELD)
	{
		// Significa que a placa não possui shield wifi
		Serial.println( "A placa não dipõe de módulo WiFi");
		return false;
	}

	multi.addAP(nome_rede, senha);

	// Inicializa a conexão com a rede wifi especificada
	statusConexao = multi.run();
	if(statusConexao != WL_CONNECTED)
	{
		*status = statusConexao;
		return false;
	}
	
	*status = WL_CONNECTED;
	
	return true;
}

/*
	Envia para um host remoto o valor da leitura realizada pelo sonar
	retorna true em caso de sucesso e false em caso de erro
*/

bool rede::enviarLeituraSonar(double valor)
{
	WiFiClient arduino;
	int iConnect;
	
	// Tenta realizar a conexão com o host remoto, caso a conexão não tenha sido estabelecida anteriormente
	if(!arduino.connected()) 
	{
		// IMPORTANTE: o firewall poderá bloquear conexões pela porta 3000.
		iConnect = arduino.connect(endereco_ip, porta);

		if(!iConnect)
		{
			Serial.print( ".");
			return false;
		}

		// Ativa o flag para manter a conexão ativa
		arduino.keepAlive();
	}
	
	// Envia a distância lida pelo sonar com quatro casas decimais
	size_t bytesEnviados = arduino.printf( "%.2lf", valor);

	if(bytesEnviados <= 0)
		Serial.println( "Erro");

	return true;
}
