
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "archiver.h"

int main(int argc, char *argv[]){
    int option;
    char *archive_name;
    FILE *arq = NULL;
    membros *membro = alloc_membro();

    while ((option = getopt(argc, argv, "p:i:m:x:r:c")) != -1) {
        switch (option) {
            case 'p':
                printf("Em implementacao...\n");
                archive_name = strdup(optarg);
                inserir(membro);
                break;
            case 'i':
                printf("Em implementacao...\n");
                break;
            case 'm':
                printf("Em implementacao...\n");
                break;
            case 'x':
                printf("Em implementacao...\n");
                break;
            case 'r':
                printf("Em implementacao...\n");
                break;
            case 'c':
                printf("Em implementacao...\n");
                break;
            default:
                fprintf(stderr, "Forma de uso: vinac <opção> <archive> [membro1 membro2 ...]\n");
                exit(1);
        }
    }   

}