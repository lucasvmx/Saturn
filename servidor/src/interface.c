/*
	Código responsável por desenhar a interface gráfica e adicionar todos 
	os eventos de cada componente
	
	Autor: Lucas Vieira de Jesus
*/

#include "interface.h"
#include "servidor.h"
#include <windows.h>
#include <pthread.h>

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
	gtk_widget_set_size_request(janela, 640, 480);

	return janela;
}

static void on_botao_iniciar_server_clicked(GtkWidget *widget, gpointer data)
{
	WSADATA wsa_data;

	if(WSAStartup(MAKEWORD(2,2), &wsa_data) != 0)
	{
		GtkWidget *d = gtk_message_dialog_new(0, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "Erro ao inicializar winsock: %u", WSAGetLastError());
		gtk_dialog_run(G_OBJECT(d));
		gtk_widget_destroy(d);

		return;
	}

	SOCKET server_socket;
	rhost *host = NULL;

	server_socket = criar_socket_tcp();
	if(server_socket != INVALID_SOCKET)
	{
		if(vincular_servidor(server_socket, "127.0.0.1", 3000) != SOCKET_ERROR)
		{
			mudar_texto_buffer(buffer_texto, "Aguardando conexoes na porta 3000 ...\n");
			gtk_label_set_text(label, "Servidor: online");

			if(escutar_conexoes(server_socket) != SOCKET_ERROR)
			{
				if((host = aceitar_conexao(server_socket)) != NULL)
				{
					mudar_texto_buffer(buffer_texto, "Cliente conectado\n");
				}
			}
		}
	}
}

void mudar_texto_buffer(GtkTextBuffer *buffer, const gchar *texto)
{
	gtk_text_buffer_set_text(buffer, texto, strlen(texto));
}

