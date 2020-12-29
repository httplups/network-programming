#include "mysockfunctions.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

#define MAXLINE 4096

int main(int argc, char **argv)
{
    char error[MAXLINE + 1], recvline[MAXLINE], username[10], option;
    int sockfd, server_port_number;
	struct sockaddr_in servaddr, cliaddr;
	socklen_t lencli;

    if (argc != 3)
	{
		printf("I need ip port\n");
		exit(1);
	}

    /* Creating socket */
    sockfd = Socket(AF_INET, SOCK_STREAM, 0);

    // Initializing
    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;

    // obtendo o numero de porta para se conectar ao servidor
    sscanf(argv[2], "%d", &server_port_number);
    servaddr.sin_port = htons(server_port_number);

    //obtendo o IP para se conectar ao servidor
    Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

    /* Connecting to the server */
    Connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    lencli = sizeof(cliaddr);

    // Agora que a conexão foi feita, o connect implicitamente chamou o método bind e associou ao socket um numero de porta e ip local
    GetSockName(sockfd, (struct sockaddr *)&cliaddr, &lencli);
    // printf("Informacoes do Socket Local:\n");
    printf("connected: %s:%d\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
    // printf("done: %s:%d\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
    
    Read(sockfd, recvline, MAXLINE);

    printf("%s\n", recvline);
    memset(username, 0, strlen(username));
    fgets(username, 10, stdin);

    Write(sockfd, username, strlen(username));
    memset(recvline, 0, strlen(recvline));

    /*  READING MENU */
    while((Read(sockfd, recvline, MAXLINE) > 0)) {
        printf("%s\n", recvline);
        // scanf(" %c", &option);
        option = getchar();
        Write(sockfd, &option, 1);
    }

    sleep(3);
    exit(0);

    // terminates connection
    // Close(sockfd);
    
	return 0;
}