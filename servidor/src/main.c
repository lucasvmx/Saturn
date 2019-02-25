/*
	Arquivo principal do código-fonte
	
	Autor: Lucas Vieira de Jesus
*/

#include <stdio.h>
#include <gtk/gtk.h>

int main(int argc, char **argv)
{
	GObject *janela_pai = NULL;
	
	janela_pai = desenhar_janela_principal(argc, argv);
	gtk_main();
	
	return 0;
}