#include "gerenciador.h"


/* ----------SIMULACAO---------- */

/* alocando espaco para a simulacao das operacoes com memoria */
void *memBase = NULL;
MemInfo *memInfo = NULL;


void * criaMemoria(long tamanho)
{
    char *memoria = malloc(tamanho);
    return memoria;
}


void devolveMemoria(int *memoria)
{
    free(memoria);
}


/* ----------MEMORIA---------- */

void inicializar(long tamanho_)
{
    ListaPreAlocada *lpa = NULL;
    /* alocando espaco para a simulacao das operacoes com memoria */
    memBase = criaMemoria(tamanho_);
    memInfo = (MemInfo*)(memBase + MEMINFOADDR);
    memInfo->memTotal = tamanho_;
    memInfo->memLivre = memInfo->memTotal;

    lpa = (ListaPreAlocada*)(memBase + MEMINFOADDR + sizeof(MemInfo));  // aloca a primeira listaPreAlocada
    memInfo->lpa = lpa;
    /* inicializando a memoria, a lista de memorias, todo mundo, 
    * nesse ponto ele nao esta correto, pois memInfo e lpa nao estao marcados como uso */
    lpa_memoriaLivre(lpa, tamanho_, memBase); // marca toda a memoria como livre
    printf("\nFUNÇÃO INICIALIZAR\n");
    lpa_printfLpaNode(lpa); 
    lpa_marcarUsado(memInfo, sizeof(MemInfo)); // marca o inicio que tem as infos da memoria como usado
}


void finalizar()
{
    devolveMemoria(memBase);
}


void debug()
{
    printf("\nFUNÇÃO DEBUG\n");
    printf("memBase: %p\n", memBase);
}


void lpa_marcarUsado(void *ptr, unsigned long comprimento)
{
    Node * no = findParaAlocar(ptr, memInfo->base); // acha o no em que o ponteiro esta contido
    printf("\nFUNÇÃO MARCARUSADO\n");
    printf("no encontrado lpa_marcarusado: %p\n", ptr);
    list_printNode(no);
    /* o no foi encontrado? */
    if(no != NULL)
    {
        if(no->status == 'L') // se ele estiver livre
        {
            void *t1 = no->endereco + no->comprimento; // Fim do nó
            void *t2 = ptr + comprimento; // Fim da região ocupada que será marcada


            if(no->endereco < ptr) // sobra espaço livre antes
            {
                // tem espaço livre antes, mas nao tem espaco livre depois
                if(t1 == t2)
                {
                    /* nó da região usada que ficou depois do espaço livre */
                    Node *new = lpa_getNode(memInfo->lpa);
                    new->endereco = ptr;
                    new->comprimento = comprimento;
                    new->prev = no;
                    new->next = no->next;
                    new->status = 'O';

                    /* nó da região livre que sobrou antes */
                    no->comprimento = ptr - no->endereco; // atualiza o tamanho da regiao livre que restou
                    no->next = new;
                }
                // tem espaço livre antes e tem espaco livre depois
                else if(t2 < t1)
                {
                    /* nó ocupado que ficou no meio dos dois espaços livres */
                    Node *new = lpa_getNode(memInfo->lpa);
                    new->endereco = ptr;
                    new->comprimento = comprimento;
                    new->prev = no;
                    new->next = no->next;
                    new->status = 'O';

                    /* nó novo para a região livre que sobrou depois */
                    Node *newD = lpa_getNode(memInfo->lpa);
                    newD->endereco = t2 + 1;
                    newD->comprimento = no->comprimento - comprimento;
                    newD->prev = new;
                    newD->next = no->next;
                    newD->status = 'L';

                    /* nó já existente para a região livre que sobrou antes */
                    no->comprimento = ptr - no->endereco; // atualiza o tamanho da regiao livre que restou
                    no->next = newD; // antes tava = no
                }
            }
            else if(no->endereco == ptr) // começam no mesmo lugar, então nao sobra espaço antes, mas pode sobrar espaço livre depois da região alocada
            {
                // regiao ocupada todo o no, nao sobra espaço livre depois
                if(t1 == t2)
                    no->status = 'O';
                // sobra espaço livre depois
                else
                {
                    /* nó novo para a região livre que sobrou depois */
                    Node *new = lpa_getNode(memInfo->lpa);
                    new->endereco = t2 + 1;
                    new->comprimento = t1 - (t2);
                    new->prev = no;
                    new->next = no->next;
                    new->status = 'L';

                    /* nó já existente para a regiao ocupada */
                    no->comprimento = comprimento;
                    no->next = new;
                    no->status = 'O';
                }
            }
        }
        else
        {
            printf("\nFUNÇÃO MARCARUSADO\n");
            printf("Impossivel marcar ocupado, no ja esta ocupado \n");
        }    
    }
}


void lpa_devolverUsado(void *ptr)
{
    printf("\nFUNÇÃO DEVOLVERUSADO\n"); 
    Node * no = findParaDevolver(ptr, memInfo->base);

    if(no->next->status == 'L' && no->prev->status == 'L')
    {
        no->prev->comprimento = no->prev->comprimento + no->comprimento + no->next->comprimento;
        lpa_devolverNode(no);
        lpa_devolverNode(no->next);

        // da um free no no // remove ele da lista
        // da um free no no->next // remove ele da lista
    }
    else if(no->next->status == 'L')
    {
        no->next->comprimento = no->comprimento + no->next->comprimento;
        no->next->endereco = no->endereco;
        lpa_devolverNode(no);
        // da um free no no // remove ele da lista
    }
    else if(no->prev->status == 'L')
    {
        no->prev->comprimento = no->prev->comprimento + no->comprimento;
        lpa_devolverNode(no);
        // da um free no no // remove ele da lista
    }
    else // prev e next == 'O'
    {
        no->status = 'L';
    }
}



/* ----------KALLOC E KFREE---------- */

void *kalloc(void *endereco, size_t tamanho_)
{
    lpa_marcarUsado(endereco, tamanho_);
    printf("\nFUNÇÃO KALLOC\n");
    printf("Memoria alocada!\n");
    return endereco;
}


void kfree(void *ptr)
{
    lpa_devolverUsado(ptr);
    printf("Memoria liberada!\n");
}


/* ----------LISTAS---------- */

Node *findParaAlocar(void *endereco, Node *base)
{
    printf("\nFUNÇÃO FINDPARAALOCAR\n");
    while(base != NULL && !(base->endereco <= endereco && 
        endereco <= (base->endereco + base->comprimento) )) 
    {
        printf("bla\n");
        base = base->next;
    }
    return base;
}


Node *findParaDevolver(void *endereco, Node *base)
{
    printf("\nFUNÇÃO FINDPARADEVOLVER\n");
    while(base != NULL && endereco != base->endereco) 
    {
        printf("ble\n");
        base = base->next;
        printf("endereco %p", endereco);
        printf("base-endereco %p", base->endereco);
    }
    return base;
}


void lpa_init(ListaPreAlocada *lpa)
{
    lpa->livres = NPREALOCADO;
    bzero(lpa->bitmap, NBITMAP*sizeof(unsigned long));
    lpa->next = NULL;
}


void lpa_memoriaLivre(ListaPreAlocada *lpa, long tamanho_, void *memBase)
{
    lpa_init(lpa);
    Node *node = lpa_getNode(lpa);
    node->status = 'L';
    node->next = NULL;
    node->comprimento = tamanho_;
    node->endereco = memBase;
    node->prev = NULL;
    memInfo->base = node;
}


Node *lpa_getNode(ListaPreAlocada *lpa)
{
    /* testar se tem livres,
    se nao tiver alocar proximo no,
    se tiver procurar no bitmap, reduzir o numero de livres,
    marcar ocupado no bitmap e retornar o ponteiro para o cara certo */
    
    printf("\nFUNÇÃO GETNODE\n");
    // verificar se há nós livres na lista
    while(1)
    {
        printf("Livres comeco: %i\n", lpa->livres);
        // caso não tenha nenhum nó livre na lista, criar um nó novo
        if(lpa->livres == 0 && lpa->next == NULL)
        {
            ListaPreAlocada *newLpa = NULL;
            newLpa = (ListaPreAlocada*)(memBase + MEMINFOADDR + sizeof(MemInfo));
            memInfo->lpa->next = newLpa;
            lpa_init(newLpa);
            lpa_marcarUsado(newLpa, sizeof(ListaPreAlocada));
        }
        else
        {
            if(lpa->livres != 0)
            {
                unsigned long int posAtual = 0;
                // -1 num complemento de 2 eh tudo 1
                // verificar se o bitmap da posAtual possui todos os bits setados em 1
                // para encontrar um espaço livre para a alocação após sair do while
                while(lpa->bitmap[posAtual] == (unsigned long)-1)
                {
                    posAtual++;
                }
                // procurar o bit dentro do unsigned long
                unsigned long campo = lpa->bitmap[posAtual];
                // inverter todos os bits do campo, 0 vira 1 e 1 vira 0
                campo = ~campo;
                unsigned long int i = 0; // posCampo

                // verificar cada bit do campo para encontrar o primeiro bit com valor 1
                for(i = 0; i < sizeof(unsigned long) * 8 && !((campo >> i) & 0x1); i++);
                /*
                    pos = calculada com a posição do bloco de bits no bitmap (posAtual)
                    multiplicada pelo tamanho de um unsigned long em bits (sizeof(unsigned long) * 8)
                    somada ao índice do bit encontrado (i)
                */
                // representa o índice do  nó disponível na lista pré-alocada
                unsigned long pos = posAtual * sizeof(unsigned long) * 8 + i;
            
                // sinalizar que tem um no a menos disponivel
                --lpa->livres; // decrementa o número de nós livres na lista pré-alocada
                lpa->bitmap[posAtual] |= (0x1 << i); // marca o bit correspondente ao nó alocado como ocupado no bitmap
                lpa->nodes[pos].pos = pos; // atribui a posição pos ao campo pos do nó alocado
                lpa->nodes[pos].pai = lpa; // atribui o ponteiro para a lista pré-alocada lpa ao campo pai do nó alocado
            
                printf("\nFUNÇÃO GETNODE\n");
                printf("pos %lu\n", pos); // imprimir a posição do nó alocado
                printf("lpa->bitmap[posAtual] %lu\n", lpa->bitmap[posAtual]); // imprimir o estado atual do bitmap
            
                printf("Livres fim: %i\n", lpa->livres);
                return &(lpa->nodes[pos]); // retorna o endereço de memória do nó alocado
            }
            else
            {
                lpa = lpa->next;
            }
        }
    }
    return 0;
}


void lpa_devolverNode(Node *node)
{
    if (node == NULL)
        return;

    ListaPreAlocada *lpa = node->pai;

    // Obtenha a posição do nó no bitmap
    unsigned long pos = node->pos;

    // Calcule a posição do bit no bitmap
    unsigned long posAtual = pos / (sizeof(unsigned long) * 8);

    // Calcule o deslocamento do bit dentro do unsigned long
    unsigned long offset = pos % (sizeof(unsigned long) * 8);

    // Desmarque o bit correspondente no bitmap
    lpa->bitmap[posAtual] &= ~(1UL << offset);

    // Incremente o número de nós livres na lista pré-alocada
    ++lpa->livres;

    // Limpe os campos do nó devolvido
    node->pos = 0;
    node->pai = NULL;
}


/* ----------PRINTS---------- */

void list_printNode(Node *n)
{
    if(n != NULL)
    {
        printf("\nFUNÇÃO PRINTNODE\n");
        printf("status: %c, ", n->status);
        printf("endereco: %p, ", n->endereco);
        printf("comprimento: %lu, ", n->comprimento);
        printf("pos: %lu, ", n->pos);
        printf("pai: %p, ", n->pai);
        printf("next: %p, ", n->next);
        printf("prev: %p\n", n->prev);        
    }
    else
    {
        printf("\nFUNÇÃO PRINTNODE\n");
        printf("Node eh nulo\n");
    }
}


void lpa_printfLpa(ListaPreAlocada *lpa)
{
    printf("\nFUNÇÃO PRINTLPA\n");
    while(lpa != NULL)
    {
        lpa_printfLpaNode(lpa);
        lpa = lpa->next;
    }
}


void lpa_printfLpaNode(ListaPreAlocada *lpa)
{
    printf("\nFUNÇÃO PRINTLPANODE\n");
    printf("este lpa: %p\n", lpa);
    printf("livres: %d\n", lpa->livres);
    printf("bitmap: \n");
    for(unsigned long i = 0; i < NBITMAP; i++)
    {
        printf("%ld %lx\n", i, lpa->bitmap[i]);
    }
    printf("\n");
    for(unsigned long i = 0; i < NPREALOCADO; i++)
    {
        printf("%lu: ", i);
        unsigned long pos = i / (sizeof(long) * 8);
        unsigned long pBit = i % (sizeof(long) * 8); // posicao do bit
        unsigned long vBit = (lpa->bitmap[pos] >> pBit) & 0x1; // valor do bit
        if(vBit)
        {
            list_printNode(&(lpa->nodes[i]));
        }
        else
        {
            printf("vazio");
        }
        printf("\n");
    }
    printf("next lpa: %p\n", lpa->next);
}