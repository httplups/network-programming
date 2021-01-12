#include "mysockfunctions.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

#define MAXLINE 4096

int main(int argc, char **argv)
{
    char error[MAXLINE + 1], ID_str[100], players[MAXLINE];
    int socktcp, sockudp, server_port_number, option, player_port, maxfdp1, nready;
	struct sockaddr_in servtcpaddr, servudpaddr, clitcpaddr, cliudpaddr;
	socklen_t lencli;
    struct timeval selTimeout;       /* Timeout for select() */

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
    printf("connected: %s:%d\n", inet_ntoa(clitcpaddr.sin_addr), ntohs(clitcpaddr.sin_port));
    // printf("done: %s:%d\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
    
     /*  ================================================================================== */

    /* STARTING A UDP SERVER */
    sockudp = Socket(AF_INET, SOCK_DGRAM, 0);
    bzero(&servudpaddr, sizeof(servudpaddr));
    bzero(&cliudpaddr, sizeof(cliudpaddr));

    // Filling me-as-server information 
    servudpaddr.sin_family    = AF_INET; // IPv4 
    servudpaddr.sin_addr.s_addr = INADDR_ANY; 
    servudpaddr.sin_port = clitcpaddr.sin_port; /* My server UDP port is the same as my TCP client port*/

    // Bind the socket with the my server udp address 
    Bind(sockudp, (const struct sockaddr *)&servudpaddr,sizeof(servudpaddr));

    /*  ================================================================================== */

    
    printf("Welcome to the game!\n");
    /*  ================================================================================== */
    /* USING SELECT TO LISTEN ON BOTH SOCKETS */
    fd_set rset;


    for (;;) {

        printf("\n\nChoose one option below:\n\n1 - Invite someone to play with\n");

        FD_ZERO(&rset);

        /* Associando file descriptors ao conjunto read*/
        FD_SET(socktcp, &rset);
        FD_SET(sockudp, &rset);
        FD_SET(STDIN_FILENO, &rset);

        selTimeout.tv_sec = 10;       /* timeout (secs.) */
        selTimeout.tv_usec = 0;            /* 0 microseconds */
        
        maxfdp1 = max(socktcp, sockudp)  +  1;
        if((nready = Select(maxfdp1, &rset, NULL, NULL, &selTimeout)) != 0) {

            
            scanf(" %d", &option);
            switch (option) {
                case 1: {
                    char * get = "get";
                    Write(socktcp, get, strlen(get));

                    if (FD_ISSET(socktcp, &rset)) {
                        Read(socktcp, players, MAXLINE);
                    

                        if (strcmp(players, "NULL") == 0) {
                            printf("No players avaiable. Try again soon...");
                            continue;
                        }
                    
                        /*STDIN blocks*/
                        printf("============ List of players: ============\n");
                        printf("\nID\tIP\t\tPort\n");
                        printf("%s\n", players);

                        printf("Choose the port number of player that you wanna play:");
                        scanf(" %d", &player_port);
                        printf("You chose %d\n", player_port);

                    }
                    // char delim[] = " ";
                    // char *ptr = strtok(player, delim);
                    // printf("IP:%s\n", ptr);
                    // ptr = strtok(NULL, delim);
                    // printf("Port:%s\n", ptr);
                    
                }
                case 0:
                    //finish conn
                    break;
            }
        }
        else {
            printf("Nothing this time...\n");
        }

    }
    
    
    // Read(sockfd, recvline, MAXLINE);

    // printf("%s\n", recvline);
    

    // Write(sockfd, username, strlen(username));
    // memset(recvline, 0, strlen(recvline));

    // /*  READING MENU */
    // while((Read(sockfd, recvline, MAXLINE) > 0)) {
    //     printf("%s\n", recvline);
    //     // scanf(" %c", &option);
    //     option = getchar();
    //     Write(sockfd, &option, 1);
    // }

    exit(0);

    // terminates connection
    // Close(sockfd);
    
	return 0;
}
