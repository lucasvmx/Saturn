/*
	Código contendo todo o gerenciamento de rede do projeto
	
	Autor: Lucas Vieira de Jesus
*/

#include <ESP8266WiFi.h>
#include "comum.h"
#include "rede.h"

rede::rede()
{
	
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

bool rede::conectarRedeWifi(char *nome_rede, const char *senha, uint8_t *status)
{
	int statusConexao;
	
	// Coloca o ESP8266 em modo de cliente wifi
	WiFi.mode(WIFI_STA);
	
	// Inicializa a conexão com a rede wifi especificada
	statusConexao = WiFi.begin(nome_rede, senha);
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
	const uint16_t porta = 3000;
	
	// Tenta realizar a conexão com o host remoto, caso a conexão não tenha sido estabelecida anteriormente
	if(!arduino.connected()) 
	{
		if(!arduino.connect("127.0.0.1", porta))
			return false;
	}
	
	// Envia a distância lida pelo sonar com quatro casas decimais
	arduino.println(String(valor, 4));
	
	return true;
}
