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

/* tipos semanticos */
%union {
    int ival;
    char *sval;
    ptno no;
}

/* tokens */
%token PROGR INTEIRO INICIO FIMPROG LEIA ESCREVA ENQUANTO FACA FIMENQ
%token SE ENTAO SENAO FIMSE NAO
%token ATRIB DIF MAIG MEN MAI IG
%token AP FP VIRG PTV
%token MAIS MENOS MULT DIVI
%token <sval> IDENT
%token <ival> NUM

/* tipos de não-terminais */
%type <no> programa dvr lista_var lista_cmd comando condicao expr

%%

programa:
    PROGR IDENT dvr INICIO lista_cmd FIMPROG
    {
        ptno p_prog = criaNo(PRG,0,NULL);
        ptno p_id = criaNo(ID,0,$2);
        adicionaFilho(p_prog, p_id);
        if ($3) adicionaFilho(p_prog, $3);   /* DVR como filho do PRG */
        if ($5) adicionaFilho(p_prog, $5);   /* LCM como filho do PRG */
        raiz = p_prog;
    }
    ;

dvr:
    INTEIRO lista_var
    {
        ptno p = criaNo(DVR,0,NULL);
        ptno p_tipo = criaNo(TIPO,0,NULL);
        adicionaFilho(p, p_tipo);
        if ($2) adicionaFilho(p, $2); /* lista de variaveis como irmao do tipo */
        $$ = p;
    }
    | /* vazio */
    { $$ = NULL; }
    ;

lista_var:
    IDENT
    {
        ptno p = criaNo(LVAR,0,NULL);
        ptno id = criaNo(ID,0,$1);
        adicionaFilho(p, id);
        $$ = p;
    }
    | lista_var VIRG IDENT
    {
        ptno p = $1;
        ptno id = criaNo(ID,0,$3);
        adicionaFilho(p, id);
        $$ = p;
    }
    ;

lista_cmd:
    comando
    {
        ptno p = criaNo(LCM,0,NULL);
        adicionaFilho(p, $1);
        $$ = p;
    }
    | lista_cmd comando
    {
        ptno p = $1;
        adicionaFilho(p, $2);
        $$ = p;
    }
    | /* vazio */
    { $$ = NULL; }
    ;

comando:
    LEIA IDENT PTV
    {
        ptno p = criaNo(LEI,0,NULL);
        ptno id = criaNo(ID,0,$2);
        adicionaFilho(p, id);
        $$ = p;
    }
    | ESCREVA expr PTV
    {
        ptno p = criaNo(ESC,0,NULL);
        adicionaFilho(p, $2);
        $$ = p;
    }
    | IDENT ATRIB expr PTV
    {
        ptno p = criaNo(ATR,0,NULL);
        ptno id = criaNo(ID,0,$1);
        adicionaFilho(p, id);       /* ATR->filho = id */
        /* ligar id->irmao = expr para que ATR->filho=id e id->irmao=expr */
        id->irmao = $3;
        $$ = p;
    }
    | ENQUANTO condicao FACA lista_cmd FIMENQ
    {
        ptno p = criaNo(REP,0,NULL);
        adicionaFilho(p, $2);          /* p->filho = cond */
        /* ligar cond->irmao = lista_cmd */
        $2->irmao = $4;
        $$ = p;
    }
    | SE condicao ENTAO lista_cmd SENAO lista_cmd FIMSE
    {
        ptno p = criaNo(SELEC,0,NULL);
        adicionaFilho(p, $2);          /* cond */
        $2->irmao = $4;                /* cond->irmao = entao */
        $4->irmao = $6;                /* entao->irmao = senao */
        $$ = p;
    }
    | SE condicao ENTAO lista_cmd FIMSE
    {
        ptno p = criaNo(SELEC,0,NULL);
        adicionaFilho(p, $2);
        $2->irmao = $4;
        $$ = p;
    }
    ;

condicao:
    expr MEN expr   { ptno p = criaNo(COMP,0,"<"); adicionaFilho(p,$1); $1->irmao = $3; $$ = p; }
    | expr MAI expr { ptno p = criaNo(COMP,0,">"); adicionaFilho(p,$1); $1->irmao = $3; $$ = p; }
    | expr IG expr  { ptno p = criaNo(COMP,0,"="); adicionaFilho(p,$1); $1->irmao = $3; $$ = p; }
    | expr DIF expr { ptno p = criaNo(COMP,0,"<>"); adicionaFilho(p,$1); $1->irmao = $3; $$ = p; }
    | NAO AP expr FP { ptno p = criaNo(NAO,0,NULL); adicionaFilho(p,$3); $$ = p; }
    ;

expr:
    NUM   { ptno p = criaNo(NUM,$1,NULL); $$ = p; }
    | IDENT { ptno p = criaNo(ID,0,$1); $$ = p; }
    | expr MAIS expr { ptno p = criaNo(OPBIN,0,"+"); adicionaFilho(p,$1); $1->irmao = $3; $$ = p; }
    | expr MENOS expr { ptno p = criaNo(OPBIN,0,"-"); adicionaFilho(p,$1); $1->irmao = $3; $$ = p; }
    | expr MULT expr { ptno p = criaNo(OPBIN,0,"*"); adicionaFilho(p,$1); $1->irmao = $3; $$ = p; }
    | expr DIVI expr { ptno p = criaNo(OPBIN,0,"/"); adicionaFilho(p,$1); $1->irmao = $3; $$ = p; }
    | AP expr FP { $$ = $2; }
    ;

%%

/* main: chama parser e escreve .dot e .mvs */
int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr,"Uso: %s <arquivo.simples>\n", argv[0]);
        return 1;
    }
    yyin = fopen(argv[1],"r");
    if (!yyin) { perror("fopen"); return 1; }
    if (yyparse() == 0) {
        char nomebase[256];
        strncpy(nomebase, argv[1], sizeof(nomebase)-1);
        nomebase[sizeof(nomebase)-1] = '\0';
        char *dot = strrchr(nomebase,'.');
        if (dot) *dot = '\0';
        char dotname[300];
        snprintf(dotname,sizeof(dotname),"%s.dot",nomebase);
        geraDot(raiz, dotname);
        char mvsname[300];
        snprintf(mvsname,sizeof(mvsname),"%s.mvs",nomebase);
        FILE *fm = fopen(mvsname,"w");
        if (fm) {
            geracod(raiz,fm);
            fclose(fm);
            printf("Arquivos gerados: %s , %s\n", dotname, mvsname);
        } else {
            perror("fopen mvs");
        }
    } else {
        fprintf(stderr,"Erros sintáticos. Nenhum arquivo gerado.\n");
    }
    return 0;
}

void yyerror(const char *s) {
    fprintf(stderr,"Erro sintatico: %s\n", s);
}
