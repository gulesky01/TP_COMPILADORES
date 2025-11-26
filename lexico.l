%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sintatico.h" /* header gerado pelo bison (-d) */
%}

/* definicoes */
DIGITO  [0-9]
LETRA   [A-Za-z_]
ID      {LETRA}({LETRA}|{DIGITO})*

%%

"programa"          { return PROGR; }
"inteiro"           { return INTEIRO; }
"inicio"            { return INICIO; }
"fimprograma"       { return FIMPROG; }
"leia"              { return LEIA; }
"escreva"           { return ESCREVA; }
"enquanto"          { return ENQUANTO; }
"faca"              { return FACA; }
"fimenquanto"       { return FIMENQ; }
"se"                { return SE; }
"entao"             { return ENTAO; }
"senao"             { return SENAO; }
"fimse"             { return FIMSE; }
"nao"               { return NAO; }

/* operadores e simbolos */
"<="                { return ATRIB; }   /* atribuição conforme enunciado */
"<>"                { return DIF; }
">="                { return MAIG; }
"<"                 { return MEN; }
">"                 { return MAI; }
"="                 { return IG; }

"+"                 { return MAIS; }
"-"                 { return MENOS; }
"*"                 { return MULT; }
"/"                 { return DIVI; }

"("                 { return AP; }
")"                 { return FP; }
","                 { return VIRG; }
";"                 { return PTV; }

/* identificador e numero com valores semanticos */
{ID}    {
            yylval.sval = strdup(yytext);
            return IDENT;
        }

{DIGITO}+ {
            yylval.ival = atoi(yytext);
            return NUM;
         }

[ \t\r\n]+   { /* ignora */ }
.           { /* caractere desconhecido: ignora ou reporta */ }

%%

int yywrap(void) { return 1; }
