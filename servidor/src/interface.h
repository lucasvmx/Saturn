/*
	Definições de interface.c
	
	Autor: Lucas Vieira de Jesus
*/

#ifndef INTERFACE_H
#define INTERFACE_H

#include <gtk/gtk.h>

GObject *desenhar_janela_principal(int argc, char **argv);
static void on_botao_iniciar_server_clicked(GtkWidget *widget, gpointer data);
static void on_botao_consultar_registros_salvos_clicked(GtkWidget *widget, gpointer data);
void ui_print(GtkTextBuffer *buffer, const gchar *texto, ...);

// Janela de registros
GtkWindow *desenhar_janela_registros();
static void on_botao_escolher_clicked(GtkWidget *widget, gpointer data);
static void on_botao_consultar_clicked(GtkWidget *widget, gpointer data);
static void on_janela_registros_destroyed();

#endif