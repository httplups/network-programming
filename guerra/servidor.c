#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include "utils.h"
#include <sys/wait.h>
#include <signal.h>

#define LISTENQ 0
#define MAXDATASIZE 100
#define MAX_STRING_SIZE 100
#define MAXLINE 4096

void sig_chld(int signo){
	pid_t pid;
	int stat;
	while((pid = waitpid(-1, &stat, WNOHANG)) > 0)
		printf("child %d terminated\n", pid);
	return;
}

int main (int argc, char **argv) {
   	int    listenfd, connfd;
   	struct sockaddr_in servaddr, cliaddr;
   	pid_t pid;
	pid_t wait (int *statloc);
	pid_t waitpid (pid_t pid, int *statloc, int options);

  	listenfd = Socket(AF_INET, SOCK_STREAM, 0); //cria a instância do socket para conexão

	int portNumber;
   	sscanf(argv[1], "%d",&portNumber); //porta como parametro

  	bzero(&servaddr, sizeof(servaddr));
   	servaddr.sin_family      = AF_INET; //define o tipo de família do socket = IPv4
   	servaddr.sin_addr.s_addr = INADDR_ANY; //servidor define o IP 
   	servaddr.sin_port        = htons(portNumber);

	Bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)); //atribui nome à um socket e o endereço especificado à ele
   
   	socklen_t len = sizeof(servaddr);
   	Getsockname(listenfd, (struct sockaddr *)&servaddr, &len); //recupera informações sobre o socket, como a porta
   	printf("Server IP address: %s\n", inet_ntoa(servaddr.sin_addr)); //IP do servidor
   
	Listen(listenfd, LISTENQ); //servidor escuta todas as requisições feitas pelo cliente
	Signal(SIGCHLD, sig_chld); /* para chamar waitpid() */
	socklen_t lencli = sizeof(cliaddr);

	for ( ; ; ) {
		connfd = Accept(listenfd, (struct sockaddr *) &cliaddr, &lencli);  //durante a escuta, todas as conexões são aceitadas	 
		sleep(3);
		if((pid = fork()) == 0){		
			close(listenfd);
			printf("%s\n",sock_ntop((struct sockaddr *)&cliaddr, lencli)); //printa IP e porta do cliente 		
			close(connfd);
			exit(0);
		}

	      	close(connfd); //servidor fecha a conexão
	}
   return(0);
}
