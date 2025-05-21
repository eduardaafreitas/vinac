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

membros *alloc_membro();

int stats(const char *file_name, membros *membro);
void interpreta_diretorio(FILE *arq, membros ***diretorio, long int *qtde_membros);
void inserir(char *archive_name, char *member_name, membros ***diretorio, long int *qtde_membros);
void listar(long int qtde_membros, membros **diretorio);
void inserir_comprimido(char *archive_name, char *member_name, membros ***diretorio, long int *qtde_membros);
void atualiza_diretorio(long int *qtde_membros, membros ***diretorio, FILE *archive);
void escrever_diretorio(FILE *arq, membros *membro);

void remover(char *archive_name, char *member_name, membros ***diretorio, long int *qtde_membros);
void extrair(char *archive_name, char *member_name, membros ***diretorio, long int *qtde_membros);
void mover(char *archive_name, char *member_name, membros ***diretorio, long int *qtde_membros, int nova_posicao);

#endif