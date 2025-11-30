/*+=============================================================
 |             UNIFAL - Universidade Federal de Alfenas .
 |               BACHARELADO EM CIENCIA DA COMPUTACAO.
 | Trabalho..: Construcao Arvore Sintatica e Geracao de Codigo
 | Disciplina: Teoria de Linguagens e Compiladores
 | Professor.: Luiz Eduardo da Silva
 | Aluno.....: Gustao Andrade Moreira de Assis - 2024.1.08.012
 | Data......: 30/11/2025
 +=============================================================*/

#ifndef TREE_H
#define TREE_H

#include <stdio.h>

typedef struct no *ptno;

typedef struct no
{
    int tipo;
    int valor;    /* para números */
    char *lexema; /* para identificadores / texto / operadores */
    ptno filho, irmao;
} no;

/* Tipos semânticos para teste de tipos */
enum {
    TIPO_INTEIRO = 1,
    TIPO_LOGICO  = 2
};

ptno criaNo(int tipo, int valor, const char *lexema);
void adicionaFilho(ptno pai, ptno filho);

/* gerar árvore e código */
void geraDot(ptno raiz, const char *filename);
void geracod(ptno p, FILE *out);

/* Tipos de nó */
enum
{
    /* Estrutura do programa */
    PRG,
    ID,
    DVR,
    TIPO,
    LVAR,
    LCM,

    /* comandos */
    LEI,
    ESC,
    ATR,
    REP,
    SELEC,
    NO_NAO,

    /* expressões */
    COMP,
    OPBIN,
    NO_NUM
};

/* utilitários */
extern int ROTULO;
extern int NUMVAR;

void empilha(int x);
int desempilha(void);

/* função pedida pelo professor */
void testaTipo(int tipo1, int tipo2, int ret);

#endif /* TREE_H */