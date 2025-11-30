%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"

extern int yylex();
extern FILE *yyin;
void yyerror(const char *s);

ptno raiz = NULL;
%}

/* Tipos Semânticos */
%union {
    int ival;
    char *sval;
    ptno no;
}

/* Tokens (usando o mesmo prefixo T_ que o lexico.l retorna) */
%token T_PROGR
%token T_INICIO 
%token T_FIMPROG
%token T_LEIA
%token T_ESCREVA
%token T_SE
%token T_ENTAO
%token T_SENAO
%token T_FIMSE
%token T_ENQUANTO
%token T_FACA
%token T_FIMENQ
%token T_INTEIRO
%token T_LOGICO
%token T_CONST_V
%token T_CONST_F
%token T_E_LOGICO
%token T_OU_LOGICO
%token T_NAO_LOGICO
%token T_DIVI_PALAVRA

%token T_ATRIB
%token T_DIF
%token T_MAIG
%token T_MAI
%token T_MEN
%token T_IG
%token T_MAIS
%token T_MENOS
%token T_MULT
%token T_DIVI
%token T_AP
%token T_FP
%token T_VIRG
%token T_PTV

%token <sval> T_IDENT
%token <ival> T_NUM

/* Tipos de não-terminais que carregam ptno (árvore) */
%type <no> programa dvr lista_var lista_cmd comando condicao expr

%%

/* ======================= PROGRAMA ======================= */
programa:
    T_PROGR T_IDENT dvr T_INICIO lista_cmd T_FIMPROG
    {
        ptno p = criaNo(PRG, 0, NULL);

        ptno id = criaNo(ID, 0, $2);
        adicionaFilho(p, id);

        if ($3) adicionaFilho(p, $3);
        if ($5) adicionaFilho(p, $5);

        raiz = p;
    }
;


/* ===================== DECLARACAO ===================== */
dvr:
    T_INTEIRO lista_var
    {
        ptno p = criaNo(DVR, 0, NULL);
        ptno tipo = criaNo(TIPO, 0, NULL);

        adicionaFilho(p, tipo);
        adicionaFilho(p, $2);

        $$ = p;
    }
    | /* vazio */ { $$ = NULL; }
;


/* ===================== LISTA VARIAVEIS ===================== */
lista_var:
    T_IDENT
    {
        ptno p = criaNo(LVAR, 0, NULL);
        ptno id = criaNo(ID, 0, $1);
        adicionaFilho(p, id);
        $$ = p;
    }
    | lista_var T_VIRG T_IDENT
    {
        ptno id = criaNo(ID, 0, $3);
        adicionaFilho($1, id);
        $$ = $1;
    }
;


/* ===================== LISTA DE COMANDOS ===================== */
lista_cmd:
    comando lista_cmd
    {
        ptno p = criaNo(LCM, 0, NULL);
        adicionaFilho(p, $1);   // comando
        adicionaFilho(p, $2);   // lista_cmd
        $$ = p;
    }
    | comando
    {
        ptno p = criaNo(LCM, 0, NULL);
        adicionaFilho(p, $1);
        $$ = p;
    }
    ;


/* ========================= COMANDOS ========================= */
comando:
    T_LEIA T_IDENT T_PTV
    {
        ptno p = criaNo(LEI, 0, NULL);
        ptno id = criaNo(ID, 0, $2);
        adicionaFilho(p, id);
        $$ = p;
    }
    |
    T_ESCREVA expr T_PTV
    {
        ptno p = criaNo(ESC,0,NULL);
        adicionaFilho(p, $2);
        $$ = p;
    }
    |
    T_IDENT T_ATRIB expr T_PTV
    {
        ptno p = criaNo(ATR,0,NULL);
        ptno id = criaNo(ID,0,$1);

        adicionaFilho(p, id);
        adicionaFilho(p, $3);

        $$ = p;
    }
    |
    T_ENQUANTO condicao T_FACA lista_cmd T_FIMENQ
    {
        ptno p = criaNo(REP,0,NULL);

        adicionaFilho(p, $2);  /* cond */
        adicionaFilho(p, $4);  /* bloco */

        $$ = p;
    }
    |
    T_SE condicao T_ENTAO lista_cmd T_SENAO lista_cmd T_FIMSE
    {
        ptno p = criaNo(SELEC,0,NULL);

        adicionaFilho(p, $2);  /* cond */
        adicionaFilho(p, $4);  /* entao */
        adicionaFilho(p, $6);  /* senao */

        $$ = p;
    }
    |
    T_SE condicao T_ENTAO lista_cmd T_FIMSE
    {
        ptno p = criaNo(SELEC,0,NULL);

        adicionaFilho(p, $2);  /* cond */
        adicionaFilho(p, $4);  /* entao */

        $$ = p;
    }
;


/* ======================== CONDICAO ======================== */
condicao:
    expr T_MEN expr
    {
        ptno p = criaNo(COMP,0,"<");
        adicionaFilho(p,$1);
        adicionaFilho(p,$3);
        $$ = p;
    }
    | expr T_MAI expr
    {
        ptno p = criaNo(COMP,0,">");
        adicionaFilho(p,$1);
        adicionaFilho(p,$3);
        $$ = p;
    }
    | expr T_IG expr
    {
        ptno p = criaNo(COMP,0,"=");
        adicionaFilho(p,$1);
        adicionaFilho(p,$3);
        $$ = p;
    }
    | expr T_DIF expr
    {
        ptno p = criaNo(COMP,0,"<>");
        adicionaFilho(p,$1);
        adicionaFilho(p,$3);
        $$ = p;
    }
    | T_NAO_LOGICO T_AP expr T_FP
    {
        ptno p = criaNo(NO_NAO,0,NULL);
        adicionaFilho(p,$3);
        $$ = p;
    }
;


/* ===================== EXPRESSÕES ===================== */
expr:
    T_NUM
    {
        $$ = criaNo(NO_NUM,$1,NULL);
    }
    |
    T_IDENT
    {
        $$ = criaNo(ID,0,$1);
    }
    |
    expr T_MAIS expr
    {
        ptno p = criaNo(OPBIN,0,"+");
        adicionaFilho(p,$1);
        adicionaFilho(p,$3);
        $$ = p;
    }
    |
    expr T_MENOS expr
    {
        ptno p = criaNo(OPBIN,0,"-");
        adicionaFilho(p,$1);
        adicionaFilho(p,$3);
        $$ = p;
    }
    |
    expr T_MULT expr
    {
        ptno p = criaNo(OPBIN,0,"multiplica");
        adicionaFilho(p,$1);
        adicionaFilho(p,$3);
        $$ = p;
    }
    |
    expr T_DIVI expr
    {
        ptno p = criaNo(OPBIN,0,"/");
        adicionaFilho(p,$1);
        adicionaFilho(p,$3);
        $$ = p;
    }
    |
    T_AP expr T_FP
    { $$ = $2; }
;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Erro sintático: %s\n", s);
}

int main(int argc, char **argv) {

    if (argc < 2) {
        fprintf(stderr, "Uso: %s <arquivo.simples>\n", argv[0]);
        return 1;
    }

    yyin = fopen(argv[1], "r");
    if (!yyin) {
        perror("fopen");
        return 1;
    }

    if (yyparse() == 0) {

        char nomebase[256];
        strncpy(nomebase, argv[1], sizeof(nomebase)-1);
        nomebase[sizeof(nomebase)-1] = '\0';

        char *dot = strrchr(nomebase, '.');
        if (dot) *dot = '\0';

        char dotname[300];
        snprintf(dotname, sizeof(dotname), "%s.dot", nomebase);

        // Gera DOT
        geraDot(raiz, dotname);

        // Gera SVG
        char svgname[300];
        snprintf(svgname, sizeof(svgname), "%s.svg", nomebase);

        char cmd[2048];
        snprintf(cmd, sizeof(cmd),
                 "dot -Tsvg \"%s\" -o \"%s\"",
                 dotname, svgname);

        system(cmd);

        // Gera MVS
        char mvsname[300];
        snprintf(mvsname, sizeof(mvsname), "%s.mvs", nomebase);

        FILE *fm = fopen(mvsname, "w");
        if (fm) {
            geracod(raiz, fm);
            fclose(fm);
        } else {
            perror("fopen mvs");
        }

        printf("Gerado: %s, %s, %s\n", dotname, svgname, mvsname);
    }
    else {
        fprintf(stderr, "Erros sintáticos. Nenhum arquivo gerado.\n");
    }

    return 0;
}
