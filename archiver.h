#ifndef __ARCHIVER__
#define __ARCHIVER__

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


struct membros {
    char *nome_do_membro;
    unsigned int UID;
    unsigned long tamanho_original;
    unsigned long tamanho_disco;
    time_t data_modificacao;
    unsigned int ordem_arquivo;
    unsigned long offset;
    
};
typedef struct membros membros;
//vetor de ponteiros

struct archive {
    FILE *diretorio;
    membros *membro;
};
typedef struct archive archive;

membros *alloc_membro();
FILE *inicia_archive(char *archive_name);

int stats(const char *file_name, membros *membro);

void inserir(membros *membro, char *archive_name, FILE *arq, char *member_name);
void escrever_diretorio(archive *arq, membros *membro);


membros *remover(membros *membro);
void atualizar(membros *membro);
void destruir(membros *membro);

#endif