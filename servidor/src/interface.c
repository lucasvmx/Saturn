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

// Utilizados pela thread do servidor
bool interromper;
bool interrompido;
sqlite3 *banco = NULL;

// Componentes da interface gráfica principal
GtkLabel *label = NULL;
GtkTextView *text_view = NULL;
GtkButton *botao_iniciar_servidor = NULL;
GtkApplicationWindow *janela = NULL;
GtkButton *botao_consultar_registros_salvos = NULL;

// Componentes da janela de registros
static GtkWindow *janela_registros = NULL;
GtkTextView *texto_registro = NULL;
GtkButton *botao_consultar = NULL;
GtkButton *botao_escolher = NULL;
GtkRadioButton *radio_button_data_hora = NULL;
GtkRadioButton *radio_button_distancia = NULL;

// Outras variáveis globais
GtkTextBuffer *buffer_texto = NULL, *buffer_texto_registro = NULL;

const char *caminho_banco_dados = NULL; 	// Armazena o banco de dados que o usuário deseja consultar
static int num_registros = 0;				// armazena a quantidade de registros localizados no banco de dados

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
	g_print( "Interface carregada com sucesso\n");
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

	// Botão de consultar registros
	botao_consultar_registros_salvos = (GtkButton*)gtk_builder_get_object(builder, "botao_consultar_registros_salvos");
	g_signal_connect(botao_consultar_registros_salvos, "clicked", on_botao_consultar_registros_salvos_clicked, NULL);
	
	// Janela de logs
	text_view = (GtkTextView*)gtk_builder_get_object(builder, "janela_log");
	buffer_texto = (GtkTextBuffer*)gtk_builder_get_object(builder, "buffer_texto");

	// Exibe a interface e a redimensiona
	gtk_widget_show(janela);
	gtk_widget_set_size_request(janela, LARGURA, ALTURA);
	
	// Muda o título da janela
	gtk_window_set_title(janela, "Saturn - servidor");
	
	// Inicializa as variáveis da thread
	interrompido = true;
	interromper = false;
	
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
	GtkWidget *msg_box = NULL;

#ifdef DEBUG
	g_print( "Interrompido: %s\nInterromper: %s\n", (interrompido) ? "true":"false", (interromper) ? "true":"false");
#endif

	if(!interrompido)
	{
		solicitar_interrupcao();
		Sleep(1000);

		// Cancela a thread de forma grosseira, caso o solicitar_interrupcao() não tenha efeito rápido
		pthread_cancel(pThread);

		ui_print(buffer_texto, "Servidor interrompido\n" );
		gtk_button_set_label(botao_iniciar_servidor, "Iniciar servidor" );

		// WARNING: essa variável é global e requer extrema cautela na sua utilização
		if(banco != NULL)
			sqlite3_close_v2(banco);

		return;
	}

	// Inicializa a biblioteca WinSock
	if(WSAStartup(MAKEWORD(2,2), &wsa_data) != 0)
	{
		msg_box = gtk_message_dialog_new(widget, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "Erro ao inicializar winsock: %d", WSAGetLastError());
		gtk_dialog_run(G_OBJECT(msg_box));
		gtk_widget_destroy(msg_box);

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
			/* 
				Essa função pode falhar caso o tamanho especificado não seja o suficiente
				Nesse caso, o que temos que fazer é realocar memória necessária novamente e chamar a função pela segunda vez
			*/
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
			} else 
			{
				ui_print(buffer_texto, "Erro ao obter informações sobre os adaptadores de rede: %d\n", result);
				WSACleanup();
			
				if(adapt)
					free(adapt);

				return;
			}
		}
		
		// IP Escolhido para ser utilizado (padrão)
		const char *ip = "127.0.0.1";

		// Decide qual endereço será associado ao servidor
		IP_ADAPTER_INFO *info = adapt;

		while(info)
		{
#ifdef DEBUG
			g_print( "IP: %s\n", info->IpAddressList.IpAddress.String);
#endif
			if(strncmp(info->IpAddressList.IpAddress.String, "0.0.0.0", 7) == 0)
			{
				info = info->Next;
			}
			else if(info->Type == IF_TYPE_IEEE80211 || info->Type == MIB_IF_TYPE_ETHERNET)
			{
				ip = info->IpAddressList.IpAddress.String;
				break;
			}
		}

		if(adapt)
			free(adapt);
			
		// Configura o servidor para que ele possa ter capacidade de receber conexões TCP
		if(vincular_servidor(server_socket, ip, PORTA_PADRAO) != SOCKET_ERROR)
		{
			ui_print(buffer_texto, "Aguardando conexoes -> %s:%d\n", ip, PORTA_PADRAO);
			gtk_label_set_text(label, "Servidor: online");

			int iThread = pthread_create(&pThread, NULL, executar, (void*)server_socket);	
			if(iThread != 0)
				ui_print(buffer_texto, "Não foi possível iniciar o servidor: Erro %d\n", iThread);
			else
				gtk_button_set_label(botao_iniciar_servidor, "Parar servidor");
		}
	}
}

static void on_botao_consultar_registros_salvos_clicked(GtkWidget *widget, gpointer data)
{
	if(janela_registros == NULL)
	{
		desenhar_janela_registros();
		gtk_widget_show(GTK_WIDGET(janela_registros));
	}
	else if(!gtk_widget_is_visible(GTK_WIDGET(janela_registros)))
	{
		/* 
			Esse if poderia ficar juntamente com o de cima, mas para isso teríamos que correr o risco de
			passar um argumento nulo para a função gtk_widget_is_visible() ...
		*/

		desenhar_janela_registros();
		gtk_widget_show(GTK_WIDGET(janela_registros));
	}	
	else
	{
		GtkWidget *dlg = gtk_message_dialog_new(0, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, 
		"A janela de registros já encontra-se aberta e visível");
		gtk_dialog_run(dlg);
		gtk_widget_destroy(dlg);
	} 
}

// Imprime um texto no TextBuffer especificado
void ui_print(GtkTextBuffer *buffer, const gchar *texto, ...)
{
	va_list lista;
	char c_buffer[256];

	va_start(lista, texto);
	_vsnprintf(c_buffer, TAM(c_buffer), texto, lista);
	gtk_text_buffer_insert_at_cursor(buffer, c_buffer, (gint)strlen(c_buffer));

	va_end(lista);
}

// Carrega a janela de registros na memória para que ela seja exibida quando necessário
GtkWindow *desenhar_janela_registros()
{
	GtkBuilder *builder = gtk_builder_new_from_file( "interface.glade" );

	if(builder == NULL)
	{
		g_print( "Falha ao ler arquivo da interface\n");
		return NULL;
	}

	// Carrega os componentes da interface
	janela_registros = (GtkWindow*)gtk_builder_get_object(builder, "janela_registros");
	texto_registro = (GtkTextView*)gtk_builder_get_object(builder, "texto_registro");
	botao_consultar = (GtkButton*)gtk_builder_get_object(builder, "botao_realizar_consulta");
	botao_escolher = (GtkButton*)gtk_builder_get_object(builder, "botao_escolher");
	radio_button_data_hora = (GtkRadioButton*)gtk_builder_get_object(builder, "radio_button_data_hora");
	radio_button_distancia = (GtkRadioButton*)gtk_builder_get_object(builder, "radio_button_distancia");
	buffer_texto_registro = (GtkTextBuffer*)gtk_builder_get_object(builder, "buffer_texto_registro");

	// Verifica se todos foram carregados corretamente
	if(janela_registros == NULL || texto_registro == NULL || botao_consultar == NULL || botao_escolher == NULL ||
	radio_button_data_hora == NULL || radio_button_distancia == NULL)
	{
		g_print( "Falha ao carregar elementos da interface grafica\n");
		return NULL;
	}

	// Conectar os objetos e seu sinais
	g_signal_connect(botao_consultar, "clicked", on_botao_consultar_clicked, NULL);
	g_signal_connect(botao_escolher, "clicked", on_botao_escolher_clicked, NULL);
	g_signal_connect(janela_registros, "destroy", on_janela_registros_destroyed, NULL);

	// Ajustar o tamanho da janela
	gtk_widget_set_size_request(janela_registros, LARGURA + 50, ALTURA);

	// Editar o título
	gtk_window_set_title(janela_registros, "Consulta de registros");

	// Edita o ícone
	gtk_window_set_icon_from_file(janela_registros, "iconfinder_search-database_49618.ico", NULL);
	
	return janela_registros;
}

// Executada quando o usuário clica no botão de escolher o registro salvo em disco
static void on_botao_escolher_clicked(GtkWidget *widget, gpointer data)
{
	GtkFileChooser *chooser = NULL;
	GtkFileChooserAction acao = GTK_FILE_CHOOSER_ACTION_OPEN;
	GtkFileFilter *filtro = gtk_file_filter_new();

	(void)data;

	if(caminho_banco_dados != NULL)
	{
		GtkWidget *j = gtk_message_dialog_new(0, GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_USE_HEADER_BAR,
		GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, "O arquivo a seguir já foi selecionado:\n\n'%s'\n\nDeseja escolher outro?", caminho_banco_dados);

		gint resposta_dialog = gtk_dialog_run(GTK_DIALOG(j));
		gtk_widget_destroy(GTK_WIDGET(j));

		if(resposta_dialog == GTK_RESPONSE_NO)
			return;
	}

#ifdef DEBUG
	g_print( "Botao escolher clicado\n");
#endif

	// Cria um diálogo para selecionar arquivo
	chooser = gtk_file_chooser_dialog_new("Escolha o arquivo de registro", widget, acao, "Cancelar", GTK_RESPONSE_CANCEL,
				"Escolher", GTK_RESPONSE_ACCEPT, NULL);

	gtk_file_filter_add_pattern(filtro, "*.sdb");
	gtk_file_filter_set_name(filtro, "Banco de dados do Saturn (*.sdb)");

	gtk_file_chooser_add_filter(chooser,filtro);
	gtk_widget_set_size_request(GTK_WIDGET(chooser), LARGURA, ALTURA);

	gint result = gtk_dialog_run(GTK_DIALOG(chooser));
	if(result == GTK_RESPONSE_ACCEPT)
	{
		caminho_banco_dados = gtk_file_chooser_get_filename(chooser);
#ifdef DEBUG
		g_print( "Arquivo selecionado: %s\n", caminho_banco_dados);
#endif
		ui_print(buffer_texto_registro, "Arquivo selecionado: %s\n", caminho_banco_dados);
	}

	gtk_widget_destroy(GTK_WIDGET(chooser));
}

// Callback utilizado pela função sqlite3_exec().
static int sql_callback(void *p, int numero_colunas, char **dados_colunas, char **nomes_colunas)
{
	int i = 0;

	// Parâmetro não utilizado
	(void)p;

#ifndef DEBUG
	(void)nomes_colunas;
#endif

	// exibe a distancia e a data
	for(i = 0; i < numero_colunas; i += 2)
		ui_print(buffer_texto_registro, "%s - %s\n", dados_colunas[i], dados_colunas[i + 1]);

	num_registros++;

	return 0;
}

// Executada quando o usuário clica o botão de realizar consulta
static void on_botao_consultar_clicked(GtkWidget *widget, gpointer data)
{
	bool ordenar_por_distancia = false;
	char instrucao_sql[256];
	char *msgErro = 0;

	(void)data;

	gtk_text_buffer_set_text(buffer_texto_registro, "", 0);

	// O servidor precisa estar parado para consultar os registros
	if(!interrompido)
	{
		GtkWidget *m = gtk_message_dialog_new(GTK_WINDOW(widget), GTK_DIALOG_DESTROY_WITH_PARENT,
		GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "O servidor precisa estar parado");
		gtk_dialog_run(GTK_DIALOG(m))	;
		gtk_widget_destroy(m);
		
		return;
	}

	if(caminho_banco_dados == NULL)
	{
		gtk_text_buffer_set_text(buffer_texto_registro, "", 0);
		ui_print(buffer_texto_registro, "Nenhum banco de dados foi selecionado\n");

		return;
	}

#ifdef DEBUG
	g_print( "Botao consultar clicado\n");
#endif

	// Verifica qual tipo de ordenação o usuário deseja
	ordenar_por_distancia = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio_button_distancia));

#ifdef DEBUG
	g_print( "Ordenar por distancia: %s\n", ordenar_por_distancia ? "sim":"nao");
	g_print( "Abrindo arquivo: %s\n", caminho_banco_dados);
#endif

	// Tenta abrir o banco de dados
	int iOpen = sqlite3_open_v2(caminho_banco_dados, &banco, SQLITE_OPEN_READONLY, NULL);
	if(iOpen != SQLITE_OK) 
	{
		ui_print(buffer_texto_registro, "Falha ao abrir o banco de dados. Erro %d\n", GetLastError());
		
		return;
	}

	// Constrói a instrução SQL
	_snprintf(instrucao_sql, TAM(instrucao_sql), "SELECT * FROM %s\nORDER BY %s;",
	NOME_TABELA, (ordenar_por_distancia) ? COLUNA_DIST:COLUNA_TSTAMP);

#ifdef DEBUG
	g_print( "[SQL] %s\n", instrucao_sql);
#endif
	// Executa a instrução SQL
	int iExec = sqlite3_exec(banco, instrucao_sql, sql_callback, NULL, &msgErro);

	if(iExec != SQLITE_OK)
	{
#ifdef DEBUG
		g_print( "Falha ao executar SQL: %s\n", msgErro);
#endif
		ui_print(buffer_texto_registro, "Erro ao realizar consulta no banco de dados\n");
		return;
	}

	// Fecha o banco de dados
	sqlite3_close_v2(banco);

	if(num_registros == 0)
	{
		ui_print(buffer_texto_registro, "Nenhum registro foi encontrado no banco de dados\n");
	}

	num_registros = 0;
}

static void on_janela_registros_destroyed()
{
	janela_registros = NULL;
#ifdef DEBUG
	g_print( "Janela de registros destruida\n");
#endif
}
