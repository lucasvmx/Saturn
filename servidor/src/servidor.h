/*
    Definições para servidor.c

    Autor: Lucas Vieira de Jesus
*/

#ifndef SERVIDOR_H
#define SERVIDOR_H

#include <winsock.h>

typedef struct host_remoto
{
    SOCKET sock;
    const char *endereco_ip;
} rhost;

extern SOCKET criar_socket_tcp();
extern int vincular_servidor(SOCKET sock, const char *IP, int porta);
extern int escutar_conexoes(SOCKET sock);
extern rhost *aceitar_conexao(SOCKET sock);
extern int ler(SOCKET sock, void *buffer, int tam_buffer);
extern void execute();

#endif