README

3938 - Sayonara Bernardes
5144 - Douglas Boaventura
5164 - Milena Nobres

	Este projeto tem como intuito uma implementação de um bash de um sistema operacional para a disciplina de sistemas
Operacionais da UFV - CRP, para execução do codigo é necessario ter na maquina o compilador GCC ou outro no linux (em caso
de outro compilador alterar no arquivo makefile), o arquivo make já compila e executa o programa.
	Para concepçao do mesmo foi utilizada a estrategia de "dividir para conquistar" onde o problema foi dividido em sub-
problemas menores, e modularizado em pequenas funções para tratamento das strings até que elas estejam no formato ideal de
execução pela função execvp().
	Ao executar o código uma instrução para digitação na tela aparece onde o usuario preenche com a sequencia de comandos
que será executado de forma não sequencial, para cada comando um fork() é executado dividindo o processo de forma concorrente,
assim todos os comandos são executados de forma não linear.
	antes que o usuario possa preencher a variavel com uma nova sequencia de comandos um waitpid é executado para cada um
dos processos antes criados no fork(), para assegurar que antes que uma nova sequencia de comandos inicie toda os comandos da 
sequencia anterior tenha terminado, alguns printfs() são executados para que usuario saiba quando um comando entrou em execução
e quando o processo pai está em espera pelos filhos, os printfs são facilmente descartaveis mas para vias de estudo da concorrencia
entre os processos e debbug do codigo é interessante observar seu comportamento.
	Alguns exemplos de commandos executados podem ser:
	"ls    -la  , , ,,,, quit, cat  proj.c  ,  "
	"ls -la,   ,     cat proj.c   , "
	"ls   ,   ,  touch ujg.txt,  ,  "´
	É bom se atentar que a ordem de como os processos serão executados é completamente aleatoria uma vez que os mesmos são executados
de forma concorrente.
