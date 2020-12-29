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
    printf("cliente tcp: %s:%d\n", inet_ntoa(clitcpaddr.sin_addr), ntohs(clitcpaddr.sin_port));

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
                    sprintf(ID_str, "%d", -1);
                    Write(socktcp, ID_str, strlen(ID_str));
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
    int sockudp, n;
    socklen_t lencli, lenserv;
    char buffer[MAXLINE];

    /* STARTING A UDP SERVER */
    sockudp = Socket(AF_INET, SOCK_DGRAM, 0);
    bzero(&servudpaddr, sizeof(servudpaddr));
    bzero(&cliudpaddr, sizeof(cliudpaddr));

    // Filling me-as-server information 
    servudpaddr.sin_family    = AF_INET; // IPv4 
    servudpaddr.sin_addr.s_addr = INADDR_ANY; 
    servudpaddr.sin_port = clitcpaddr.sin_port; /* My server UDP port is the same as my TCP client port*/
    // servudpaddr.sin_port = 0;
    // Bind the socket with the my server udp address 
    Bind(sockudp, (const struct sockaddr *)&servudpaddr,sizeof(servudpaddr));

    lenserv = sizeof(servudpaddr);
    GetSockName(sockudp, (struct sockaddr *)&servudpaddr, &lenserv);
    printf("server udp: %s:%d\n", inet_ntoa(servudpaddr.sin_addr), ntohs(servudpaddr.sin_port));

    while (1) {
        lencli = sizeof(cliudpaddr);  //len is value/resuslt 
        n = Recvfrom(sockudp, &buffer, MAXLINE, MSG_WAITALL, (struct sockaddr *) &cliudpaddr, &lencli);
        buffer[n] = '\0'; 
        printf("Client : %s\n", buffer); 
        Sendto(sockudp, buffer, strlen(buffer),MSG_CONFIRM, (const struct sockaddr *) &cliudpaddr, lencli);  
    }
}

int main(int argc, char **argv)
{
    
	

    if (argc != 3)
	{
		printf("I need ip port\n");
		exit(1);
	}

    pthread_create(&thread1, 0, tcp_client, argv);
    sleep(2); /* Fiz isso pro tcp ja ter definido a porta antes de o udp utilizar */
	pthread_create(&thread2, 0, udp_server, NULL);
    
	pthread_join(thread2, NULL);
	pthread_join(thread1, NULL);
    
	return 0;
}
