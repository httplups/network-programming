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

#define LISTENQ 10
#define MAXDATASIZE 100

int main (int argc, char **argv) {
    int    listenfd, connfd;
    struct sockaddr_in servaddr;
    char   buf[MAXDATASIZE];
    time_t ticks;

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    bzero(&servaddr, sizeof(servaddr)); // inicializando com zeros
    servaddr.sin_family      = AF_INET; // utilizando ipv4

    servaddr.sin_addr.s_addr = INADDR_ANY; // deixando escutar em qualquer IP
    servaddr.sin_port        = 0;   // deixando a porta automatica

    // associando a porta e ip ao socket
    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
        perror("bind");
        exit(1);
    }

    // Descobrindo a porta que foi escolhida
    socklen_t len = sizeof(servaddr);
    if (getsockname(listenfd, (struct sockaddr *)&servaddr, &len) == -1) {
       perror("getsockname");
       exit(1);
    }
    else {
        printf("IP para voce se conectar: %s\n", inet_ntoa(servaddr.sin_addr));
        printf("Numero de porta para voce se conectar: %d\n", ntohs(servaddr.sin_port));
    }

    // escutando no socket, esperando um pedido de conexão
    if (listen(listenfd, LISTENQ) == -1) {
        perror("listen");
        exit(1);
    }

    for ( ; ; ) {

        // aceitando pedido de conexão no socket
        if ((connfd = accept(listenfd, (struct sockaddr *) NULL, NULL)) == -1 ) {
            perror("accept");
            exit(1);
        }

        // obtendo informações sobre socket remoto (cliente)
        if (getpeername(connfd, (struct sockaddr *) &servaddr,&len) == -1) {
            perror("getppername");
            exit(1);
        }

        printf("Informacoes sobre socket remoto:\n");
        printf("IP remoto:%s\n", inet_ntoa(servaddr.sin_addr));
        printf("Porta remota: %d\n", ntohs(servaddr.sin_port));

        ticks = time(NULL);
        snprintf(buf, sizeof(buf), "%.24s\r\n", ctime(&ticks));
        write(connfd, buf, strlen(buf));

        // fechando conexão
        close(connfd);
    }

    return(0);
}
