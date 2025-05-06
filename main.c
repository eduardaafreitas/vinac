
#define _GNU_SOURCE
#include "archiver.h"

int main(int argc, char *argv[]){
    int option;
    char *archive_name;
    FILE *arq = NULL;
    membros *membro = alloc_membro();

    while ((option = getopt(argc, argv, "p:i:m:x:r:c")) != -1) {
        switch (option) {
            case 'p':
                archive_name = strdup(optarg);
                inserir(membro);
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