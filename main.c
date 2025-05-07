#define _GNU_SOURCE
#define MAX_MEMBER 50
#include "archiver.h"

int main(int argc, char *argv[]){
    int option;
    char *archive_name;
    //FILE *arq = NULL;
    membros *membro = alloc_membro();

    membros *diretorio[MAX_MEMBER];

    while ((option = getopt(argc, argv, "p:i:m:x:r:c")) != -1) {
        switch (option) {
            case 'p':
                archive_name = strcat(strdup(optarg), ".vc");

                for (int i = optind; i < argc; i++) {
                    inserir(membro, archive_name, argv[i]);  // Inserir cada membro
                }
                break;
            case 'i':
                printf("-i em implementacao...\n");
                break;
            case 'm':
                printf("-m em implementacao...\n");
                break;
            case 'x':
                printf("-x em implementacao...\n");
                break;
            case 'r':
                printf("-r em implementacao...\n");
                break;
            case 'c':
                printf("-c em implementacao...\n");
                break;
            default:
                fprintf(stderr, "Forma de uso: vinac <opção> <archive> [membro1 membro2 ...]\n");
                exit(1);
        }
    }   

}