/*
    Definições para servidor.c

    Autor: Lucas Vieira de Jesus
*/

#ifndef SERVIDOR_H
#define SERVIDOR_H

#include <winsock.h>
#include <stdbool.h>
#include <sqlite3.h>

#define PORTA_PADRAO    3000

struct host_remoto
{
    SOCKET sock;
    const char *endereco_ip;
    unsigned short porta;
};

struct pacote_arduino
{
    double distancia_lida;      // Distância lida pelo sensor
    char timestamp[22];         // Timestamp de quando a distância foi lida
};

extern SOCKET criar_socket_tcp();
extern int vincular_servidor(SOCKET sock, const char *IP, int porta);
extern int escutar_conexoes(SOCKET sock);
extern struct host_remoto *aceitar_conexao(SOCKET sock);
extern int ler(SOCKET sock, void *buffer, int tam_buffer);
extern void *executar();
extern void solicitar_interrupcao();
extern char *compilar_timestamp();

// Para uso pela interface gráfica e pela thread
extern bool interromper;
extern bool interrompido;
extern sqlite3 *banco;

// Para uso pelo banco de dados
static const char *NOME_TABELA = "dados_sonar";
static const char *COLUNA_DIST = "distancia_cm";
static const char *COLUNA_TSTAMP = "timestamp";

#endif