#define _GNU_SOURCE
#include <unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <sys/wait.h>

// função pegar substring até a virgula
// é utilizada para separar os comandos pela virgula
void getCommand (char *string, char *result) {
  int i = 0;
  char c, command[50];
  // enquanto ele n atinge o \0
  while(string[i] != '\0') {
    if (string[i] == ',') {
      // se atinge a virgula retorna a substring
      result[i] = '\0';
      return;
    } else {
      // se n atinge a virgula vai passando o caractere para a variavel que vai armazenar o comando
      result[i] = string[i];
    }
    i++;
  }
  result[i] = '\0';
}
// função para remover o commando da string uma vez que será executado
void commandRemove(char *toRemove, char *string) {
  // apos separar o comando por virgula ele é removido da lista de comandos a serem processados
  int count = 0;
  int i;
  for (i = 0; toRemove[i] != '\0'; i++) {
    // conta a quantidade de caracteres do comando que será removido do inicio da string
    count++;
  }
  // caso exista a ',' então é um caracter a mais que será removido
  if (string[i] == ',') count++;
  for (i = 0; string[i + count] != '\0'; i++) {
    // caractere por caractere ele vai sendo reordenado o vetor para que a substring inicial seja removida
    string[i] = string[i + count];
  }

  string[i] = '\0';
}
// função pegar commando e executar seus parametros
int countWords(char *string) {
  // função que conta o numero de palavras para alocar dinamicamento o numero de argumentos
  int count = 0, i;
  for (i = 0; string[i] != '\0'; i++) {
    // caso seja o primeiro elemento ou caso o elemento anterior tenha sido um espaço conta como palavra
    if((i == 0 || string[i - 1] == ' ') && string[i] != ' ') count++;
  }
  return count;
}

void getFirstWord(char *string, char *word) {
  // pega a substring referente a primeira palavra
  int gotWord = 0, i, j = 0;
  // variavel gotWord booleana referente se alguma palavra já foi pega, para descontinuar os espaços vazios do inicio
  for (i = 0; string[i] != '\0'; i++) {
    if (gotWord && (string[i] == ' ' || string[i] == ',')) break;
    if (string[i] != ' ') {
      // se ainda n tiver pego a palavra então agora ela esta sendo pega
      if (!gotWord) gotWord = 1;
      word[j] = string[i];
      j++;
    } 
  }
  // coloca o \0 no final para a string funcionar normalmente
  word[j] = '\0';
  // remove a palavra da string 
  for (j = 0; string[j + i] != '\0'; j++) {
    string[j] = string[j + i];
  }
  string[j] = '\0';
}
int checkEmpty (char *string) {
  if (string[0] == '\0') return 1;
  for (int i = 1; string[i] != '\0'; i++) {
    if (string[i] != ' ') return 0;
  }
  return 1;
}
// função para conta a quantidade de comandos para fins de alocação do numero de processos necessarios
int countCommands(char *string) {
  int count = 0;
  char aux[513], aux2[513], word[513];
  strcpy(aux, string);
  for (int i = 0; aux[0] != '\0'; i++) {
    getCommand(aux, aux2);
    commandRemove(aux2, aux);
    // caso não seja 
    if (!checkEmpty(aux2)) count++;
    getFirstWord(aux2, word);
    // caso exista um quit é tratado para abortagem do programa
    if (strstr(word, "quit") || strcmp(word, "quit") == 0) {
      return -1;
    }
  }
  return count;
}


int main() {
  // declaração de variaveis
  // limitado a 512 caracteres, não sendo possivel maior que isso, assim impedindo que ocorra erro, 513 pois existe o '/0'
  char command[513];
  char nextCommand[513];
  char aux1[513], aux2[513];
  char **args;
  pid_t *pids;
  int i, nCommand, j;
  // enquanto não tiver lido um quit no meio dos processos o fluxo segue normalmente
  while (1) {
    j=0;
    // leitura da string de comandos
    // printf("digite o comando\n");
    gets(command);
    // checagem do numero de comandos para alocação do numero de processos
    nCommand = countCommands(command);
    if (nCommand == -1) {
      // encontrou um quit
      return 0;
    }
    // printf("ncom = %d\n", nCommand);
    // alocaçao do vetor de processos
    pids = (int *) malloc(nCommand * sizeof(int));
    if (pids == NULL) {
      // caso ocorra um erro na alocação da memoria
      printf("erro de memoria");
      exit(1);
    }
    do {
      // armazenado na variavel a substring do proximo comando a ser executado
      getCommand(command, nextCommand);
      // removido o comando da string de comandos para que não seja executado novamente
      commandRemove(nextCommand, command);
      // criado variaveis auxiliares para checagem do comando quit
      strcpy(aux1, nextCommand);
      getFirstWord(aux1, aux2);
      if (!checkEmpty(nextCommand)) {
        // feito o fork para que os comandos executem em concorrencia
        pids[j] = fork();
        if (pids[j] < 0) {
          // caso o fork falhe
          printf("fork failed");
          exit(1);
        } else if (pids[j] == 0) {
          // caso seja o processo filho
          args = (char **) malloc((countWords(nextCommand) + 1) * sizeof(char*));
          //  o numero de argumentos são o numero de palavras na substring do comando
          if(args == NULL) {
            // caso haja falha na alocação do vetor de argumetnos
            printf("Failed to allocate");
            exit(1);
          }
          // contagem de palavras para que seja alocado 
          int words = countWords(nextCommand);
          for (i = 0; i < words; i++) {
            // loop pelos argumentos onde é alocado o tamanho da string maxima de cada palavra
            args[i] = (char *) malloc(513 * sizeof(char));
            if (args[i] == NULL) {
              printf("Failed to allocate");
            }
            // armazenado no argumento a palavra da comando
            getFirstWord(nextCommand, args[i]);
          }
          // o vetor de argumentos deve encerrar em NULL
          args[i] = NULL;
          // printf só para saber se o commando entrou corretamente em execução
          // printf("executando");
          // execvp entra em execução para finalmente trabalhar o comando da concatenação feita pelo usuario
          if (execvp(args[0], args) == -1) {
            // tratamento de erro
            perror("erro ao tentar executar o comando:");
          }
          // for (i = 0; i < words; i++) {
          //   printf("%s ", args[i]);
          // }
          // printf("\n");
          // desalocação para liberação da memoria para o sistema operacional
          for (i = 0; i < words; i++) {
            free(args[i]);
          }
          free(args);
          // exit para que não seja executado o loop pelos filhos
          exit(1);
        }
        j++;
      }
    }while(command[0] != '\0'); // caso já tenham acabados os commandos a serem executados
    for (i = 0; i < nCommand; i++) {
      // espera todos os commandos terminarem seus processamentos antes de pedir uma nova string de comandos pro usuario
      int status;
      // printf("esperando %d\n", pids[i]);
      waitpid(pids[i], &status, 0);
    }
    // libera memoria para o sistema operacional
    free(pids);
  }
  return 0;
}