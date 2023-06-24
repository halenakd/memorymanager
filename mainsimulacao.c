#include "gerenciador.h"

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>


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

    // Testar alocação e liberação de memória
    //int* ptr1 = 0;
    //int* ptr1 = (int*)kalloc(&ptr1 + 1200, sizeof(int));
    /*if (ptr1 != NULL) {
        *ptr1 = 10;
        printf("Valor de ptr1: %d\n", *ptr1);
        //kfree(ptr1);
    }*/

    /*char* ptr2 = (char*)kalloc(&ptr2, 5 * sizeof(char));
    if (ptr2 != NULL) {
        strcpy(ptr2, "Hello");
        printf("Valor de ptr2: %s\n", ptr2);
        //kfree(ptr2);
    }

    double* ptr3 = (double*)kalloc(&ptr3, 3 * sizeof(double));
    if (ptr3 != NULL) {
        for (int i = 0; i < 3; i++) {
            ptr3[i] = i + 0.5;
            printf("Valor de ptr3[%d]: %f\n", i, ptr3[i]);
        }
        //kfree(ptr3);
    }*/

    //lpa_getNode(memInfo->lpa);

    //debug();

    void *bla = kalloc(memBase + 4096, 2048);
    //void *bla1 = kalloc(memBase + 4096, 2048);
    //void *bla2 = kalloc(memBase + 8296, 2048);
    void *bla3 = kalloc(memBase + 6146, 2048);
    void *bla2 = kalloc(memBase + 8296, 2048);
    /*void *bla1 = 0;
    void *bla2 = 0;
    void *bla3 = 0;
    void *bla4 = 0;*/

    //bla = kalloc(memBase + 4096, 2048);
    //kalloc(bla + 12000, 2300);
    //kalloc(bla + 7068, 3200);
    //kalloc(bla + 9800, 1000);
    //lpa_marcarUsado(bla + 4095, 2048);*/

    lpa_printfLpa(memInfo->lpa);

    //kfree(memBase + 4096);

    /*void *endereco = memBase;

    printf("memBase na main: %p\n", endereco);

    Node * no = find(endereco + 1024, &(memInfo->lpa->nodes[0]));

    list_printNode(no);*/

    finalizar();

    return 0;
}