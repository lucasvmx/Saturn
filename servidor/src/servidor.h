/*
    Definições para servidor.c

    Autor: Lucas Vieira de Jesus
*/

#ifndef SERVIDOR_H
#define SERVIDOR_H

#include <winsock.h>

#define PORTA_PADRAO    3000

typedef struct host_remoto
{
    SOCKET sock;
    const char *endereco_ip;
    unsigned short porta;
};

extern SOCKET criar_socket_tcp();
extern int vincular_servidor(SOCKET sock, const char *IP, int porta);
extern int escutar_conexoes(SOCKET sock);
extern struct host_remoto *aceitar_conexao(SOCKET sock);
extern int ler(SOCKET sock, void *buffer, int tam_buffer);
extern void execute();

#endif