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

typedef struct no {
    int tipo;
    int valor;      /* para números (se necessário) */
    char *lexema;   /* identifica/armazenar identificadores ou texto */
    ptno filho, irmao;
} no;

ptno criaNo(int tipo, int valor, char *lexema);
void adicionaFilho(ptno pai, ptno filho);

void geraDot(ptno raiz, const char *filename);
void geracod(ptno p, FILE *out);

/* Tipos de nó (enum) */
enum {
    PRG,      /* programa */
    ID,       /* identificador (nome do programa / variavel) */
    DVR,      /* declaracao de variaveis */
    TIPO,     /* tipo (inteiro) */
    LVAR,     /* lista variaveis */
    LCM,      /* lista comandos */
    LEI,      /* leitura */
    ESC,      /* escrita */
    ATR,      /* atribuicao */
    REP,      /* repeticao (enquanto) */
    SELEC,    /* selecao (se .. entao .. senao .. fimse) */
    NAO,      /* negacao (NAO) */
    COMP,     /* comparacao binaria */
    OPBIN,    /* operador binario aritmetico (+ - * /) */
    NUM,      /* numero */
    MUL,      /* multiplicacao (pode usar OPBIN) */
    DIVI,     /* divisao */
    SOMA,     /* soma */
    SUB,      /* subtracao */
    /* outras categorias utilitárias */
    LISTA_COMANDOS,
    LISTA_VARS,
    /* tokens terminais para lexemas específicos */
    PROG_NAME
};

/* funções utilitárias usadas por outros módulos (definidas em utils.c) */
extern int ROTULO;   /* contador de rótulos */
extern int NUMVAR;   /* contador de variáveis (se necessário) */

void empilha(int x);
int desempilha(void);

#endif /* TREE_H */
