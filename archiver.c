#include "archiver.h"
#include <sys/stat.h>
#define MAX_MEMBER 50

membros *alloc_membro(){ //aloca estrutura membro
    membros *membro = (membros*) malloc(sizeof(membros));

    membro->nome_do_membro = NULL;
    membro->UID = 0;
    membro->tamanho_original = 0.0;
    membro->tamanho_disco = 0.0;
    membro->data_modificacao = time(NULL);
    membro->ordem_arquivo = 0;
    membro->offset = 0.0;
//stats
    return membro;
}

int stats(const char *file_name, membros *membro) {
    struct stat file_stat;

    // Obter as informações do arquivo
    if (stat(file_name, &file_stat) != 0) {
        perror("Erro ao obter informações do arquivo");
        return -1;
    }

    // Preencher os campos da struct membros
    membro->nome_do_membro = strdup(file_name);
    membro->UID = file_stat.st_uid; // UID do proprietário do arquivo
    membro->tamanho_original = file_stat.st_size; // Tamanho do arquivo
    membro->data_modificacao = file_stat.st_mtime; // Data da última modificação

    // Preenchimento adicional
    membro->ordem_arquivo = 0; // Pode ser atualizado conforme a lógica do programa
    membro->offset = 0; // Atualizado posteriormente

    return 0; // Retorna 0 para indicar sucesso
}

void escrever_diretorio(FILE *arq, membros *membro) {
    if (!arq || !arq || !membro) {
        fprintf(stderr, "Arquivo ou membro inválido.\n");
        return;
    }

    // Abrir o arquivo no modo de leitura/escrita
    arq = fopen("archive.vc", "rb+");
    if (!arq) {
        perror("Erro ao abrir o arquivo archive para escrita do diretório");
        return;
    }

    // Posicionar o ponteiro no final do arquivo
    fseek(arq, 0, SEEK_END);

    // Escrever as informações do diretório
    fwrite(&membro->UID, sizeof(membro->UID), 1, arq);
    fwrite(&membro->tamanho_original, sizeof(membro->tamanho_original), 1, arq);
    fwrite(&membro->tamanho_disco, sizeof(membro->tamanho_disco), 1, arq);
    fwrite(&membro->data_modificacao, sizeof(membro->data_modificacao), 1, arq);
    fwrite(&membro->ordem_arquivo, sizeof(membro->ordem_arquivo), 1, arq);
    fwrite(&membro->offset, sizeof(membro->offset), 1, arq);

    // Escrever o nome do membro
    size_t nome_length = strlen(membro->nome_do_membro) + 1; // Inclui o '\0'
    fwrite(&nome_length, sizeof(nome_length), 1, arq);
    fwrite(membro->nome_do_membro, sizeof(char), nome_length, arq);

    // Fechar o arquivo
    fclose(arq);

    printf("Diretório do membro %s escrito com sucesso.\n", membro->nome_do_membro);
}

void inserir(membros *membro, char *archive_name, char *member_name){
    
    FILE *arq;
    FILE *member_file;
    char buffer[1024];
    size_t bytes_read;

    // Verificar se o arquivo membro existe antes de abrir
    if (access(member_name, F_OK) != 0) {
        fprintf(stderr, "Erro: Arquivo %s não encontrado.\n", member_name);
        return;
    }

    arq = fopen(archive_name, "rb+");
    if (!arq) {
        arq = fopen(archive_name, "wb+");
        if (!arq) {
            perror("Erro ao abrir o arquivo archive");
            return;
        }
    }

    if (stats(archive_name, membro) != 0) {
        fprintf(stderr, "Erro ao preencher informações do arquivo %s.\n", archive_name);
        return;
    }

    // Abrir o arquivo do membro para leitura
    membro->nome_do_membro = strdup(member_name);
    member_file = fopen(member_name, "rb");
    if (!member_file) {
        perror("Erro ao abrir o arquivo do membro");
        fclose(arq);
        return;
    }

    // Obter o tamanho do arquivo membro
    fseek(member_file, 0, SEEK_END);
    membro->tamanho_original = ftell(member_file);
    rewind(member_file);

    // Escrever os dados do membro no final do archive
    fseek(arq, 0, SEEK_END);
    membro->offset = ftell(arq);

    while ((bytes_read = fread(buffer, 1, sizeof(buffer), member_file)) > 0) {
        fwrite(buffer, 1, bytes_read, arq);
    }

    fclose(member_file);
    fclose(arq);

    printf("Membro %s inserido com sucesso no arquivo %s.\n", membro->nome_do_membro, archive_name);
    free(membro->nome_do_membro);

}
