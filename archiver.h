#ifndef __ARCHIVER__
#define __ARCHIVER__

#include <time.h>
#include <stdio.h>


struct membros {
    unsigned char *nome_do_membro;
    unsigned int UID;
    unsigned long tamanho_original;
    unsigned long tamanho_disco;
    time_t data_modificacao;
    unsigned int ordem_arquivo;
    unsigned long offset;
    struct membros *anterior;
    struct membros *proximo;
};
typedef struct membros membros;

struct archive {
    FILE *diretorio;
    membros *membro;
};
typedef struct archive archive;

membros *alloc_membro();
void inserir(membros *membro);
membros *remover(membros *membro);
void atualizar(membros *membro);
void destruir(membros *membro);

#endif