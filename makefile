simples: utils.c lexico.l sintatico.y
	@flex -o lexico.c lexico.l
	@bison -v -d sintatico.y -o sintatico.c
	@gcc -Wall -g lexico.c sintatico.c utils.c tree.c -o simples

limpa: 
	@echo "Limpando..."
	@rm -f lexico.c sintatico.c sintatico.h sintatico.output simples *.dot *.mvs
