/*+=============================================================
 |             UNIFAL - Universidade Federal de Alfenas .
 |               BACHARELADO EM CIENCIA DA COMPUTACAO.
 | Trabalho..: Construcao Arvore Sintatica e Geracao de Codigo
 | Disciplina: Teoria de Linguagens e Compiladores
 | Professor.: Luiz Eduardo da Silva
 | Aluno.....: Gustao Andrade Moreira de Assis - 2024.1.08.012
 | Data......: 30/11/2025
 +=============================================================*/


%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "tree.h"
#include "sintatico.h"

void yyerror(const char *s);
%}

/* DEFINICOES */
identificador   [A-Za-z][A-Za-z0-9]*
numero          [0-9]+


%%

"programa"      { return T_PROGR; }
"inicio"        { return T_INICIO; }
"fimprograma"   { return T_FIMPROG; }
"leia"          { return T_LEIA; }
"escreva"       { return T_ESCREVA; }
"se"            { return T_SE; }
"entao"         { return T_ENTAO; }
"senao"         { return T_SENAO; }
"fimse"         { return T_FIMSE; }
"enquanto"      { return T_ENQUANTO; }
"faca"          { return T_FACA; }
"fimenquanto"   { return T_FIMENQ; }
"inteiro"       { return T_INTEIRO; }
"logico"        { return T_LOGICO; }
"V"             { return T_CONST_V; }
"F"             { return T_CONST_F; }

"div"           { return T_DIVI_PALAVRA; }
"e"             { return T_E_LOGICO; }
"ou"            { return T_OU_LOGICO; }
"nao"           { return T_NAO_LOGICO; }

"<-"            { return T_ATRIB; }
"<>"            { return T_DIF; }
">="            { return T_MAIG; }
">"             { return T_MAI; }
"<"             { return T_MEN; }
"="             { return T_IG; }

"+"             { return T_MAIS; }
"-"             { return T_MENOS; }
"*"             { return T_MULT; }
"/"             { return T_DIVI; }

"("             { return T_AP; }
")"             { return T_FP; }
","             { return T_VIRG; }
";"             { return T_PTV; }

{identificador} {
    yylval.sval = strdup(yytext);
    return T_IDENT;
}

{numero} {
    yylval.ival = atoi(yytext);
    return T_NUM;
}

[ \t\r\n]+      { /* ignoraos espaços, tabs e enters */ }


.               { yyerror("ERRO LEXICO"); }

%%

int yywrap(void) { return 1; }
