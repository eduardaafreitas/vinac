#define _GNU_SOURCE
#include "archiver.h"

int main(int argc, char *argv[]) {
    int option;
    char *archive_name;
    membros **diretorio = NULL;
    long int qtde_membros = 0;

    if (argc < 3) {
        fprintf(stderr, "Forma de uso: vinac <opção> <archive> [membro1 membro2 ...]\n");
        exit(1);
    }

    archive_name = argv[2];
    FILE *arq = fopen(archive_name, "a+");
    if (!arq) {
        perror("Erro ao abrir ou criar o arquivo archive");
        exit(1);
    }

    interpreta_diretorio(arq, &diretorio, &qtde_membros);
    membros *novo_membro = alloc_membro();

    while ((option = getopt(argc, argv, "i:p:m:x:r:c")) != -1) {
        switch (option) {
            case 'i': { // Inserir membros comprimidos
                for (int i = optind; i < argc; i++) {
                    inserir_comprimido(archive_name, argv[i], &diretorio, &qtde_membros);
                }
                break;
            }            
            case 'p': //inserir sem compressao
                for (int i = optind; i < argc; i++) {
                    inserir(archive_name, argv[i], &diretorio, &qtde_membros);
                }
                break;
            case 'm': //mover 
                printf("-m em implementação...\n");
                break;
            case 'x': //extrair 
                printf("-x em implementação...\n");
                break;
            case 'r': //remover 
                printf("-r em implementação...\n");
                break;
            case 'c': // Listar conteúdo do diretório
                printf("Listando conteúdo do arquivo %s:\n", archive_name);
                listar(qtde_membros, diretorio);
                break;
            default: // Opção inválida
                fprintf(stderr, "Forma de uso: vinac <opção> <archive> [membro1 membro2 ...]\n");
                exit(1);
        }
    }

    // Liberar recursos alocados
    for (int i = 0; i < qtde_membros; i++) {
        free(diretorio[i]->nome_do_membro);
        free(diretorio[i]);
    }
    free(diretorio);
    free(novo_membro);

    fclose(arq);
    return 0;
}