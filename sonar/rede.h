/*
	Cabeçalho para o arquivo rede.cpp
	
	Autor: Lucas Vieira de Jesus
*/

#ifndef REDE_H
#define REDE_H

#define localhost	"127.0.0.1"

class rede 
{
	public:
		rede();
		~rede();
		bool conectarRedeWifi(char *nome_rede, const char *nome_senha, uint8_t *status);
		bool enviarLeituraSonar(double valor);
};

#endif