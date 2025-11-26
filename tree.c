/* tree.c - construção da árvore e geração .dot + gerador MVS simples */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"

/* cria nó */
ptno criaNo(int tipo, int valor, char *lexema) {
    ptno n = (ptno) malloc(sizeof(struct no));
    n->tipo = tipo;
    n->valor = valor;
    if (lexema) n->lexema = strdup(lexema); else n->lexema = NULL;
    n->filho = NULL;
    n->irmao = NULL;
    return n;
}

/* adiciona filho (insere como primeiro filho, preservando irmao encadeado) */
void adicionaFilho(ptno pai, ptno filho) {
    if (!pai || !filho) return;
    filho->irmao = pai->filho;
    pai->filho = filho;
}

/* --- geração .dot --- */
static void dot_nodes(FILE *f, ptno p) {
    if (!p) return;
    /* imprime label conforme tipo */
    char label[256];
    switch (p->tipo) {
        case PRG: snprintf(label,sizeof(label),"programa | "); break;
        case ID: snprintf(label,sizeof(label),"identificador | %s", p->lexema?p->lexema:""); break;
        case DVR: snprintf(label,sizeof(label),"declaracao de variaveis | "); break;
        case TIPO: snprintf(label,sizeof(label),"tipo | inteiro"); break;
        case LVAR: snprintf(label,sizeof(label),"lista variaveis | "); break;
        case LCM: snprintf(label,sizeof(label),"lista comandos | "); break;
        case LEI: snprintf(label,sizeof(label),"leitura | "); break;
        case ESC: snprintf(label,sizeof(label),"escrita | "); break;
        case ATR: snprintf(label,sizeof(label),"atribuicao | "); break;
        case REP: snprintf(label,sizeof(label),"repeticao | "); break;
        case SELEC: snprintf(label,sizeof(label),"selecao | "); break;
        case NAO: snprintf(label,sizeof(label),"negacao (NAO) | "); break;
        case COMP: snprintf(label,sizeof(label),"compara | %s", p->lexema?p->lexema:""); break;
        case OPBIN: snprintf(label,sizeof(label),"opbin | %s", p->lexema?p->lexema:""); break;
        case NUM: snprintf(label,sizeof(label),"numero | %d", p->valor); break;
        default: snprintf(label,sizeof(label),"no | %s", p->lexema?p->lexema:""); break;
    }
    fprintf(f,"n%p [ label = \"%s\" ];\n", (void*)p, label);
    /* recursão filhos e irmãos */
    if (p->filho) dot_nodes(f, p->filho);
    if (p->irmao) dot_nodes(f, p->irmao);
}

/* imprime arestas pai -> filhos */
static void dot_edges(FILE *f, ptno p) {
    if (!p) return;
    ptno c = p->filho;
    while (c) {
        fprintf(f,"n%p -> n%p;\n", (void*)p, (void*)c);
        dot_edges(f, c);
        c = c->irmao;
    }
    if (p->irmao) dot_edges(f, p->irmao);
}

void geraDot(ptno raiz, const char *filename) {
    FILE *f = fopen(filename,"w");
    if (!f) {
        perror("geraDot: fopen");
        return;
    }
    fprintf(f,"digraph {\n");
    fprintf(f,"node [ shape=record, height=.1 ];\n");
    dot_nodes(f, raiz);
    dot_edges(f, raiz);
    fprintf(f,"}\n");
    fclose(f);
}

/* --- gerador MVS (simplificado conforme enunciado) --- */

/* rotulos e pilha são definidos em utils.c (ROTULO, empilha, desempilha) */
#include "tree.h"
extern int ROTULO;
extern int NUMVAR;
extern void empilha(int x);
extern int desempilha(void);

/* forward */
static void gen_expr(ptno p, FILE *out);

/* geracod: percorre e escreve no arquivo MVS */
void geracod(ptno p, FILE *out) {
    if (!p) return;
    ptno p1, p2, p3;
    switch (p->tipo) {
        case PRG:
            p1 = p->filho;         /* nome do programa */
            p2 = p1 ? p1->irmao : NULL; /* DVR */
            p3 = p2 ? p2->irmao : NULL; /* LCM */
            fprintf(out,"\tINPP\n");
            NUMVAR = 0;
            geracod(p2,out);
            geracod(p3,out);
            fprintf(out,"\tFIMP\n");
            break;

        case DVR: /* declaração de variáveis: filho = TIPO, irmão = lista variaveis? */
            /* apenas conta variáveis (simplificado) */
            {
                ptno tipo = p->filho;
                ptno lista = tipo ? tipo->irmao : NULL;
                ptno v = lista ? lista->filho : NULL;
                while (v) {
                    NUMVAR++;
                    v = v->irmao;
                }
            }
            break;

        case LCM:
            {
                ptno cmd = p->filho;
                while (cmd) {
                    geracod(cmd,out);
                    cmd = cmd->irmao;
                }
            }
            break;

        case LEI:
            /* filho é identificador */
            if (p->filho && p->filho->lexema) {
                fprintf(out,"\tLEIT %s\n", p->filho->lexema);
            } else {
                fprintf(out,"\t; LEIT ???\n");
            }
            break;

        case ESC:
            /* escritor espera expressão ou variável como filho */
            if (p->filho) {
                if (p->filho->tipo == NUM) {
                    fprintf(out,"\tCRCT\t%d\n", p->filho->valor);
                    fprintf(out,"\tIMPR\n");
                } else if (p->filho->tipo == ID) {
                    fprintf(out,"\tCRVL\t%s\n", p->filho->lexema);
                    fprintf(out,"\tIMPR\n");
                } else {
                    /* gerar expressão no topo da pilha e imprimir */
                    gen_expr(p->filho, out);
                    fprintf(out,"\tIMPR\n");
                }
            }
            break;

        case ATR: /* atribuicao: filho1 = identificador, filho2 = expr */
            if (p->filho && p->filho->irmao) {
                ptno var = p->filho;
                ptno expr = var->irmao;
                gen_expr(expr,out);
                if (var->lexema) {
                    fprintf(out,"\tARMZ\t%s\n", var->lexema);
                } else {
                    fprintf(out,"\t; ARMZ ?\n");
                }
            }
            break;

        case REP:
            /* repeticao: filho1 = cond (expressão comparativa), filho2 = LCM */
            {
                int l1 = ++ROTULO;
                int l2;
                fprintf(out,"L%d\tNADA\n", l1);
                empilha(l1);
                ptno cond = p->filho;
                ptno bloco = cond ? cond->irmao : NULL;
                /* gera condição (assumimos que cond deixa valor lógico no topo) */
                gen_expr(cond, out);
                l2 = ++ROTULO;
                fprintf(out,"\tDSVF\tL%d\n", l2);
                empilha(l2);
                geracod(bloco,out);
                /* desvio para início */
                fprintf(out,"\tDSVS\tL%d\n", desempilha());
                /* rótulo do fim */
                int aux = desempilha();
                fprintf(out,"L%d\tNADA\n", aux);
            }
            break;

        case SELEC:
            /* seleção: filho = cond; filho->irmao = LCM entao; possivel filho->irmao->irmao = LCM senao */
            {
                ptno cond = p->filho;
                ptno entao = cond ? cond->irmao : NULL;
                ptno senao = entao ? entao->irmao : NULL;
                gen_expr(cond, out);
                int lfalse = ++ROTULO;
                int lend = ++ROTULO;
                fprintf(out,"\tDSVF\tL%d\n", lfalse);
                geracod(entao,out);
                fprintf(out,"\tDSVS\tL%d\n", lend);
                fprintf(out,"L%d\tNADA\n", lfalse);
                if (senao) geracod(senao,out);
                fprintf(out,"L%d\tNADA\n", lend);
            }
            break;

        default:
            /* por segurança percorre filhos/irmaos */
            if (p->filho) geracod(p->filho,out);
            break;
    }

    if (p->irmao) geracod(p->irmao,out);
}

/* gera código para expressões: empilha valores e operadores MVS */
static void gen_expr(ptno p, FILE *out) {
    if (!p) return;
    if (p->tipo == NUM) {
        fprintf(out,"\tCRCT\t%d\n", p->valor);
    } else if (p->tipo == ID) {
        fprintf(out,"\tCRVL\t%s\n", p->lexema);
    } else if (p->tipo == OPBIN) {
        /* filho (direita?) e irmão? Nossa representação: filho = operando esquerdo; filho->irmao = operando direito
           Porém durante construção da árvore preservamos esta ordem. */
        ptno e1 = p->filho;
        ptno e2 = e1 ? e1->irmao : NULL;
        gen_expr(e1,out);
        gen_expr(e2,out);
        if (p->lexema) {
            if (strcmp(p->lexema,"*")==0) fprintf(out,"\tMULT\n");
            else if (strcmp(p->lexema,"/")==0) fprintf(out,"\tDIVI\n");
            else if (strcmp(p->lexema,"+")==0) fprintf(out,"\tSOMA\n");
            else if (strcmp(p->lexema,"-")==0) fprintf(out,"\tSUB\n");
            else if (strcmp(p->lexema,">")==0) fprintf(out,"\tCMME\n"); /* placeholder */
            else fprintf(out,"\t; OP %s\n", p->lexema);
        }
    } else if (p->tipo == COMP) {
        /* comparacao - deixamos 1 (verdade) ou 0 (falso) na pilha conforme comparação */
        ptno e1 = p->filho;
        ptno e2 = e1 ? e1->irmao : NULL;
        gen_expr(e1,out);
        gen_expr(e2,out);
        /* Usaremos instrução hipotética 'CMIG' para comparação '=' etc. Como exemplo: */
        if (p->lexema) {
            if (strcmp(p->lexema,">")==0) fprintf(out,"\tCMME\n"); /* placeholder: compare greater */
            else if (strcmp(p->lexema,"<")==0) fprintf(out,"\tCMME_INV\n"); /* placeholder */
            else if (strcmp(p->lexema,"=")==0) fprintf(out,"\tCMIG\n");
            else if (strcmp(p->lexema,"<>")==0) fprintf(out,"\tCMDIF\n");
            else fprintf(out,"\t; COMP %s\n", p->lexema);
        }
    } else if (p->tipo == NAO) {
        ptno expr = p->filho;
        gen_expr(expr,out);
        fprintf(out,"\tNADA\t; NAO (inverter) - implementar conforme MVS real\n");
    } else {
        /* se for nó composto, percorre filhos */
        if (p->filho) gen_expr(p->filho,out);
        if (p->irmao) gen_expr(p->irmao,out);
    }
}
