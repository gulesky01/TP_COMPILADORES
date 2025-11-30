/* tree.c - construção da árvore, saída .dot e gerador MVS */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"

/* cria nó */
ptno criaNo(int tipo, int valor, const char *lexema)
{
    ptno n = (ptno)malloc(sizeof(struct no));
    if (!n)
    {
        perror("malloc");
        exit(1);
    }
    n->tipo = tipo;
    n->valor = valor;
    if (lexema)
        n->lexema = strdup(lexema);
    else
        n->lexema = NULL;
    n->filho = NULL;
    n->irmao = NULL;
    return n;
}

/* adiciona filho ao final da lista de filhos (filho encadeado por 'irmao') */
void adicionaFilho(ptno pai, ptno filho)
{
    if (!pai || !filho) return;

    if (pai->filho == NULL) {
        pai->filho = filho;
    }
    else {
        ptno p = pai->filho;
        while (p->irmao != NULL)
            p = p->irmao;
        p->irmao = filho;
    }
}

/* --- gera .dot --- */
/* Imprime todos os nós começando por p e seus irmãos (itera irmãos).
   Para cada nó impresso, recursa apenas sobre seu filho (não sobre irmão). */
static void dot_nodes(FILE *f, ptno p)
{
    if (!p) return;

    char label[256];

    switch (p->tipo)
    {
        case PRG:
            snprintf(label, sizeof(label), "programa | ");
            break;

        case ID:
            snprintf(label, sizeof(label), "identificador | %s",
                     p->lexema ? p->lexema : "");
            break;

        case DVR:
            snprintf(label, sizeof(label), "declaracao de variaveis | ");
            break;

        case TIPO:
            snprintf(label, sizeof(label), "tipo | inteiro");
            break;

        case LVAR:
            snprintf(label, sizeof(label), "lista variaveis | ");
            break;

        case LCM:
            snprintf(label, sizeof(label), "lista comandos | ");
            break;

        case LEI:
            snprintf(label, sizeof(label), "leitura | ");
            break;

        case ESC:
            snprintf(label, sizeof(label), "escrita | ");
            break;

        case ATR:
            snprintf(label, sizeof(label), "atribuicao | ");
            break;

        case OPBIN:
            snprintf(label, sizeof(label), "multiplica | ");
            break;

        case NO_NUM:
            snprintf(label, sizeof(label), "numero | %d", p->valor);
            break;

        default:
            snprintf(label, sizeof(label), "no | ");
            break;
    }

    fprintf(f, "n%p [ label=\"%s\" ];\n", (void*)p, label);

    dot_nodes(f, p->filho);
    dot_nodes(f, p->irmao);
}


static void dot_edges(FILE *f, ptno p)
{
    if (!p) return;

    for (ptno c = p->filho; c; c = c->irmao)
    {
        fprintf(f, "n%p -> n%p;\n", (void*)p, (void*)c);
        dot_edges(f, c);
    }
}


void geraDot(ptno raiz, const char *filename)
{
    FILE *f = fopen(filename, "w");
    if (!f)
    {
        perror("geraDot fopen");
        return;
    }
    fprintf(f, "digraph {\n");
    fprintf(f, "node [ shape=record, height=.1 ];\n");
    if (raiz)
    {
        dot_nodes(f, raiz);
        dot_edges(f, raiz);
    }
    fprintf(f, "}\n");
    fclose(f);
    

}

/* --- Gerador MVS --- */

/* Instruções MVS definidas pelo professor - usaremos os mnemonicos exatos */
extern int ROTULO;
extern int NUMVAR;
extern void empilha(int x);
extern int desempilha(void);

/* gera expressões - deixa resultado no topo da pilha MVS */
static void gen_expr(ptno p, FILE *out)
{
    if (!p)
        return;
    if (p->tipo == NO_NUM)
    {
        fprintf(out, "\tCRCT\t%d\n", p->valor);
    }
    else if (p->tipo == ID)
    {
        /* variáveis globais: carrega valor global */
        if (p->lexema)
            fprintf(out, "\tCRVG\t%s\n", p->lexema);
        else
            fprintf(out, "\t; CRVG ?\n");
    }
    else if (p->tipo == OPBIN)
    {
        ptno e1 = p->filho;
        ptno e2 = e1 ? e1->irmao : NULL;
        gen_expr(e1, out);
        gen_expr(e2, out);
        if (p->lexema)
        {
            if (strcmp(p->lexema, "+") == 0)
                fprintf(out, "\tSOMA\n");
            else if (strcmp(p->lexema, "-") == 0)
                fprintf(out, "\tSUBT\n");
            else if (strcmp(p->lexema, "*") == 0 || strcmp(p->lexema, "multiplica") == 0)
                fprintf(out, "\tMULT\n");
            else if (strcmp(p->lexema, "/") == 0)
                fprintf(out, "\tDIVI\n");
            else
                fprintf(out, "\t; OPBIN %s\n", p->lexema);
        }
    }
    else if (p->tipo == COMP)
    {
        ptno e1 = p->filho;
        ptno e2 = e1 ? e1->irmao : NULL;
        gen_expr(e1, out);
        gen_expr(e2, out);
        if (p->lexema)
        {
            if (strcmp(p->lexema, ">") == 0)
                fprintf(out, "\tCMMA\n");
            else if (strcmp(p->lexema, "<") == 0)
                fprintf(out, "\tCMME\n");
            else if (strcmp(p->lexema, "=") == 0)
                fprintf(out, "\tCMIG\n");
            else if (strcmp(p->lexema, "<>") == 0)
            {
                fprintf(out, "\tCMIG\n\tNEGA\n");
            }
            else
                fprintf(out, "\t; COMP %s\n", p->lexema);
        }
    }
    else if (p->tipo == NO_NAO)
    {
        ptno expr = p->filho;
        gen_expr(expr, out);
        fprintf(out, "\tNEGA\n");
    }
    else
    {
        /* caso composto - percorre filhos */
        if (p->filho)
            gen_expr(p->filho, out);
        if (p->irmao)
            gen_expr(p->irmao, out);
    }
}

/* geracod: percorre nós e emite instruções MVS */
void geracod(ptno p, FILE *out)
{
    if (!p)
        return;
    ptno p1, p2, p3;
    switch (p->tipo)
    {
    case PRG:
        p1 = p->filho;              /* id */
        p2 = p1 ? p1->irmao : NULL; /* dvr */
        p3 = p2 ? p2->irmao : NULL; /* lcm */
        fprintf(out, "\tINPP\n");
        NUMVAR = 0;
        if (p2)
            geracod(p2, out); /* declara variaveis (conta) */
        if (p3)
            geracod(p3, out); /* corpo */
        fprintf(out, "\tFIMP\n");
        break;
    

    case DVR:
        /* DVR -> TIPO LVAR
           contamos variáveis (globais) mas também geramos AMEM se quiser alocar
           Vamos só incrementar NUMVAR; se desejar emitir AMEM, poderia ser feito aqui. */
        {
            ptno tipo = p->filho;
            ptno lista = tipo ? tipo->irmao : NULL;
            ptno v = lista ? lista->filho : NULL;
            while (v)
            {
                NUMVAR++;
                v = v->irmao;
            }
            /* opcional: alocar memória global (AMEM) com NUMVAR
               aqui não sabemos o endereco; muitos MVS usam AMEM n para reservar */
            if (NUMVAR > 0)
                fprintf(out, "\tAMEM\t%d\n", NUMVAR);
        }
        break;

    case LCM:
    {
        ptno cmd = p->filho;
        /* filhos encadeados por irmao */
        while (cmd)
        {
            geracod(cmd, out);
            cmd = cmd->irmao;
        }
    }
    break;

    case LEI:
        if (p->filho && p->filho->lexema)
        {
            fprintf(out, "\tLEIA\t%s\n", p->filho->lexema);
        }
        else
        {
            fprintf(out, "\t; LEIA ?\n");
        }
        break;

    case ESC:
        if (p->filho)
        {
            /* pode ser NUM ou ID ou expressão */
            if (p->filho->tipo == NO_NUM)
            {
                fprintf(out, "\tCRCT\t%d\n", p->filho->valor);
            }
            else
            {
                gen_expr(p->filho, out);
            }
            fprintf(out, "\tESCR\n");
        }
        break;

    case ATR:
        /* filho = ID, filho->irmao = expr */
        if (p->filho)
        {
            ptno var = p->filho;
            ptno expr = var->irmao;
            gen_expr(expr, out);
            if (var->lexema)
                fprintf(out, "\tARZG\t%s\n", var->lexema);
            else
                fprintf(out, "\t; ARZG ?\n");
        }
        break;

    case REP:
        /* filho = cond, filho->irmao = lista_cmd */
        {
            ptno cond = p->filho;
            ptno bloco = cond ? cond->irmao : NULL;
            int lbegin = ++ROTULO;
            int lend = ++ROTULO;
            fprintf(out, "L%d\tNADA\n", lbegin);
            gen_expr(cond, out);
            fprintf(out, "\tDSVF\tL%d\n", lend);
            if (bloco)
                geracod(bloco, out);
            fprintf(out, "\tDSVS\tL%d\n", lbegin);
            fprintf(out, "L%d\tNADA\n", lend);
        }
        break;

    case SELEC:
        /* filho = cond, filho->irmao = entao, entao->irmao = senao (opcional) */
        {
            ptno cond = p->filho;
            ptno entao = cond ? cond->irmao : NULL;
            ptno senao = entao ? entao->irmao : NULL;
            int lfalse = ++ROTULO;
            int lend = ++ROTULO;
            gen_expr(cond, out);
            fprintf(out, "\tDSVF\tL%d\n", lfalse);
            if (entao)
                geracod(entao, out);
            fprintf(out, "\tDSVS\tL%d\n", lend);
            fprintf(out, "L%d\tNADA\n", lfalse);
            if (senao)
                geracod(senao, out);
            fprintf(out, "L%d\tNADA\n", lend);
        }
        break;

    default:
        /* percorre recursivamente */
        if (p->filho)
            geracod(p->filho, out);
        break;
    }

    if (p->irmao)
        geracod(p->irmao, out);
}
