#include "archiver.h"
#include "lz.h"
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

void interpreta_diretorio(FILE *arq, membros ***diretorio, long int *qtde_membros) {
    if (arq == NULL) {
        fprintf(stderr, "Arquivo não pode ser aberto.\n");
        exit(EXIT_FAILURE);
    }

    // Ler a quantidade de membros do cabeçalho
    fseek(arq, 0, SEEK_SET);
    if (fread(qtde_membros, sizeof(int), 1, arq) != 1) {
        fprintf(stderr, "Este arquivo ainda nao possui membros.\n");
        //exit(1);
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

    //printf("Diretório interpretado com sucesso. Total de membros: %ld\n", *qtde_membros);
}

void inserir(char *archive_name, char *member_name, membros ***diretorio, long int *qtde_membros) {
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

    // Criar uma nova instância para o membro
    membros *novo_membro = alloc_membro();

    // Obter informações do arquivo membro
    stats(member_name, novo_membro);
    novo_membro->offset = ftell(arq); // Posição no arquivo archive
    novo_membro->ordem_arquivo = *qtde_membros + 1;

    // Abrir o arquivo do membro para leitura
    member_file = fopen(member_name, "rb");
    if (!member_file) {
        perror("Erro ao abrir o arquivo do membro");
        free(novo_membro);
        fclose(arq);
        return;
    }

    // Escrever os dados do membro no final do archive
    fseek(arq, 0, SEEK_END);
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), member_file)) > 0) {
        fwrite(buffer, 1, bytes_read, arq);
    }
    fclose(member_file);

    // Atualizar o diretório
    if (substituir_indice != -1) {
        // Substituir membro existente
        free((*diretorio)[substituir_indice]->nome_do_membro);
        free((*diretorio)[substituir_indice]);
        (*diretorio)[substituir_indice] = novo_membro;
    } else {
        // Inserir novo membro
        *diretorio = realloc(*diretorio, (*qtde_membros + 1) * sizeof(membros *));
        if (*diretorio == NULL) {
            fprintf(stderr, "Erro ao realocar memória para o diretório.\n");
            free(novo_membro);
            fclose(arq);
            return;
        }
        (*diretorio)[*qtde_membros] = novo_membro;
        (*qtde_membros)++;
    }

    // Escrever o diretório atualizado no archive
    fseek(arq, 0, SEEK_SET);
    fwrite(qtde_membros, sizeof(int), 1, arq); // Escreve o número total de membros
    atualiza_diretorio(qtde_membros,  diretorio, arq);
    fclose(arq);
    printf("Membro %s inserido com sucesso no arquivo %s.\n", member_name, archive_name);
}

void inserir_comprimido(char *archive_name, char *member_name, membros ***diretorio, long int *qtde_membros) {
    FILE *archive = fopen(archive_name, "rb+");
    if (!archive) {
        archive = fopen(archive_name, "wb+");
        if (!archive) {
            fprintf(stderr, "Erro ao criar arquivo archive em inserir_comprimido.\n");
            return;
        }
        int zero = 0;
        fwrite(&zero, sizeof(int), 1, archive);
    }

    // Carregar o diretório existente
    interpreta_diretorio(archive, diretorio, qtde_membros);

    // Abrir o arquivo do membro para leitura
    FILE *in = fopen(member_name, "rb");
    if (!in) {
        fprintf(stderr, "Erro ao abrir o arquivo %s em inserir_comprimido.\n", member_name);
        fclose(archive);
        return;
    }

    // Obter o tamanho do arquivo original
    fseek(in, 0, SEEK_END);
    long original_size = ftell(in);
    fseek(in, 0, SEEK_SET);

    // Ler os dados do arquivo original
    unsigned char *data = malloc(original_size);
    if (!data) {
        fprintf(stderr, "Erro ao alocar memória para dados originais.\n");
        fclose(in);
        fclose(archive);
        return;
    }
    fread(data, 1, original_size, in);
    fclose(in);

    // Criar espaço para os dados comprimidos
    unsigned char *compressed_data = malloc(original_size);
    if (!compressed_data) {
        fprintf(stderr, "Erro ao alocar memória para dados comprimidos.\n");
        free(data);
        fclose(archive);
        return;
    }

    // Comprimir os dados
    int compressed_size = LZ_Compress(data, compressed_data, original_size);
    if (compressed_size >= original_size) {
        // Inserir sem compressão se não houver ganho
        free(data);
        free(compressed_data);
        fclose(archive);
        inserir(archive_name, member_name, diretorio, qtde_membros);
        return;
    }

    free(data);

    // Obter metadados do arquivo original
    struct stat st;
    stat(member_name, &st);

    // Preencher a estrutura membros
    membros *novo_membro = alloc_membro();
    novo_membro->nome_do_membro = strdup(member_name);
    novo_membro->UID = (unsigned int)getuid();
    novo_membro->tamanho_original = (unsigned int)original_size;
    novo_membro->tamanho_disco = (unsigned int)compressed_size;
    novo_membro->data_modificacao = st.st_mtime;
    novo_membro->ordem_arquivo = *qtde_membros + 1;

    // Obter o offset no arquivo archive
    fseek(archive, 0, SEEK_END);
    novo_membro->offset = ftell(archive);

    // Escrever os dados comprimidos no archive
    fwrite(compressed_data, 1, compressed_size, archive);
    free(compressed_data);

    // Atualizar o diretório
    *diretorio = realloc(*diretorio, (*qtde_membros + 1) * sizeof(membros *));
    if (!*diretorio) {
        fprintf(stderr, "Erro ao realocar memória para o diretório.\n");
        free(novo_membro);
        fclose(archive);
        return;
    }
    (*diretorio)[*qtde_membros] = novo_membro;
    (*qtde_membros)++;

    // Salvar o diretório atualizado
    fseek(archive, 0, SEEK_SET);
    fwrite(qtde_membros, sizeof(int), 1, archive); // Atualizar o número de membros
    atualiza_diretorio(qtde_membros,  diretorio, archive);

    fclose(archive);
    printf("Arquivo %s inserido comprimido no arquivo %s.\n", member_name, archive_name);
}

void atualiza_diretorio(long int *qtde_membros, membros ***diretorio, FILE *archive){
    for (int i = 0; i < *qtde_membros; i++) {
        membros *membro = (*diretorio)[i];
        fwrite(&membro->UID, sizeof(membro->UID), 1, archive);
        fwrite(&membro->tamanho_original, sizeof(membro->tamanho_original), 1, archive);
        fwrite(&membro->tamanho_disco, sizeof(membro->tamanho_disco), 1, archive);
        fwrite(&membro->data_modificacao, sizeof(membro->data_modificacao), 1, archive);
        fwrite(&membro->ordem_arquivo, sizeof(membro->ordem_arquivo), 1, archive);
        fwrite(&membro->offset, sizeof(membro->offset), 1, archive);

        // Escrever o nome do membro
        size_t nome_length = strlen(membro->nome_do_membro) + 1;
        fwrite(&nome_length, sizeof(size_t), 1, archive);
        fwrite(membro->nome_do_membro, sizeof(char), nome_length, archive);
    }
}

void listar(long int qtde_membros, membros **diretorio){
    for (int i = 0; i < qtde_membros; i++) {
        printf("Membro %d: %s, UID: %u, Tamanho: %zu bytes, Offset: %zu\n",
               i + 1,
               diretorio[i]->nome_do_membro,
               diretorio[i]->UID,
               diretorio[i]->tamanho_original,
               diretorio[i]->offset);
    }
}

void remover(char *archive_name, char *member_name, membros ***diretorio, long int *qtde_membros) {
    FILE *archive = fopen(archive_name, "rb+");
    if (!archive) {
        fprintf(stderr, "Erro ao abrir arquivo archive em remover.\n");
        return;
    }

    // Carregar o diretório
    interpreta_diretorio(archive, diretorio, qtde_membros);

    // Localizar o membro a ser removido
    int indice = -1;
    for (int i = 0; i < *qtde_membros; i++) {
        if (strcmp((*diretorio)[i]->nome_do_membro, member_name) == 0) {
            indice = i;
            break;
        }
    }

    if (indice == -1) {
        fprintf(stderr, "Erro: Membro %s não encontrado no arquivo.\n", member_name);
        fclose(archive);
        return;
    }

    // Remover o membro do diretório
    free((*diretorio)[indice]->nome_do_membro);
    free((*diretorio)[indice]);
    for (int i = indice; i < *qtde_membros - 1; i++) {
        (*diretorio)[i] = (*diretorio)[i + 1];
    }
    (*qtde_membros)--;

    // Atualizar o diretório no arquivo
    fseek(archive, 0, SEEK_SET);
    fwrite(qtde_membros, sizeof(int), 1, archive);
    for (int i = 0; i < *qtde_membros; i++) {
        membros *membro = (*diretorio)[i];
        fwrite(&membro->UID, sizeof(membro->UID), 1, archive);
        fwrite(&membro->tamanho_original, sizeof(membro->tamanho_original), 1, archive);
        fwrite(&membro->tamanho_disco, sizeof(membro->tamanho_disco), 1, archive);
        fwrite(&membro->data_modificacao, sizeof(membro->data_modificacao), 1, archive);
        fwrite(&membro->ordem_arquivo, sizeof(membro->ordem_arquivo), 1, archive);
        fwrite(&membro->offset, sizeof(membro->offset), 1, archive);

        size_t nome_length = strlen(membro->nome_do_membro) + 1;
        fwrite(&nome_length, sizeof(size_t), 1, archive);
        fwrite(membro->nome_do_membro, sizeof(char), nome_length, archive);
    }

    fclose(archive);
    printf("Membro %s removido com sucesso do arquivo %s.\n", member_name, archive_name);
}

void extrair(char *archive_name, char *member_name, membros ***diretorio, long int *qtde_membros) {
    FILE *archive = fopen(archive_name, "rb");
    if (!archive) {
        fprintf(stderr, "Erro ao abrir arquivo archive em extrair.\n");
        return;
    }

    // Carregar o diretório
    interpreta_diretorio(archive, diretorio, qtde_membros);

    // Localizar o membro a ser extraído
    int indice = -1;
    for (int i = 0; i < *qtde_membros; i++) {
        if (strcmp((*diretorio)[i]->nome_do_membro, member_name) == 0) {
            indice = i;
            break;
        }
    }

    if (indice == -1) {
        fprintf(stderr, "Erro: Membro %s não encontrado no arquivo.\n", member_name);
        fclose(archive);
        return;
    }

    membros *membro = (*diretorio)[indice];

    // Criar o arquivo para extração
    FILE *out = fopen(membro->nome_do_membro, "wb");
    if (!out) {
        fprintf(stderr, "Erro ao criar arquivo %s para extração.\n", membro->nome_do_membro);
        fclose(archive);
        return;
    }

    // Ler os dados do membro do arquivo archive
    unsigned char *data = malloc(membro->tamanho_disco);
    if (!data) {
        fprintf(stderr, "Erro ao alocar memória para extração.\n");
        fclose(out);
        fclose(archive);
        return;
    }

    fseek(archive, membro->offset, SEEK_SET);
    fread(data, 1, membro->tamanho_disco, archive);

    // Verificar se o membro está comprimido
    if (membro->tamanho_disco < membro->tamanho_original) {
        unsigned char *decompressed_data = malloc(membro->tamanho_original);
        if (!decompressed_data) {
            fprintf(stderr, "Erro ao alocar memória para descompressão.\n");
            free(data);
            fclose(out);
            fclose(archive);
            return;
        }

        LZ_Uncompress(data, decompressed_data, membro->tamanho_disco);
        fwrite(decompressed_data, 1, membro->tamanho_original, out);
        free(decompressed_data);
    } else {
        fwrite(data, 1, membro->tamanho_disco, out);
    }

    free(data);
    fclose(out);
    fclose(archive);

    printf("Membro %s extraído com sucesso.\n", membro->nome_do_membro);
}

void mover(char *archive_name, char *member_name, membros ***diretorio, long int *qtde_membros, int nova_posicao) {
    if (nova_posicao < 0 || nova_posicao >= *qtde_membros) {
        fprintf(stderr, "Erro: Posição inválida.\n");
        return;
    }

    FILE *archive = fopen(archive_name, "rb+");
    if (!archive) {
        fprintf(stderr, "Erro ao abrir arquivo archive em mover.\n");
        return;
    }

    // Carregar o diretório
    interpreta_diretorio(archive, diretorio, qtde_membros);

    // Localizar o membro a ser movido
    int indice = -1;
    for (int i = 0; i < *qtde_membros; i++) {
        if (strcmp((*diretorio)[i]->nome_do_membro, member_name) == 0) {
            indice = i;
            break;
        }
    }

    if (indice == -1) {
        fprintf(stderr, "Erro: Membro %s não encontrado no arquivo.\n", member_name);
        fclose(archive);
        return;
    }

    // Mover o membro na lista
    membros *membro = (*diretorio)[indice];
    if (indice < nova_posicao) {
        for (int i = indice; i < nova_posicao; i++) {
            (*diretorio)[i] = (*diretorio)[i + 1];
        }
    } else {
        for (int i = indice; i > nova_posicao; i--) {
            (*diretorio)[i] = (*diretorio)[i - 1];
        }
    }
    (*diretorio)[nova_posicao] = membro;

    // Regravar o diretório atualizado
    fseek(archive, 0, SEEK_SET);
    fwrite(qtde_membros, sizeof(int), 1, archive);
    for (int i = 0; i < *qtde_membros; i++) {
        membros *membro = (*diretorio)[i];
        fwrite(&membro->UID, sizeof(membro->UID), 1, archive);
        fwrite(&membro->tamanho_original, sizeof(membro->tamanho_original), 1, archive);
        fwrite(&membro->tamanho_disco, sizeof(membro->tamanho_disco), 1, archive);
        fwrite(&membro->data_modificacao, sizeof(membro->data_modificacao), 1, archive);
        fwrite(&membro->ordem_arquivo, sizeof(membro->ordem_arquivo), 1, archive);
        fwrite(&membro->offset, sizeof(membro->offset), 1, archive);

        size_t nome_length = strlen(membro->nome_do_membro) + 1;
        fwrite(&nome_length, sizeof(size_t), 1, archive);
        fwrite(membro->nome_do_membro, sizeof(char), nome_length, archive);
    }

    fclose(archive);
    printf("Membro %s movido para a posição %d no arquivo %s.\n", member_name, nova_posicao, archive_name);
}

void derivar(char *archive_name, char **members_to_copy, int num_members, membros ***diretorio, long int *qtde_membros) {
    // Abrir o arquivo original
    FILE *archive = fopen(archive_name, "rb");
    if (!archive) {
        fprintf(stderr, "Erro: Não foi possível abrir o arquivo %s.\n", archive_name);
        return;
    }

    // Carregar o diretório do arquivo original
    interpreta_diretorio(archive, diretorio, qtde_membros);

    // Verificar se todos os membros especificados existem no arquivo
    int *indices = malloc(num_members * sizeof(int)); // Guardar índices dos membros selecionados
    if (!indices) {
        fprintf(stderr, "Erro ao alocar memória para índices dos membros.\n");
        fclose(archive);
        return;
    }

    for (int i = 0; i < num_members; i++) {
        indices[i] = -1; // Inicializar com índice inválido
        for (int j = 0; j < *qtde_membros; j++) {
            if (strcmp((*diretorio)[j]->nome_do_membro, members_to_copy[i]) == 0) {
                indices[i] = j; // Encontrou o membro
                break;
            }
        }
        if (indices[i] == -1) {
            fprintf(stderr, "Erro: O membro %s não foi encontrado no arquivo %s.\n", members_to_copy[i], archive_name);
            free(indices);
            fclose(archive);
            return;
        }
    }

    // Criar o nome do novo arquivo
    char new_archive_name[256];
    snprintf(new_archive_name, sizeof(new_archive_name), "%s_z.vc", strtok(archive_name, "."));

    // Criar o novo arquivo de archive
    FILE *new_archive = fopen(new_archive_name, "wb+");
    if (!new_archive) {
        fprintf(stderr, "Erro ao criar o novo arquivo %s.\n", new_archive_name);
        free(indices);
        fclose(archive);
        return;
    }

    // Escrever o número de membros no novo arquivo
    fwrite(&num_members, sizeof(int), 1, new_archive);

    // Copiar os membros especificados para o novo arquivo
    for (int i = 0; i < num_members; i++) {
        membros *membro = (*diretorio)[indices[i]];

        // Copiar metadados do membro
        fwrite(&membro->UID, sizeof(membro->UID), 1, new_archive);
        fwrite(&membro->tamanho_original, sizeof(membro->tamanho_original), 1, new_archive);
        fwrite(&membro->tamanho_disco, sizeof(membro->tamanho_disco), 1, new_archive);
        fwrite(&membro->data_modificacao, sizeof(membro->data_modificacao), 1, new_archive);
        fwrite(&membro->ordem_arquivo, sizeof(membro->ordem_arquivo), 1, new_archive);
        fwrite(&membro->offset, sizeof(membro->offset), 1, new_archive);

        size_t nome_length = strlen(membro->nome_do_membro) + 1;
        fwrite(&nome_length, sizeof(size_t), 1, new_archive);
        fwrite(membro->nome_do_membro, sizeof(char), nome_length, new_archive);

        // Copiar o conteúdo do membro
        unsigned char *data = malloc(membro->tamanho_disco);
        if (!data) {
            fprintf(stderr, "Erro ao alocar memória para os dados do membro %s.\n", membro->nome_do_membro);
            fclose(new_archive);
            fclose(archive);
            free(indices);
            return;
        }

        fseek(archive, membro->offset, SEEK_SET);
        fread(data, 1, membro->tamanho_disco, archive);
        fwrite(data, 1, membro->tamanho_disco, new_archive);

        free(data);
    }

    free(indices);
    fclose(new_archive);
    fclose(archive);

    printf("Arquivo derivado criado com sucesso: %s\n", new_archive_name);
}
