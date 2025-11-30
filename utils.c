/*+=============================================================
 |             UNIFAL - Universidade Federal de Alfenas .
 |               BACHARELADO EM CIENCIA DA COMPUTACAO.
 | Trabalho..: Construcao Arvore Sintatica e Geracao de Codigo
 | Disciplina: Teoria de Linguagens e Compiladores
 | Professor.: Luiz Eduardo da Silva
 | Aluno.....: Gustao Andrade Moreira de Assis - 2024.1.08.012
 | Data......: 30/11/2025
 +=============================================================*/

#include <stdio.h>
#include <stdlib.h>
#include "tree.h"

int ROTULO = 0;
int NUMVAR = 0;

#define MAXP 1000
static int pilha[MAXP];
static int top = 0;

void yyerror(const char *s); 

void empilha(int x)
{
    if (top < MAXP)
        pilha[top++] = x;
    else
    {
        fprintf(stderr, "empilha: overflow\n");
        exit(1);
    }
}

int desempilha(void)
{
    if (top > 0)
        return pilha[--top];
    fprintf(stderr, "desempilha: underflow\n");
    return -1;
}

void testaTipo(int tipo1, int tipo2, int ret)
{
    int t1 = desempilha();  // último empilhado  (operando direito)
    int t2 = desempilha();  // penúltimo         (operando esquerdo)

    if (t1 != tipo1 || t2 != tipo2)
        yyerror("Incompatibilidade de tipo!");

    empilha(ret);
}