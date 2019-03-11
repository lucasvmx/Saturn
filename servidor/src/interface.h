/*
	Definições de interface.c
	
	Autor: Lucas Vieira de Jesus
*/

#ifndef INTERFACE_H
#define INTERFACE_H

#include <gtk/gtk.h>

GObject *desenhar_janela_principal(int argc, char **argv);
void ui_print(GtkTextBuffer *buffer, const gchar *texto, ...);
void limpar_texto(GtkTextBuffer *buffer);

// Janela de registros
GtkWindow *desenhar_janela_registros(GtkWindow *janela_mae);

#endif