#include "mysockfunctions.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <pthread.h>

#define MAXLINE 4096
pthread_t thread1, thread2, thread3;
struct sockaddr_in servtcpaddr, serv_local_udp_addr, serv_remote_udp_addr, clitcpaddr, cliudpaddr;

void *udp_client(void *p) {
    int player_port = *((int *)p);
    int sockudp_remote, n;
    char buffer[MAXLINE];
    printf("Trying to connect to %d by UDP\n", player_port);

    sockudp_remote = Socket(AF_INET, SOCK_DGRAM, 0);
  
    memset(&serv_remote_udp_addr, 0, sizeof(serv_remote_udp_addr)); 
      
    // Filling server information 
    serv_remote_udp_addr.sin_family = AF_INET; 

    serv_remote_udp_addr.sin_port = htons(player_port);
    serv_remote_udp_addr.sin_addr.s_addr = INADDR_ANY; 

    Connect(sockudp_remote, (const struct sockaddr *) &serv_remote_udp_addr, sizeof(serv_remote_udp_addr));
    char * hello = "Hello, Let's play now?! (yes/no)";
    
    Write(sockudp_remote, hello, strlen(hello));
    Read(sockudp_remote, buffer, MAXLINE);

    if (strcmp(buffer, "no") == 0)
        printf("You'll need to choose another player...\n");
    else {
        printf("Started! ...\n");
        sleep(10);
    }

    Close(sockudp_remote);
    // while ((n = Read(sockudp_remote, buffer, MAXLINE)) > 0) {
    //     printf("%s\n", buffer);
    // }

    pthread_exit(NULL);
}

void *tcp_client(void *p) {
    int server_port_number, socktcp, option, ID, port;
    char **argv = p;
    char * get = "get";
    char * back = "back";
    char * playing = "playing";
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

                /* getting port of player assuming it's listening on 0.0.0.0 */
                Read(socktcp, player_port, MAXLINE);
                printf("Port: %s\n", player_port);
                port = atoi(player_port);
                // sprintf(player_port, "%d", port);
                printf("PORTA: %d\n", port);

                /* Trying to play, so offline for now */
                Write(socktcp, playing, strlen(playing));
                Write(socktcp, player_port, strlen(player_port));

                pthread_create(&thread3, 0, udp_client, &port);
                pthread_join(thread3, NULL);

                /* Already played or could not */
                Write(socktcp, back, strlen(back));
                sleep(1);
                Write(socktcp, player_port, strlen(player_port));
                
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
    char buffer[MAXLINE], sendline[MAXLINE];

    /* STARTING A UDP SERVER */
    sockudp = Socket(AF_INET, SOCK_DGRAM, 0);
    bzero(&serv_local_udp_addr, sizeof(serv_local_udp_addr));
    bzero(&cliudpaddr, sizeof(cliudpaddr));

    // Filling me-as-server information 
    serv_local_udp_addr.sin_family    = AF_INET; // IPv4 
    serv_local_udp_addr.sin_addr.s_addr = INADDR_ANY; 
    serv_local_udp_addr.sin_port = clitcpaddr.sin_port; /* My server UDP port is the same as my TCP client port*/
    // serv_local_udp_addr.sin_port = 0;
    // Bind the socket with the my server udp address 
    Bind(sockudp, (const struct sockaddr *)&serv_local_udp_addr,sizeof(serv_local_udp_addr));

    lenserv = sizeof(serv_local_udp_addr);
    GetSockName(sockudp, (struct sockaddr *)&serv_local_udp_addr, &lenserv);
    // printf("server udp: %s:%d\n", inet_ntoa(serv_local_udp_addr.sin_addr), ntohs(serv_local_udp_addr.sin_port));
    while (1) {
        memset(sendline, 0, strlen(sendline));
        memset(buffer, 0, strlen(buffer));
        // Write(sockudp, hello, strlen(hello));
        lencli = sizeof(cliudpaddr);  //len is value/resuslt 
        n = Recvfrom(sockudp, &buffer, MAXLINE, MSG_WAITALL, (struct sockaddr *) &cliudpaddr, &lencli);
        printf("n: %d\n", n);
        if (n == 0)
            break;

        buffer[n] = '\0'; 
        printf("%s\n", buffer); 

        fgets(sendline, MAXLINE, stdin);
        sendline[strlen(sendline) -1] = '\0';
        printf("%s",sendline);

        Sendto(sockudp, sendline, strlen(sendline),MSG_CONFIRM, (const struct sockaddr *) &cliudpaddr, lencli);  
    }
    pthread_exit(NULL);
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
