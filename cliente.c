#include "mysockfunctions.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <pthread.h>

#define MAXLINE 4096
int n_clientes = 10;
pthread_t tid[10];

void* doSomeThing(void *arg) {
    pthread_exit(NULL);
}

int main(int argc, char **argv)
{
    char error[MAXLINE + 1];

	if (argc != 3)
	{
		strcpy(error, "uso: ");
		strcat(error, argv[0]);
		strcat(error, " <IPaddress>");
		perror(error);
		exit(1);
	}

    int sockfd, server_port_number, n;
	char recvline[MAXLINE + 1];
	struct sockaddr_in servaddr, cliaddr;
	socklen_t lencli, lenserv;
    /* Creating socket */
    sockfd = Socket(AF_INET, SOCK_STREAM, 0);

    // inicializando o socket com zeros
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;

    // obtendo o numero de porta para se conectar ao servidor
    sscanf(arguments[2], "%d", &server_port_number);
    servaddr.sin_port = htons(server_port_number);

    //obtendo o IP para se conectar ao servidor
    Inet_pton(AF_INET, arguments[1], &servaddr.sin_addr);

    /* Connecting to the server */
    Connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    lencli = sizeof(cliaddr);
    lenserv = sizeof(servaddr);

    // Agora que a conexão foi feita, o connect implicitamente chamou o método bind e associou ao socket um numero de porta e ip local
    GetSockName(sockfd, (struct sockaddr *)&cliaddr, &lencli);
    // printf("Informacoes do Socket Local:\n");
    printf("connected: %s:%d\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));

    // GetPeerName(sockfd, (struct sockaddr *)&servaddr, &lenserv);
    // printf("Informacoes do Socket Remoto:\n");
    // printf("IP: %s\n", inet_ntoa(servaddr.sin_addr));
    // printf("Porta: %d\n", ntohs(servaddr.sin_port));

    while ( (n = Read(sockfd, recvline, MAXLINE)) > 0) {
        recvline[n] = 0;
        // if (fputs(recvline, stdout) == EOF) {
        //     perror("fputs error");
        //     exit(1);
        // }
    }
    printf("done: %s:%d\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
    if (n < 0) {
        perror("read error");
        exit(1);
    }

    // exit(0);

    // terminates connection
    Close(sockfd);
    
	return 0;
}
