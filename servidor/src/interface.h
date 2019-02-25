/*
	Definições de interface.c
	
	Autor: Lucas Vieira de Jesus
*/

#ifndef INTERFACE_H
#define INTERFACE_H

#include <gtk/gtk.h>

GObject *desenhar_janela_principal(int argc, char **argv);
static void on_botao_iniciar_server_clicked(GtkWidget *widget, gpointer data);
void mudar_texto_buffer(GtkTextBuffer *buffer, const gchar *texto);

#endif