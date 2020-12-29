#include "mysockfunctions.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

#define MAXLINE 4096

int main(int argc, char **argv)
{
    char error[MAXLINE + 1], ID_str[100], players[MAXLINE], player_port[MAXLINE];
    int socktcp, sockudp, server_port_number, option, ID, maxfdp1;
	struct sockaddr_in servtcpaddr, servudpaddr, clitcpaddr, cliudpaddr;
	socklen_t lencli;

    if (argc != 3)
	{
		printf("I need ip port\n");
		exit(1);
	}

    /* CONNECTING TO A TCP SERVER FIRST */
    /* Creating socket */
    socktcp = Socket(AF_INET, SOCK_STREAM, 0);

    // Initializing
    bzero(&servtcpaddr, sizeof(servtcpaddr));

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


    /* STARTING A UDP SERVER */



    printf("Welcome to the game!\n");

    do {
        printf("\n\nChoose one option below:\n\n1 - Invite someone to play with\n0 - Quit\n");
        scanf(" %d", &option);

        switch (option) {
            case 1: {
                char * get = "get";
                Write(socktcp, get, strlen(get));
                Read(socktcp, players, MAXLINE);
                

                if (strcmp(players, "NULL") == 0) {
                    printf("No players avaiable. Try again soon...");
                    continue;
                }
                
                printf("============ List of players: ============\n");
                printf("\nID\tIP\tPort\n");
                printf("%s\n", players);

                // printf("Choose one player to play with by ID:");
                // scanf(" %d", &ID);
                // printf("You chose %d\n", ID);

                // memset(ID_str, 0, strlen(ID_str));
                // sprintf(ID_str, "%d", ID);
                // printf("str: %s\n",ID_str);
                // Write(socktcp, ID_str, strlen(ID_str));

                // /* getting port of player assuming it's listening on 0.0.0.0 */
                // Read(socktcp, player_port, MAXLINE);
                // printf("Port: %s\n", player_port);
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

    } while(option != 0);
    
    
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
