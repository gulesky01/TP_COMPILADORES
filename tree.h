/*+=============================================================
 | UNIFAL = Universidade Federal de Alfenas .
 | BACHARELADO EM CIENCIA DA COMPUTACAO.
 | Trabalho . . : Construcao Arvore Sintatica e Geracao de Codigo
 | Disciplina : Teoria de Linguagens e Compiladores
 | Professor . : Luiz Eduardo da Silva
 | Aluno . . . : (preencher)
 | Data . . . : 99/99/9999
 +=============================================================*/

#ifndef TREE_H
#define TREE_H

#include <stdio.h>

typedef struct no *ptno;

typedef struct no
{
    int tipo;
    int valor;    /* para números */
    char *lexema; /* para identificadores / texto */
    ptno filho, irmao;
} no;

ptno criaNo(int tipo, int valor, const char *lexema);
void adicionaFilho(ptno pai, ptno filho);

void geraDot(ptno raiz, const char *filename);
void geracod(ptno p, FILE *out);

/* Tipos de nó */
enum
{
    /* Estrutura do programa */
    PRG,  /* programa */
    ID,   /* identificador (nome / variável) */
    DVR,  /* declaração de variáveis */
    TIPO, /* tipo (inteiro) */
    LVAR, /* lista de variáveis */
    LCM,  /* lista de comandos */

    /* comandos */
    LEI,   /* leitura */
    ESC,   /* escrita */
    ATR,   /* atribuicao */
    REP,   /* repeticao (enquanto) */
    SELEC, /* selecao (se entao senao) */
    NAO,   /* negacao */

    /* expressões */
    COMP,  /* comparacao (<, >, =, <>) - lexema guarda operador */
    OPBIN, /* operador binario (+ - * /) - lexema guarda operador */
    NUM    /* numero */
};

/* utilitários / geracao */
extern int ROTULO;
extern int NUMVAR;

void empilha(int x);
int desempilha(void);

#endif /* TREE_H */
