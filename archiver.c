#include "archiver.h"

membros *alloc_membro(){ //aloca estrutura membro
    membros *membro = (membros*) malloc(sizeof(membro));

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

void inserir(membros *membro){
    
    FILE *archive;
    FILE *member_file;
    char buffer[1024];
    size_t bytes_read;
    
    // Abrir ou criar o arquivo archive
    archive = fopen("archive.vc", "rb+");
    if (!archive) {
        archive = fopen("archive.vc", "wb+");
        if (!archive) {
            perror("Erro ao abrir o arquivo archive");
            return;
        }
    }
    
    // Aqui, você carregaria o diretório do archive para verificar existência
    // Por simplicidade, vamos prosseguir com a inserção
    
    // Abrir o arquivo do membro para leitura
    member_file = fopen((char *)membro->nome_do_membro, "rb");
    if (!member_file) {
        perror("Erro ao abrir o arquivo do membro");
        fclose(archive);
        return;
    }
    
    // Obter o tamanho do arquivo membro
    fseek(member_file, 0, SEEK_END);
    membro->tamanho_original = ftell(member_file);
    rewind(member_file);
    
    // Escrever os dados do membro no final do archive
    fseek(archive, 0, SEEK_END);
    membro->offset = ftell(archive);
    
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), member_file)) > 0) {
        fwrite(buffer, 1, bytes_read, archive);
    }
    
    // Atualizar o diretório na memória e salvar no arquivo
    // Aqui, você adicionaria o membro ao diretório e gravaria no archive
    
    // Fechar arquivos
    fclose(member_file);
    fclose(archive);
    
    printf("Membro %s inserido com sucesso.\n", membro->nome_do_membro);

    
}
