/*
	Cabeçalho para o arquivo rede.cpp
	
	Autor: Lucas Vieira de Jesus
*/

#ifndef REDE_H
#define REDE_H

#define localhost		"127.0.0.1"
#define porta			3000

class rede 
{
	public:
		rede();
		rede(const char *ip);
		~rede();
		bool conectarRedeWifi(const char *nome_rede, const char *nome_senha, uint8_t *status);
		bool enviarLeituraSonar(double valor);
	private:
		const char *endereco_ip;
};

#endif