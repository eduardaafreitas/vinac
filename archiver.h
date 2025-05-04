#ifndef __ARCHIVER__
#define __ARCHIVER__

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct {
    unsigned char *nome_do_membro;
    unsigned int UID;
    unsigned long tamanho_original;
    unsigned long tamanho_disco;
    time_t data_modificacao;
    unsigned int ordem_arquivo;
    unsigned long offset;
    struct membros *anterior;
    struct membros *proximo;
} membros;

typedef struct {
    FILE *diretorio;
    membros *membro;
} archive;


membros *alloc_membro();
void inserir(membros *membro);
membros *remover(membros *membro);
void atualizar(membros *membro);
void destruir(membros *membro);

#endif