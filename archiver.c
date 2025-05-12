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

int conta_membros(const char *archive) {
    FILE *arq = fopen(archive, "rb");
    if (!arq) {
        perror("Erro ao abrir archive");
        return -1;  
    }

    fseek(arq, 0, SEEK_END);
    long tam = ftell(arq);
    if (tam < sizeof(int)) {
        fclose(arq);
        return 0; 
    }

    fseek(arq, -sizeof(int), SEEK_END);
    int total_membros;
    if (fread(&total_membros, sizeof(int), 1, arq) != 1) {
        perror("Erro ao ler quantidade de membros");
        fclose(arq);
        return -1;
    }

    fclose(arq);
    return total_membros;
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

void interpreta_diretorio(FILE *arq, membros ***diretorio, long int *qtde_membros) {
    if (arq == NULL) {
        fprintf(stderr, "Arquivo não pode ser aberto.\n");
        exit(EXIT_FAILURE);
    }

    // Ler a quantidade de membros do cabeçalho
    fseek(arq, 0, SEEK_SET);
    if (fread(qtde_membros, sizeof(int), 1, arq) != 1) {
        fprintf(stderr, "Erro ao ler a quantidade de membros no cabeçalho.\n");
    }

    // Alocar memória para o array de ponteiros de membros
    *diretorio = (membros **)malloc(*qtde_membros * sizeof(membros *));
    if (*diretorio == NULL) {
        fprintf(stderr, "Erro ao alocar memória para o diretório.\n");
        exit(EXIT_FAILURE);
    }

    // Ler cada membro do arquivo
    for (int i = 0; i < *qtde_membros; i++) {
        (*diretorio)[i] = (membros *)malloc(sizeof(membros));
        if ((*diretorio)[i] == NULL) {
            fprintf(stderr, "Erro ao alocar memória para o membro %d.\n", i);
            exit(EXIT_FAILURE);
        }

        // Ler dados fixos do membro
        fread(&(*diretorio)[i]->UID, sizeof((*diretorio)[i]->UID), 1, arq);
        fread(&(*diretorio)[i]->tamanho_original, sizeof((*diretorio)[i]->tamanho_original), 1, arq);
        fread(&(*diretorio)[i]->tamanho_disco, sizeof((*diretorio)[i]->tamanho_disco), 1, arq);
        fread(&(*diretorio)[i]->data_modificacao, sizeof((*diretorio)[i]->data_modificacao), 1, arq);
        fread(&(*diretorio)[i]->ordem_arquivo, sizeof((*diretorio)[i]->ordem_arquivo), 1, arq);
        fread(&(*diretorio)[i]->offset, sizeof((*diretorio)[i]->offset), 1, arq);

        // Ler o nome do membro
        size_t nome_length;
        fread(&nome_length, sizeof(size_t), 1, arq);
        (*diretorio)[i]->nome_do_membro = (char *)malloc(nome_length);
        if ((*diretorio)[i]->nome_do_membro == NULL) {
            fprintf(stderr, "Erro ao alocar memória para o nome do membro %d.\n", i);
            exit(EXIT_FAILURE);
        }
        fread((*diretorio)[i]->nome_do_membro, sizeof(char), nome_length, arq);
    }

    printf("Diretório interpretado com sucesso. Total de membros: %ld\n", *qtde_membros);
}


void inserir(membros *novo_membro, char *archive_name, char *member_name, membros ***diretorio, long int *qtde_membros) {
    FILE *arq;
    FILE *member_file;
    char buffer[1024];
    size_t bytes_read;

    // Verificar se o arquivo membro existe antes de abrir
    if (access(member_name, F_OK) != 0) {
        fprintf(stderr, "Erro: Arquivo %s não encontrado.\n", member_name);
        return;
    }

    // Abrir o arquivo de archive (criar se não existir)
    arq = fopen(archive_name, "rb+");
    if (!arq) {
        arq = fopen(archive_name, "wb+");
        if (!arq) {
            perror("Erro ao abrir o arquivo archive");
            return;
        }
    }

    // Interpretar o diretório existente
    interpreta_diretorio(arq, diretorio, qtde_membros);

    // Verificar se o membro já existe no diretório
    int substituir_indice = -1;
    for (int i = 0; i < *qtde_membros; i++) {
        if (strcmp((*diretorio)[i]->nome_do_membro, member_name) == 0) {
            substituir_indice = i;
            break;
        }
    }

    // Abrir o arquivo do membro para leitura
    member_file = fopen(member_name, "rb");
    if (!member_file) {
        perror("Erro ao abrir o arquivo do membro");
        fclose(arq);
        return;
    }

    // Obter informações do arquivo membro
    stats(member_name, novo_membro);
    novo_membro->offset = ftell(arq); // Posição no arquivo archive
    novo_membro->ordem_arquivo = *qtde_membros + 1;

    // Escrever os dados do membro no final do archive
    fseek(arq, 0, SEEK_END);
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), member_file)) > 0) {
        fwrite(buffer, 1, bytes_read, arq);
    }
    fclose(member_file);

    // Atualizar o diretório
    if (substituir_indice != -1) {
        // Substituir membro existente
        free((*diretorio)[substituir_indice]);
        (*diretorio)[substituir_indice] = novo_membro;
    } else {
        // Inserir novo membro
        *diretorio = realloc(*diretorio, (*qtde_membros + 1) * sizeof(membros *));
        (*diretorio)[*qtde_membros] = novo_membro;
        (*qtde_membros)++;
    }

    // Escrever o diretório atualizado no archive
    fseek(arq, 0, SEEK_SET);
    fwrite(qtde_membros, sizeof(int), 1, arq); // Escreve o número total de membros
    for (int i = 0; i < *qtde_membros; i++) {
        membros *membro = (*diretorio)[i];
        fwrite(&membro->UID, sizeof(membro->UID), 1, arq);
        fwrite(&membro->tamanho_original, sizeof(membro->tamanho_original), 1, arq);
        fwrite(&membro->tamanho_disco, sizeof(membro->tamanho_disco), 1, arq);
        fwrite(&membro->data_modificacao, sizeof(membro->data_modificacao), 1, arq);
        fwrite(&membro->ordem_arquivo, sizeof(membro->ordem_arquivo), 1, arq);
        fwrite(&membro->offset, sizeof(membro->offset), 1, arq);

        // Escrever o nome do membro
        size_t nome_length = strlen(membro->nome_do_membro) + 1;
        fwrite(&nome_length, sizeof(nome_length), 1, arq);
        fwrite(membro->nome_do_membro, sizeof(char), nome_length, arq);
    }

    fclose(arq);
    printf("Membro %s inserido com sucesso no arquivo %s.\n", member_name, archive_name);
}
