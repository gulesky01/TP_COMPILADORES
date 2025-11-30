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
    int valor;    
    char *lexema; 
    ptno filho, irmao;
} no;


enum {
    TIPO_INTEIRO = 1,
    TIPO_LOGICO  = 2
};

ptno criaNo(int tipo, int valor, const char *lexema);
void adicionaFilho(ptno pai, ptno filho);


void geraDot(ptno raiz, const char *filename);
void geracod(ptno p, FILE *out);

enum
{
    
    PRG,
    ID,
    DVR,
    TIPO,
    LVAR,
    LCM,

  
    LEI,
    ESC,
    ATR,
    REP,
    SELEC,
    NO_NAO,

    
    COMP,
    OPBIN,
    NO_NUM
};


extern int ROTULO;
extern int NUMVAR;

void empilha(int x);
int desempilha(void);


void testaTipo(int tipo1, int tipo2, int ret);

#endif 