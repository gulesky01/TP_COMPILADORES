%{
#include "sintatico.h"
#include <string.h>
/* para atribuir lexema ao yylval */
extern int yylval;
%}

DIGITO      [0-9]
LETRA       [A-Za-z_]
ID          {LETRA}({LETRA}|{DIGITO})*

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

/* operadores e pontuação */
"<="                { return ATRIB; }   /* conforme PDF, '<=' usado para atribuicao */
"<>"                { return DIF; }
">="                { return MAIG; }
"<"                 { return MEN; }
">"                 { return MAI; }
"="                 { return IG; }

"+"                 { yylval = 0; return MAIS; }
"-"                 { yylval = 0; return MENOS; }
"*"                 { yylval = 0; return MULT; }
"/"                 { yylval = 0; return DIVI; }

"("                 { return AP; }
")"                 { return FP; }
","                 { return VIRG; }
";"                 { return PTV; }

{ID} {
    yylval = 0;
    /* comparar palavras reservadas já feitas acima, então aqui é identificador */
    yylval = (int) strdup(yytext);
    return IDENT;
}

{DIGITO}+ {
    yylval = atoi(yytext);
    return NUM;
}

[ \t\r\n]+   { /* ignora espaços e quebras */ }
.           { /* caractere desconhecido */ }

%%

int yywrap(void) { return 1; }
