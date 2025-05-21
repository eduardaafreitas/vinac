Desenvolvido por Eduarda de Aguiar Freitas, GRR20211799.

Lista de arquivos e diretórios contidos no pacote:
- archiver.c: biblioteca principal com as implementações
- archiver.h: arquivo header da biblioteca principal, com a struct utilizada
- lz.c: compressor
- lz.h: compressor
- main.c: arquivo principal

A estrutura utilizada é a Membros, definida da seguinte forma:
struct membros {
    char *nome_do_membro;
    unsigned int UID;
    unsigned long tamanho_original;
    unsigned long tamanho_disco;
    time_t data_modificacao;
    unsigned int ordem_arquivo;
    unsigned long offset;
};

Com essa estrutura, optei por fazer um vetor de membros para ser o diretório e o mesmo será encontrado no início do archive. Inicialmente estava sendo implementada uma lista duplamente encadeada para fazer a função de diretório, mas a ideia foi descartada pela complexidade de implementação. Atualmente as opções funcionais são:
-p: inserção sem compressão;
-c: listagem de membros contidos no archive;
As outras opções exigidas pelo enunciado do trabalho não foram implementadas devido à problemas pessoais ao decorrer do trabalho. 
Durante a execução do trabalho, tive muita dificuldade em entender realmente o que o enunciado pedia, isso me prejudicou visto que não consegui iniciar a implementação corretamente do arquivador. A maior dificuldade encontrada durante a implementação foi na parte de interpretar o diretório pois demorei muito tempo para entender realmente o que ele precisava fazer e também demorei a entender como eu iria de fato implementá-lo.

Um bug conhecido é que o programa não guarda o offset corretamente e acabei não conseguindo encontrar o motivo.
