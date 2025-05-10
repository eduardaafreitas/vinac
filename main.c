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

    FILE *arq = fopen(archive_name, "rb+");
    if (!arq) {
        arq = fopen(archive_name, "wb+");
        if (!arq) {
            perror("Erro ao abrir ou criar o arquivo archive");
            exit(1);
        }
    }

    interpreta_diretorio(arq, &diretorio, &qtde_membros);

    membros *novo_membro = alloc_membro();

    while ((option = getopt(argc, argv, "i:p:m:x:r:c")) != -1) {
        switch (option) {
            case 'i': { // Inserir membros
                printf("-i em implementação...\n");
                break;
            }
            case 'p': // Placeholder para a funcionalidade -p
                printf("Inserindo membros...\n");
                for (int i = optind; i < argc; i++) {
                    inserir(novo_membro, archive_name, argv[i], &diretorio, &qtde_membros);
                }
                break;
            case 'm': // Placeholder para mover membros
                printf("-m em implementação...\n");
                break;
            case 'x': // Placeholder para extrair membros
                printf("-x em implementação...\n");
                break;
            case 'r': // Placeholder para remover membros
                printf("-r em implementação...\n");
                break;
            case 'c': // Listar conteúdo do diretório
                printf("Listando conteúdo do arquivo %s:\n", archive_name);
                for (int i = 0; i < qtde_membros; i++) {
                    printf("Membro %d: %s, UID: %u, Tamanho: %zu bytes, Offset: %zu\n",
                           i + 1,
                           diretorio[i]->nome_do_membro,
                           diretorio[i]->UID,
                           diretorio[i]->tamanho_original,
                           diretorio[i]->offset);
                }
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