/*
	Arquivo principal do código-fonte
	
	Autor: Lucas Vieira de Jesus
*/

#include "interface.h"
#include "comum.h"

#include <stdio.h>
#include <gtk/gtk.h>
#include <io.h>
#include <windows.h>

int main(int argc, char **argv)
{
	// Preparar o ambiente para salvar os registros e planilhas
	if(access(PASTA_PLANILHAS, 0) == -1)
	{
		if(CreateDirectoryA(PASTA_PLANILHAS, NULL) == 0)
		{
			fprintf(stderr, "Falha ao criar pasta: %lu\n", GetLastError());
			exit((int)GetLastError());
		}
	}
	
	if(access(PASTA_DATABASE, 0) == -1)
	{
		if(CreateDirectoryA(PASTA_DATABASE, NULL) == 0)
		{
			fprintf(stderr, "Falha ao criar pasta: %lu\n", GetLastError());
			exit((int)GetLastError());			
		}
	}

	desenhar_janela_principal(argc, argv);
	gtk_main();

	return 0;
}