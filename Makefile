# makefile simples utilizado apenas para compilar o .c padrão já que não existe dependenca .h

run: compile
	./main 

compile: 
	gcc proj.c -o main

clean:
	rm -rf *.o *~ main