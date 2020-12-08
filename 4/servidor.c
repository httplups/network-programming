#include <time.h>
#include "mysockfunctions.h"
#include <stdint.h>

#define LISTENQ 10
#define MAXLINE 4096

int main (int argc, char **argv) {
    int    listenfd, connfd, n;
    struct sockaddr_in servaddr;
    char   recvline[MAXLINE + 1];
    time_t ticks;

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr)); // inicializando com zeros
    servaddr.sin_family      = AF_INET; // utilizando ipv4

    servaddr.sin_addr.s_addr = INADDR_ANY; // deixando escutar em qualquer IP
    servaddr.sin_port        = 0;   // deixando a porta automatica

    // associando a porta e ip ao socket
    Bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    // Descobrindo a porta que foi escolhida
    socklen_t len = sizeof(servaddr);
    GetSockName(listenfd, (struct sockaddr *)&servaddr, &len);
    printf("IP: %s\n", inet_ntoa(servaddr.sin_addr));
    printf("Porta: %d\n\n", ntohs(servaddr.sin_port));


    // escutando no socket, esperando um pedido de conexão
    Listen(listenfd, LISTENQ);

    for ( ; ; ) {

        bzero(recvline, strlen(recvline));
        // aceitando pedido de conexão no socket
        connfd = Accept(listenfd, (struct sockaddr *) NULL, NULL);

        // // obtendo informações sobre socket remoto (cliente)
        // if (getpeername(connfd, (struct sockaddr *) &servaddr,&len) == -1) {
        //     perror("getppername");
        //     exit(1);
        // }

        // printf("Informacoes sobre socket remoto:\n");
        // printf("IP remoto:%s\n", inet_ntoa(servaddr.sin_addr));
        // printf("Porta remota: %d\n", ntohs(servaddr.sin_port));

        while((n = Read(connfd, recvline, MAXLINE)) > 0) {
            // printf("n:%d\n", n);
            recvline[strlen(recvline)] = 0;
            printf("%s", recvline);
            Write(connfd, recvline, strlen(recvline));

            bzero(recvline, strlen(recvline));
        }
        // printf("n:%d\n", n);

        // ticks = time(NULL);
        // snprintf(buf, sizeof(buf), "%.24s\r\n", ctime(&ticks));
        // Write(connfd, buf, strlen(buf));

        // fechando conexão
        Close(connfd);
    }

    return(0);
}
