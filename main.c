#include "archiver.h"

int main(int argc, char *argv[]){
    int option;
    FILE *arq = NULL;
    
    while ((option = getopt(argc, argv, "p:i:m:x:r:c")) != -1) {
        switch (option) {
            case 'p':
                printf("Em implementacao...\n");
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