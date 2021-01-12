#include "mysockfunctions.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

#define MAXLINE 4096

int main(int argc, char **argv)
{
    char error[MAXLINE + 1], ID_str[100], players[MAXLINE], buffer[MAXLINE], sendline[MAXLINE];
    int socktcp, sockudp, server_port_number, option, player_port, maxfdp1, nready, n;
	struct sockaddr_in servtcpaddr, servudpaddr, clitcpaddr, cliudpaddr, servaddr;
	socklen_t lencli, lencliudp;
    struct timeval selTimeout;       /* Timeout for select() */
    long timeout = 1;
    int another_player_port = 0;
    char * playing = "playing";
    int playing_now = 0;

    if (argc != 3)
	{
		printf("I need ip port\n");
		exit(1);
	}

    /*  ================================================================================== */

    /* CONNECTING TO A TCP SERVER FIRST */
    /* Creating socket */
    socktcp = Socket(AF_INET, SOCK_STREAM, 0);

    // Initializing
    bzero(&servtcpaddr, sizeof(servtcpaddr));
    bzero(&clitcpaddr, sizeof(clitcpaddr));

    servtcpaddr.sin_family = AF_INET;

    // obtendo o numero de porta para se conectar ao servidor
    sscanf(argv[2], "%d", &server_port_number);
    servtcpaddr.sin_port = htons(server_port_number);

    //obtendo o IP para se conectar ao servidor
    Inet_pton(AF_INET, argv[1], &servtcpaddr.sin_addr);

    /* Connecting to the server */
    Connect(socktcp, (struct sockaddr *)&servtcpaddr, sizeof(servtcpaddr));

    lencli = sizeof(clitcpaddr);

    // Agora que a conexão foi feita, o connect implicitamente chamou o método bind e associou ao socket um numero de porta e ip local
    GetSockName(socktcp, (struct sockaddr *)&clitcpaddr, &lencli);
    // printf("Informacoes do Socket Local:\n");
    printf("Ip e porta local: %s:%d\n", inet_ntoa(clitcpaddr.sin_addr), ntohs(clitcpaddr.sin_port));
    // printf("done: %s:%d\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
    
     /*  ================================================================================== */

    /* STARTING A UDP SERVER */
    sockudp = Socket(AF_INET, SOCK_DGRAM, 0);
    bzero(&servudpaddr, sizeof(servudpaddr));
    

    // Filling me-as-server information 
    servudpaddr.sin_family    = AF_INET; // IPv4 
    servudpaddr.sin_addr.s_addr = INADDR_ANY; 
    servudpaddr.sin_port = clitcpaddr.sin_port; /* My server UDP port is the same as my TCP client port*/

    // Bind the socket with the my server udp address 
    Bind(sockudp, (const struct sockaddr *)&servudpaddr,sizeof(servudpaddr));

    /*  ================================================================================== */

    
    printf("Bem vindo ao jogo!\n");
    /*  ================================================================================== */
    /* USING SELECT TO LISTEN ON BOTH SOCKETS */
    fd_set rset;
    int r;

    for (;;) {

        memset(players, 0, strlen(players));
        sleep(3);
        char * get = "get";
        Write(socktcp, get, strlen(get));

        
        FD_ZERO(&rset);

        /* Associando file descriptors ao conjunto read*/
        if (playing_now == 0)
            FD_SET(socktcp, &rset);
        FD_SET(sockudp, &rset);

        selTimeout.tv_sec = timeout;       /* timeout (secs.) */
        selTimeout.tv_usec = 0;            /* 0 microseconds */
        
        maxfdp1 = max(socktcp, sockudp)  +  1;
        if((nready = Select(maxfdp1, &rset, NULL, NULL, &selTimeout)) != 0) {

            if(FD_ISSET(sockudp, &rset)) { /* UDP SERVER*/

                bzero(&cliudpaddr, sizeof(cliudpaddr));
                memset(buffer, 0, strlen(buffer));
                memset(sendline, 0, strlen(sendline));

                n = Recvfrom(sockudp, &buffer, MAXLINE, MSG_WAITALL, (struct sockaddr *) &cliudpaddr, &lencliudp);
                lencliudp = sizeof(cliudpaddr);

                if (ntohs(cliudpaddr.sin_port) == another_player_port) {

                     /* tell tcp server i am not avaiable */
                    Write(socktcp, playing, strlen(playing));
                    playing_now = 1;

                    /* tcp stop sending */
                    FD_CLR(socktcp, &rset);


                    if (n == 0) {
                        /* get out*/
                        continue;
                    }


                    buffer[n] = '\0'; 
                    printf("O jogador aceitou jogar e enviou: %s\n", buffer); 
                    printf("Estamos jogando...\n");

                    // Sendto(sockudp, result, strlen(result),MSG_CONFIRM, (const struct sockaddr *) &cliudpaddr, lencliudp);
                    sleep(10);
                    char* result = "lose";
                    Write(socktcp, result, strlen(result));
                    printf("Voce perdeu\n");
                    playing_now = 0;

                }

                /* Otherwise, DISCARD */
            }
            

            if (FD_ISSET(socktcp, &rset)) { /* TCP */
                
                Read(socktcp, players, MAXLINE);
                // printf("%s\n", players);

                if (strcmp(players, "NULL") == 0) {
                    printf("Nao tem jogadores disponiveis, tentando dnv...\n\n");
                    sleep(3);
                }
                else if(strcmp(players, "NULL") != 0){
                    
                    /*STDIN blocks*/
                    printf("============ Lista de Jogadores: ============\n");
                    printf("\nID\tIP\t\tPorta\n");
                    printf("%s\n", players);

                    printf("Digite uma das opcoes:\nNumero de porta de quem deseja jogar\n0 - Sair\n1 - Atualizar lista\n");
                    scanf(" %d", &player_port);

                    if (player_port == 1)
                        continue;
                    
                    if (player_port == 0) {
                        Close(socktcp);
                        break;
                    }

                    char* player_port_str = integer_to_string(player_port);

                    /* envia a porta do jogador que quer jogar */
                    Write(socktcp, player_port_str, strlen(player_port_str));

                    /* =============================================================== */

                    /* START UDP CLIENT - GAME*/
                    // Creating socket file descriptor 
                    int sockfd;
                    sockfd = Socket(AF_INET, SOCK_DGRAM, 0);
                
                    memset(&servaddr, 0, sizeof(servaddr)); 
                    
                    // Filling server information 
                    servaddr.sin_family = AF_INET; 
                    servaddr.sin_port = htons(player_port); /*Connect UDP server port*/
                    servaddr.sin_addr.s_addr = INADDR_ANY; 
                    
                    socklen_t len; 
                    char * hello = "Hello\n";

                    printf("Pedindo para jogar com: %s:%d\n", inet_ntoa(servaddr.sin_addr), ntohs(servaddr.sin_port));

                    Connect(sockfd, (const struct sockaddr *) &servaddr, sizeof(servaddr));


                    /* tell tcp server we are not avaiable */
                    Write(socktcp, playing, strlen(playing));

                    /* tcp stop sending */
                    FD_CLR(socktcp, &rset);
                    playing_now = 1;

                    printf("Estamos jogando...\n");
                    Write(sockfd, hello, strlen(hello));
                    sleep(10); /*  PRETEDING PLAYING */
                    
                    char* result = "won";
                    Write(socktcp, result, strlen(result));
                    printf("Voce venceu\n");

                    playing_now = 0;
                }
                else {
                    /* its the port number of another player that wants to play with me */
                    another_player_port = atoi(players);

                }

            }
        }
        else {
            printf("Nada nesse timeout\n");
        }

    }
    

    exit(0);

    // terminates connection
    // Close(sockfd);
    
	return 0;
}
