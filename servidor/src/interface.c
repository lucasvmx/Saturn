/*
	Código responsável por desenhar a interface gráfica e adicionar todos 
	os eventos de cada componente
	
	Autor: Lucas Vieira de Jesus
*/

#include "interface.h"
#include <windows.h>

// Desenha a interface e adiciona os sinais necessários
GObject *desenhar_janela_principal(int argc, char **argv)
{
	GtkBuilder *builder = NULL;
	GObject *janela = NULL;
	GObject *botao_iniciar_servidor = NULL;
	
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
	janela = gtk_builder_get_object(builder, "janela_principal");
	g_signal_connect(janela, "destroy", G_CALLBACK(gtk_main_quit), NULL);

	botao_iniciar_servidor = gtk_builder_get_object(builder, "botao_iniciar_server");
	g_signal_connect(botao_iniciar_servidor, "clicked", G_CALLBACK(on_botao_iniciar_server_clicked), NULL);
	
	// Exibe a interface
	gtk_widget_show(janela);
	
	return janela;
}

static void on_botao_iniciar_server_clicked(GtkWidget *widget, gpointer data)
{
	if(WSAStartup(MAKEWORD(2,2), NULL) != 0)
	{
		gtk_message_dialog_new(0, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "Erro ao inicializar winsock: %u", WSAGetLastError());
		return;
	}
}

