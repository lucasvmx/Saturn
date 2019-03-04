/*
	Arquivo principal do código-fonte
	
	Autor: Lucas Vieira de Jesus
*/

#include "interface.h"

#include <stdio.h>
#include <gtk/gtk.h>

int main(int argc, char **argv)
{
	desenhar_janela_principal(argc, argv);
	gtk_main();

	return 0;
}