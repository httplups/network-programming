#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include "mysockfunctions.h"

#define MAXLINE 4096
#define max(m,n,p) ( (m) > (n) ? ((m) > (p) ? (m) : (p)) : ((n) > (p) ? (n) : (p)))

void str_cli(int SOCK_FD) {
    char   sendline[MAXLINE + 1], recvline[MAXLINE + 1];
    int maxfdp1, eof_stdin = 0;
    fd_set rset;
    // fd_set wset;

    FD_ZERO(&rset);
    FD_ZERO(&wset);

    for ( ;; ) {
        bzero(recvline, strlen(recvline));
        bzero(sendline, strlen(sendline));
        FD_SET(STDIN_FILENO, &rset);
        FD_SET(SOCK_FD, &rset);
        // FD_SET(sockfd, &wset); /* Por que o sockfd não está associado a write set, se ele chama a função write? */
        // FD_SET(STDOUT_FILENO, &wset); Nao sei se precisa
        maxfdp1 = max(STDIN_FILENO, STDOUT_FILENO, SOCK_FD)  +  1;
        Select(maxfdp1,  &rset,  &wset,  NULL,  NULL);

        if (FD_ISSET(STDIN_FILENO, &rset)) {

            if ( fgets(sendline, sizeof(sendline), stdin) != NULL ) { /* Devo trocar por while? */
                Write(SOCK_FD, sendline, strlen(sendline));
                bzero(sendline, strlen(sendline));
            }
            else {
                eof_stdin = 1;
                FD_CLR(STDIN_FILENO, &rset);
                fclose(stdin);
                shutdown(SOCK_FD, SHUT_WR); /* nao vai mandar mais nada pro servidor */
            }
        }

        if (FD_ISSET(SOCK_FD,  &rset)){ /*if socket is readable*/
            if(Read(SOCK_FD, recvline, MAXLINE) > 0) {
                printf("%s", recvline);
                fflush(stdout);
                bzero(recvline, strlen(recvline));
                // salva em um arquivo > 
                // fputs(recvline, stdout);
            }
        }

        if (eof_stdin) {
            shutdown(sockfd, SHUT_RD); 
            FD_CLR(sockfd, &rset);
            exit(0);
        }
        
    }
}

int main(int argc, char **argv) {

    int    sockfd, n, server_port_number;
    
    char   error[MAXLINE + 1];
    struct sockaddr_in servaddr,cliaddr;
	socklen_t lencli, lenserv;

    if (argc != 3) {
        printf("./cliente ip porta < arquivo.in");
        strcpy(error,"uso: ");
        strcat(error,argv[0]);
        strcat(error," <IPaddress>");
        perror(error);
        exit(1);
    }
    /* Creating socket */
	sockfd = Socket(AF_INET, SOCK_STREAM, 0);

	// inicializando o socket com zeros
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;

	// obtendo o numero de porta para se conectar ao servidor
	sscanf(argv[2], "%d", &server_port_number);
	servaddr.sin_port = htons(server_port_number);

	//obtendo o IP para se conectar ao servidor
	Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

	/* Connecting to the server */
	Connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    str_cli(sockfd);


	// lencli = sizeof(cliaddr);
	// lenserv = sizeof(servaddr);

	// Agora que a conexão foi feita, o connect implicitamente chamou o método bind e associou ao socket um numero de porta e ip local
	

    
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
