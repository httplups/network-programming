#include "mysockfunctions.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <pthread.h>

#define MAXLINE 4096
pthread_t thread1, thread2;


void *tcp_client(void *p) {
    int server_port_number;
    char **argv = p;
    sscanf(argv[2], "%d", &server_port_number);
    printf("arg: %d\n", server_port_number);
}

void *udp_server(void *p) {
    return;
}

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

    pthread_create(&thread1, 0, tcp_client, argv);
	// pthread_create(&thread2, 0, udp_server, NULL);
	// pthread_join(thread2, NULL);
	pthread_join(thread1, NULL);
    
	return 0;
}
