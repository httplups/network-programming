#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define MAXLINE 4096

int main(int argc, char **argv) {

    int    sockfd, n;
    char   recvline[MAXLINE + 1];
    char   error[MAXLINE + 1];
    struct sockaddr_in servaddr;

    if (argc != 3) {
        strcpy(error,"uso: ");
        strcat(error,argv[0]);
        strcat(error," <IPaddress>");
        perror(error);
        exit(1);
    }


    /* This code snippet should be in a function */

    char buffer[MAXLINE];

    while( fgets(buffer, sizeof(buffer), stdin) != NULL )
    {
        printf("%s\n", buffer);
        /* check and process data */
    }
   
    printf("EOF...");
     

    // // criando o socket
    // if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    //     perror("socket error");
    //     exit(1);
    // }

    // // inicializando o socket com zeros
    // bzero(&servaddr, sizeof(servaddr));
    // servaddr.sin_family = AF_INET;
    
    // // obtendo o numero de porta para se conectar ao servidor
    // int server_port_number;
    // sscanf(argv[2], "%d", &server_port_number);
    // servaddr.sin_port   = htons(server_port_number);

    // //obtendo o IP para se conectar ao servidor
    // if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
    //     perror("inet_pton error");
    //     exit(1);
    // }
    
    // if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
    //     perror("connect error");
    //     exit(1);
    // }
    
    // socklen_t len = sizeof(servaddr);
    // // Agora que a conexão foi feita, o connect implicitamente chamou o método bind e associou ao socket um numero de porta e ip local
    // if (getsockname(sockfd, (struct sockaddr *) &servaddr, &len) == -1) {
    //     perror("getsockname");
    //         exit(1);
    // }
    // printf("Informacoes do Socket Local:\n");
    // printf("IP local: %s\n", inet_ntoa(servaddr.sin_addr));
    // printf("Porta local: %d\n", ntohs(servaddr.sin_port));

    // while ( (n = read(sockfd, recvline, MAXLINE)) > 0) {
    //     recvline[n] = 0;
    //     if (fputs(recvline, stdout) == EOF) {
    //         perror("fputs error");
    //         exit(1);
    //     }
    // }

    // if (n < 0) {
    //     perror("read error");
    //     exit(1);
    // }

    exit(0);
}
