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

//diretorio => vetor de ponteiros

// struct archive {
//     FILE *diretorio;
//     membros *membro;
// };
// typedef struct archive archive;

membros *alloc_membro();

FILE *inicia_archive(char *archive_name);
int stats(const char *file_name, membros *membro);

void interpreta_diretorio(FILE *arq, membros ***diretorio, long int *qtde_membros);

void inserir(membros *novo_membro, char *archive_name, char *member_name, membros ***diretorio, long int *qtde_membros);
void escrever_diretorio(FILE *arq, membros *membro);
membros *remover(membros *membro);
void atualizar(membros *membro);
void destruir(membros *membro);

#endif