/* utils.c - pilha de rotulos e contadores simples */

#include <stdio.h>
#include <stdlib.h>
#include "tree.h"

int ROTULO = 0;
int NUMVAR = 0;

/* pilha simples */
#define MAXP 1000
static int pilha[MAXP];
static int top = 0;

void empilha(int x) {
    if (top < MAXP) pilha[top++] = x;
    else {
        fprintf(stderr,"empilha: overflow\n");
        exit(1);
    }
}

int desempilha(void) {
    if (top > 0) return pilha[--top];
    fprintf(stderr,"desempilha: underflow\n");
    return -1;
}
