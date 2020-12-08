#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include "mysockfunctions.h"

#define MAXLINE 10000
#define max(a,b) (((a)>(b))?(a):(b))

void str_cli(int SOCK_FD) {
    char   sendline[MAXLINE + 1], recvline[MAXLINE + 1];
    int maxfdp1, eof_stdin = 0;
    fd_set rset;

    FD_ZERO(&rset);

    for ( ;; ) {

        /*Limpando buffers */
        memset(recvline, 0, strlen(recvline));
        memset(sendline, 0, strlen(sendline));

        /* Associando file descriptors ao conjunto read*/
        FD_SET(STDIN_FILENO, &rset);
        FD_SET(SOCK_FD, &rset);
        
        maxfdp1 = max(STDIN_FILENO, SOCK_FD)  +  1;
        Select(maxfdp1,  &rset,  NULL,  NULL,  NULL);

        if (FD_ISSET(STDIN_FILENO, &rset)) { /* conseguimos ler do stdin */

            if ( fgets(sendline, sizeof(sendline), stdin) != NULL ) {
                sendline[strlen(sendline)] = 0;
                Write(SOCK_FD, sendline, strlen(sendline)); // envia para o servidor
                memset(sendline, 0, strlen(sendline));
            }
            else {
                eof_stdin = 1;
                FD_CLR(STDIN_FILENO, &rset);
                fclose(stdin);
                shutdown(SOCK_FD, SHUT_WR); /* nao vai mandar mais nada pro servidor */
            }
        }

        if (FD_ISSET(SOCK_FD,  &rset)){ /* conseguimos ler do socket*/
            if(Read(SOCK_FD, recvline, MAXLINE) > 0) {
                printf("%s", recvline); // print que será redirecionado para > saida.out
                fflush(stdout);
                memset(recvline, 0, strlen(recvline));
            }
        }

        if (eof_stdin) { 
            FD_CLR(SOCK_FD, &rset);
            shutdown(SOCK_FD, SHUT_RD);
            break;
        }
        
    }
}

int main(int argc, char **argv) {

    int    sockfd, server_port_number;
    char   error[MAXLINE + 1];
    struct sockaddr_in servaddr;

    if (argc != 3) {
        printf("./cliente ip porta < entrada.in > saida.out ");
        strcpy(error,"uso: ");
        strcat(error,argv[0]);
        strcat(error," <IPaddress>");
        perror(error);
        exit(1);
    }
    /* Criando socket */
	sockfd = Socket(AF_INET, SOCK_STREAM, 0);

	// inicializando o socket com zeros
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;

	// obtendo o numero de porta para se conectar ao servidor
	sscanf(argv[2], "%d", &server_port_number);
	servaddr.sin_port = htons(server_port_number);

	//obtendo o IP para se conectar ao servidor
	Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

	/* Conectando ao servidor */
	Connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    str_cli(sockfd);   

    return 0;
}
