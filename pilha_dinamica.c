#include <stdio.h>
#include <stdlib.h>

typedef struct no {
    int dado;
    struct no* prox;
} No;

typedef struct pilha {
    No* topo;
} Pilha;

void push(Pilha *pilha, int valor) {
    No* no = (No *)malloc(sizeof(No));
    no->dado = valor;
    no->prox = pilha->topo;
    pilha->topo = no;
}

int pop(Pilha *pilha) {
    No *no = pilha->topo;

    int temp;
    if(no == NULL) {
        printf("Pilha vazia");
        return -1;
    }

    temp = no->dado;
    pilha->topo = no->prox;
    free(no);

    return temp;
}

void imprime(Pilha *pilha) {
    No *no = pilha->topo;
    while(no!= NULL) {
        printf("[%d] ", no->dado);

        no = no->prox;
    }
    printf("\n");
}

int main() {

    Pilha pilha;
    pilha.topo = NULL;
    int valor;
    char resp;

    do {

        printf("Numero: ");
        scanf("%d", &valor);
        push(&pilha, valor);

        while (getchar() != '\n');

        printf("CONTINUAR: S | N");
        scanf("%c", &resp);

    }while(resp == 'S' || resp == 's');


    imprime(&pilha);

    printf("remocao: %d", pop(&pilha));

    return 0;
}
