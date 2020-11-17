#include "mysockfunctions.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <pthread.h>

#define MAXLINE 4096
pthread_t tid[3];

void* doSomeThing(void *arg) {
    char **arguments = (char**)arg;
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
    printf("Informacoes do Socket Local:\n");
    printf("IP: %s\n", inet_ntoa(cliaddr.sin_addr));
    printf("Porta: %d\n", ntohs(cliaddr.sin_port));

    // GetPeerName(sockfd, (struct sockaddr *)&servaddr, &lenserv);
    // printf("Informacoes do Socket Remoto:\n");
    // printf("IP: %s\n", inet_ntoa(servaddr.sin_addr));
    // printf("Porta: %d\n", ntohs(servaddr.sin_port));

    while ( (n = Read(sockfd, recvline, MAXLINE)) > 0) {
        recvline[n] = 0;
        if (fputs(recvline, stdout) == EOF) {
            perror("fputs error");
            exit(1);
        }
    }
    printf("saiu\n");
    if (n < 0) {
        perror("read error");
        exit(1);
    }

    // exit(0);

    // terminates connection
    Close(sockfd);
    pthread_exit(NULL);
}

int main(int argc, char **argv)
{
    char error[MAXLINE + 1];
    int i = 0, err;

	if (argc != 3)
	{
		strcpy(error, "uso: ");
		strcat(error, argv[0]);
		strcat(error, " <IPaddress>");
		perror(error);
		exit(1);
	}


    while(i < 3)
    {
        err = pthread_create(&(tid[i]), NULL, &doSomeThing, (void *) argv);
        if (err != 0)
            printf("\ncan't create thread :[%s]", strerror(err));
        else
            printf("\n Thread created successfully\n");

        i++;
    }
    i = 0;
    while (i < 3) {
        pthread_join(tid[i], NULL);
    }

	return 0;
}
