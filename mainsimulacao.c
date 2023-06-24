#include "gerenciador.h"

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <strings.h>


/* -----VARIAVEIS GLOBAIS----- */

//#define TAMANHO_MEMORIA 512 * 1024 * 1024
#define TAMANHO_MEMORIA 1 << 30
#define PAGE_SIZE 1 << 14


/* -----VARIAVEIS EXTERNAS----- */

extern void *memBase;
extern MemInfo *memInfo;


/* -----DEFINICOES----- */

/* struct so para testar o kalloc */
struct bla
{
    int a;
    float f;
};


// posicao na memoria pra guardar as listas
// prealocar um monte
// posicao com pilha dos ponteiros/nos livres


int main()
{
    setlocale(LC_ALL, "pt_BR.UTF-8"); // Define o ambiente local para português do Brasil com UTF-8

    inicializar(TAMANHO_MEMORIA);

    debug();

    //struct bla b1 = 
    void *bla = 0;
    kalloc(bla + 4095, 2048);
    //lpa_marcarUsado(bla + 4095, 2048);
    lpa_printfLpa(memInfo->lpa);

    kfree(bla + 4095);

    /*void *endereco = memBase;

    printf("memBase na main: %p\n", endereco);

    Node * no = find(endereco + 1024, &(memInfo->lpa->nodes[0]));

    list_printNode(no);*/

    finalizar();

    return 0;
}