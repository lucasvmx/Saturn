/*
    Operações de rede do servidor

    Autor: Lucas Vieira de Jesus
*/

#include "servidor.h"
#include <stdio.h>
#include <Windows.h>

/*
    Cria um socket TCP para envio e recepção de dados
*/
SOCKET criar_socket_tcp()
{
    SOCKET tcp_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    return tcp_socket;
}

/*
    Inicia o vínculo do servidor com um endereço de IP local. Retorna 0 em caso de sucesso
*/
int vincular_servidor(SOCKET sock, const char *IP, int porta)
{
    struct sockaddr_in endereco_sock;
    
    // Configura a estrutura
    endereco_sock.sin_port = htons((u_short)porta);
    endereco_sock.sin_family = AF_INET;
    endereco_sock.sin_addr.S_un.S_addr = inet_addr(IP);

    // Inicia o vínculo
    return bind(sock,&endereco_sock, sizeof(endereco_sock));
}

/*
    Inicia a escuta de conexões
*/
int escutar_conexoes(SOCKET sock)
{
    return listen(sock, SOMAXCONN);
}

struct host_remoto *aceitar_conexao(SOCKET sock)
{
    SOCKET sock_resultado;
    rhost host;
    struct sockaddr_in end_socket;

    // Aceita a solicitação de conexão remota
    sock_resultado = accept(sock, 0, 0);
    if(sock_resultado == INVALID_SOCKET) 
    {
        fprintf(stderr, "Falha ao aceitar conexão remota: %u\n", WSAGetLastError());
        return NULL;
    }

    // Ativa o modo keep-alive no socket
    int optval = 1;
    int optlen = sizeof(optval);

    if(setsockopt(sock_resultado, SOL_SOCKET, SO_KEEPALIVE, &optval, optlen) < 0)
    {
        fprintf(stderr, "Falha ao ativar o modo keep-alive: %u\n", WSAGetLastError());
        shutdown(sock, 2);
        return NULL;
    }

    // Armazena as informações do host remoto na estrutura
    host.endereco_ip = "127.0.0.1";
    host.sock = sock_resultado;

    return &host; 
}

int ler(SOCKET sock, void *buffer, int tam_buffer)
{
    return recv(sock, buffer, tam_buffer, 0);
}