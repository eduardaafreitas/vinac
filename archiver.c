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

    membro->anterior = NULL;
    membro->proximo = NULL;

    return membro;
}

void inserir(membros *membro){
    if(!membro)
    return -1;

    if (membro->anterior){
        membro->anterior->proximo = membro;
        membro->proximo = NULL;
        membro->anterior = NULL;
    } 

    return 0;
}
