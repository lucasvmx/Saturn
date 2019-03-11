/*
    Operações de rede do servidor

    Autor: Lucas Vieira de Jesus
*/

#include "servidor.h"
#include "comum.h"
#include <stdio.h>
#include <Windows.h>
#include <sqlite3.h>
#include <io.h>
#include <stdbool.h>
#include <pthread.h>

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
    
    //- Configura a estrutura
    endereco_sock.sin_port = htons((u_short)porta);
    endereco_sock.sin_family = AF_INET;
    endereco_sock.sin_addr.S_un.S_addr = inet_addr(IP);

    // Inicia o vínculo
    return bind(sock, (struct sockaddr*)&endereco_sock, sizeof(endereco_sock));
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

    // Ativa o flag keepalive
    int optval = 1;

    if(setsockopt(sock_resultado, SOL_SOCKET, SO_REUSEADDR, (const char*)&optval, sizeof(int)) == -1)
    {
        fprintf(stderr, "Falha ao configurar keepalive: %d\n", WSAGetLastError());
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
extern void *executar(void *param)
{
    SOCKET p = (SOCKET)param;
    struct host_remoto *host = NULL;
    char nome_banco_dados[64];
    char instrucao_sql[256];
    char buffer[256];
    int sqlite_status;
    int flags_abertura;
    char *errMsg = 0;
    SYSTEMTIME *systime = NULL;
    
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    
    // Verificar se já existe um banco de dados em disco 
    systime = (SYSTEMTIME*)malloc(sizeof(SYSTEMTIME));
    if(systime == NULL)
    {
        fprintf(stderr, "Erro ao reservar memoria: %lu\n", GetLastError());
        return NULL;
    }

    GetLocalTime(systime);
    
    _snprintf(nome_banco_dados, TAM(nome_banco_dados), "%s\\leitura_%02d%02d%02d.sdb", PASTA_DATABASE, systime->wDay, systime->wMonth, systime->wYear);
    
    // Libera a memória alocada
    free(systime);

    if(access(nome_banco_dados, 0) == -1)
    {
        // O banco de dados não existe e precisa ser criado
        flags_abertura = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;   
    } else {
        flags_abertura = SQLITE_OPEN_READWRITE;
    }

    sqlite_status = sqlite3_open_v2(nome_banco_dados, &banco, flags_abertura, NULL);
    if(sqlite_status != SQLITE_OK) 
    {
        fprintf(stderr, "Falha ao abrir banco de dados: %s\n", sqlite3_errmsg(banco));
        return NULL;
    }

    /*
        Formato do banco de dados
        _______________________________________
        |  distancia_cm  |       timestamp     |
        |      15.40     | 25/04/2018 15:32:45 |
        ---------------------------------------
    */

    _snprintf(instrucao_sql, TAM(instrucao_sql), "CREATE TABLE IF NOT EXISTS %s (%s REAL, %s TEXT);", 
    NOME_TABELA, 
    COLUNA_DIST, 
    COLUNA_TSTAMP);
    
#ifdef DEBUG
    fprintf(stderr, "[SQL] %s\n", instrucao_sql);
#endif

    if(sqlite3_exec(banco, instrucao_sql, NULL, NULL, &errMsg) != SQLITE_OK)
    {
        fprintf(stderr, "Falha ao executar instrucao SQL: %s\n", errMsg);

        sqlite3_close_v2(banco);
        return NULL;
    }

    // Ligar o servidor
    if(escutar_conexoes(p) != SOCKET_ERROR)
    {
        fprintf(stderr, "Aguardando conexoes ...\n");

        interrompido = false;
        interromper = false;
        
        while(!interromper)
        {
            host = aceitar_conexao(p);
            if(host != NULL)
            {
                fprintf(stderr, "Arduino conectado -> IP: %s -> porta %hu\n", host->endereco_ip, host->porta);

                // Com o modo keepalive já ativado, só nos resta receber e processar os dados

                int bytesRec, totalBytes = 0;

                do {
                    bytesRec = ler(host->sock, buffer, sizeof(buffer));
                    if(bytesRec < 0)
                    {
                        fprintf(stderr, "Falha ao receber dados: %d\n", WSAGetLastError());
                        break;
                    }

                    totalBytes += bytesRec;
                } while(bytesRec > 0);

                if(totalBytes > 0)
                {
                    // Decodifica os dados recebidos e formata-os para que sejam inseridos no banco de dados
                    double distancia = strtod(buffer, NULL);
                    char *timestamp = compilar_timestamp();

                    if(timestamp == NULL)
                    {
                        fprintf(stderr, "Erro fatal: falha ao obter timestamp. O programa nao podera continuar\n");
                        Sleep(5000);
                        exit(-1);
                    }  

                    char buffer_distancia[6];
                    int pos = 0;

                    snprintf(buffer_distancia, TAM(buffer_distancia), "%.2lf", distancia);
                    
                    // Esta operação é necessária, pois existe o risco de que haja vírgula no lugar do ponto ao formatar o número flutuante
                    while(buffer_distancia[pos])
                    {
                        if(buffer_distancia[pos] == ',')
                            buffer_distancia[pos] = '.';

                        pos++;
                    }

                    // Insere a distância lida no banco de dados
                    snprintf(instrucao_sql, TAM(instrucao_sql), "INSERT INTO %s (%s,%s) VALUES (%s,'%s');",
                    NOME_TABELA, 
                    COLUNA_DIST, 
                    COLUNA_TSTAMP, 
                    buffer_distancia, 
                    timestamp);

                    // É necessário liberar a memória alocada
                    free(timestamp);
#ifdef DEBUG 
                    fprintf(stderr, "[SQL] %s\n", instrucao_sql);
#endif
                    if(sqlite3_exec(banco, instrucao_sql, NULL, NULL, &errMsg) != SQLITE_OK)
                        fprintf(stderr, "Falha ao passar informacoes para o banco de dados: %s\n", errMsg);
                }

                fprintf(stderr, "Conexao terminada\n");
                
                // Libera a memória alocada para armazenar os dados do arduino conectado
                if(host)
                    free(host);
            } else {
                fprintf(stderr, "Falha ao receber conexao: %d\n", WSAGetLastError());
            }
        }

        interrompido = true;
        interromper = false;

#ifdef DEBUG
        fprintf(stderr, "Thread interrompida\n");
#endif

        // fecha o banco de dados
        sqlite3_close_v2(banco);
    } else {
        fprintf(stderr, "Falha ao aguardar conexões: %d\n", WSAGetLastError());
    }

    return NULL;
}

// Cria um timestamp e retorna em formato de string. O formato adotado é: dd/mm/yyyy hh:mm:ss
char *compilar_timestamp()
{
    SYSTEMTIME *timestamp = NULL;
    char *buffer_tempo = NULL;
    size_t tamanho_timestamp = 22;

    // Aloca memória para a string e para a estrutura que armazena o timestamp
    timestamp = (SYSTEMTIME*)malloc(sizeof(SYSTEMTIME));
    if(!timestamp)
        return NULL;
        
    buffer_tempo = (char*)malloc(tamanho_timestamp);
    if(!buffer_tempo)
        return NULL;

    // Obtém a hora local
    GetLocalTime(timestamp);

    //Formata o timestamp no padrão ISO 8601
    snprintf(buffer_tempo, tamanho_timestamp, "%02d/%02d/%02d %02d:%02d:%02d", 
    timestamp->wDay,
    timestamp->wMonth,
    timestamp->wYear,
    timestamp->wHour,
    timestamp->wMinute,
    timestamp->wSecond);

    free(timestamp);

    // ALERTA: esse ponteiro precisa ser liberado após ser utilizado
    return buffer_tempo;
}

// Solicita a interrupcao do servidor
void solicitar_interrupcao()
{
    // Se o servidor estiver aguardando conexões, não será possível interromper a thread de imediato
    if(!interrompido)
        interromper = true;
}