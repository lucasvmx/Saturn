/*
	Código responsável por desenhar a interface gráfica e adicionar todos 
	os eventos de cada componente
	
	Autor: Lucas Vieira de Jesus
*/

#include "interface.h"
#include "servidor.h"
#include "comum.h"
#include <stdio.h>
#include <windows.h>
#include <pthread.h>
#include <iphlpapi.h>

#define LARGURA		400
#define ALTURA		300

// Componentes da interface gráfica
GtkLabel *label = NULL;
GtkTextView *text_view = NULL;
GtkButton *botao_iniciar_servidor = NULL;
GtkApplicationWindow *janela = NULL;

// Outras variáveis globais
GtkTextBuffer *buffer_texto;

// Desenha a interface e adiciona os sinais necessários
GObject *desenhar_janela_principal(int argc, char **argv)
{
	GtkBuilder *builder = NULL;

	// Inicializa a biblioteca GTK
	gtk_init(&argc, &argv);
	
	// Lê as definições da interface a partir de um arquivo XML
	builder = gtk_builder_new_from_file("interface.glade");
	if(builder == NULL)
	{
		g_print( "Erro ao carregar janela principal\n");
		return NULL;
	}
	
#ifdef DEBUG
	g_print( "Interface carregada com sucesso");
#endif

	// Conecta todos os sinais de todos os componentes da interface gráfica e liga eles à uma função
	janela = (GtkApplicationWindow*)gtk_builder_get_object(builder, "janela_principal");
	g_signal_connect(janela, "destroy", G_CALLBACK(gtk_main_quit), NULL);

	// Botão de iniciar o servidor
	botao_iniciar_servidor = (GtkButton*)gtk_builder_get_object(builder, "botao_iniciar_server");
	g_signal_connect(G_OBJECT(botao_iniciar_servidor), "clicked", G_CALLBACK(on_botao_iniciar_server_clicked), NULL);
	
	// Label de informações resumidas
	label = (GtkLabel*)gtk_builder_get_object(builder, "label");
	gtk_label_set_text(label, "Servidor: offline");

	// Janela de logs
	text_view = (GtkTextView*)gtk_builder_get_object(builder, "janela_log");
	buffer_texto = (GtkTextBuffer*)gtk_builder_get_object(builder, "buffer_texto");

	// Exibe a interface e a redimensiona
	gtk_widget_show(janela);
	gtk_widget_set_size_request(janela, LARGURA, ALTURA);

	return janela;
}

/* 
	Essa função é executada assim que o usuário clica o botão "iniciar servidor"
*/
static void on_botao_iniciar_server_clicked(GtkWidget *widget, gpointer data)
{
	SOCKET server_socket;	// socket de rede do servidor
	struct host_remoto *host = NULL;		// ponteiro para a estrutura rhost
	pthread_t pThread;		// Utilizado como instrumento para manusear a thread
	WSADATA wsa_data;		// Estrutura utilizada pela função WSaStartup
	
	// Inicializa a biblioteca WinSock
	if(WSAStartup(MAKEWORD(2,2), &wsa_data) != 0)
	{
		GtkWidget *d = gtk_message_dialog_new(0, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "Erro ao inicializar winsock: %d", WSAGetLastError());
		gtk_dialog_run(G_OBJECT(d));
		gtk_widget_destroy(d);

		return;
	}

	// Cria um socket TCP para poder se comunicar com o arduino
	server_socket = criar_socket_tcp();
	if(server_socket != INVALID_SOCKET)
	{
		// Obter o primeiro endereço de IP do servidor
		IP_ADAPTER_INFO *adapt = NULL;
		ULONG tamanho = sizeof(IP_ADAPTER_INFO);
		ULONG result;

		adapt = (IP_ADAPTER_INFO*)malloc(sizeof(IP_ADAPTER_INFO));
		if(adapt == NULL)
		{
			g_print( "Falha ao alocar memória: %d\n", GetLastError());
			return;
		}

		// Obtém informações sobre os adaptadores de rede da máquina
		if((result = GetAdaptersInfo(adapt, &tamanho)) != NO_ERROR)
		{	
			if(result == ERROR_BUFFER_OVERFLOW)
			{
				free(adapt);
				
				adapt = (IP_ADAPTER_INFO*)malloc(tamanho);
				if(adapt == NULL)
				{
					g_print( "Memória insuficiente\n");
					return;
				}

				result = GetAdaptersInfo(adapt, &tamanho);
				if(result != NO_ERROR)
				{
					g_print( "Falhou: %d\n", result);
					return;
				}

				g_print( "Informações obtidas com sucesso\n");
			} else 
			{
				mudar_texto_buffer(buffer_texto, "Erro ao obter informações sobre os adaptadores de rede: %d\n", result);
				WSACleanup();
			
				if(adapt)
					free(adapt);

				return;
			}
		}
		
		// IP Escolhido para ser utilizado (padrão)
		const char *ip = "127.0.0.1";

		// Decide qual endereço será associado ao servidor
		while(adapt)
		{
			if(adapt->Type == MIB_IF_TYPE_ETHERNET || adapt->Type == IF_TYPE_IEEE80211)
			{
				ip = adapt->IpAddressList.IpAddress.String;
				break;
			}

			adapt = adapt->Next;
		}

		mudar_texto_buffer(buffer_texto, "IP: %s\n", ip);

		if(adapt)
			free(adapt);
			
		if(vincular_servidor(server_socket, ip, PORTA_PADRAO) != SOCKET_ERROR)
		{
			mudar_texto_buffer(buffer_texto, "Aguardando conexoes na porta 3000 ...\n");
			gtk_label_set_text(label, "Servidor: online");

			pthread_create(&pThread, NULL, execute, (void*)server_socket);
		}
	}
}

void mudar_texto_buffer(GtkTextBuffer *buffer, const gchar *texto, ...)
{
	va_list lista;
	char c_buffer[256];

	va_start(lista, texto);
	_vsnprintf(c_buffer, TAM(c_buffer), texto, lista);
	gtk_text_buffer_set_text(buffer, c_buffer, (gint)strlen(c_buffer));

	va_end(lista);
}

