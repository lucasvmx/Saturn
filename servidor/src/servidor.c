/*
    Operações de rede do servidor

    Autor: Lucas Vieira de Jesus
*/

#include "servidor.h"
#include "comum.h"
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
    struct host_remoto *host = NULL;
    struct sockaddr_in end_socket;
    int size = sizeof(struct sockaddr_in);

    // Aceita a solicitação de conexão remota
    sock_resultado = accept(sock, (struct sockaddr*)&end_socket, &size);
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
    host = (struct host_remoto*)malloc(sizeof(struct host_remoto));
    if(host == NULL)
        return NULL;

    host->endereco_ip = inet_ntoa(end_socket.sin_addr);
    host->sock = sock_resultado;
    host->porta = end_socket.sin_port;

    return host; 
}

/* Recebe os dados de um host remoto */
int ler(SOCKET sock, void *buffer, int tam_buffer)
{
    return recv(sock, buffer, tam_buffer, 0);
}

/*
    É a função principal do servidor. Aqui toda a parte de comunicação com o arduino é realizada
*/
extern void execute(void *param)
{
    SOCKET p = (SOCKET)param;
    struct host_remoto *host = NULL;

    if(escutar_conexoes(p) != SOCKET_ERROR)
    {
        fprintf(stderr, "Aguardando conexões ...\n");
        host = aceitar_conexao(p);
        if(host != NULL)
        {
            fprintf(stderr, "Arduino conectado -> IP: %s -> porta %hu\n", host->endereco_ip, host->porta);

            // Libera a memória alocada para armazenar os dados do arduino conectado
            if(host)
                free(host);
        } else {
            fprintf(stderr, "Falha ao receber conexao: %d\n", WSAGetLastError());
        }
    } else {
        fprintf(stderr, "Falha ao aguardar conexões: %d\n", WSAGetLastError());
    }
}