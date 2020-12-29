#include "mysockfunctions.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <pthread.h>

#define MAXLINE 4096
pthread_t thread1, thread2;
struct sockaddr_in servtcpaddr, servudpaddr, clitcpaddr, cliudpaddr;


void *tcp_client(void *p) {
    int server_port_number, socktcp, option, ID;
    char **argv = p;
    char ID_str[100], players[MAXLINE], player_port[MAXLINE];
    socklen_t lencli;
    
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

                printf("Choose one player to play with by ID:");
                scanf(" %d", &ID);
                printf("You chose %d\n", ID);

                memset(ID_str, 0, strlen(ID_str));
                sprintf(ID_str, "%d", ID);
                // printf("str: %s\n",ID_str);
                Write(socktcp, ID_str, strlen(ID_str));

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

    pthread_exit(NULL);

}

void *udp_server(void *p) {
    return;
}

int main(int argc, char **argv)
{
    
	

    if (argc != 3)
	{
		printf("I need ip port\n");
		exit(1);
	}

    pthread_create(&thread1, 0, tcp_client, argv);
	// pthread_create(&thread2, 0, udp_server, NULL);
	// pthread_join(thread2, NULL);
	pthread_join(thread1, NULL);
    
	return 0;
}
