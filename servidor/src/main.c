/*
	Arquivo principal do código-fonte
	
	Autor: Lucas Vieira de Jesus
*/

#include <stdio.h>
#include <gtk/gtk.h>

int main(int argc, char **argv)
{
	GObject *janela_pai = NULL;
	
	g_thread_init(NULL);
	gdk_threads_init();

	janela_pai = desenhar_janela_principal(argc, argv);
	gdk_threads_enter();
	gtk_main();
	gdk_threads_leave();

	return 0;
}