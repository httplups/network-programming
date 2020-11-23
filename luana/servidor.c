#include <time.h>
#include "mysockfunctions.h"
#include <stdint.h>
#include <sys/wait.h> 

// #define LISTENQ 10
#define MAXDATASIZE 4096

void sig_chld(int signo) {
    pid_t pid;
    int stat;
    while ( (pid = waitpid(-1, &stat, WNOHANG)) > 0)
        printf("child %d terminated\n", pid);
    return;
}

/* Function to retrive client information */
char *sock_ntop(const struct sockaddr *sa, socklen_t salen)
{
	char portstr[8];
	static char str[128];

	switch (sa->sa_family) {
		case AF_INET: {
			struct sockaddr_in *sin = (struct sockaddr_in *)sa;
			if (inet_ntop(AF_INET, &sin->sin_addr, str, sizeof(str)) == NULL)
			{
				return NULL;
			}
			if (ntohs(sin->sin_port) != 0)
			{
				snprintf(portstr, sizeof(portstr), ":%d", ntohs(sin->sin_port));
				strcat(str, portstr);
				return str;
			}
		}
	}

    return NULL;
}

int main(int argc, char **argv)
{
	int listenfd, connfd, port, backlog;
	struct sockaddr_in servaddr, cliaddr;
	pid_t pid;
	socklen_t lenserv, lencli;
	char info_cliente[25];

	if (argc != 3)
	{
		perror("Numero de argumentos insuficientes. Preciso da porta e backlog\n\n");
		exit(0);
	}

	/* Creating listening parent socket */
    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;

	servaddr.sin_addr.s_addr = INADDR_ANY; // deixando o ip do localhost automatico
	sscanf(argv[1], "%d", &port); //recebendo a porta pelo argumento
	servaddr.sin_port = htons(port);

	// associando a porta e ip ao socket
	Bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

	lenserv = sizeof(servaddr);
	lencli = sizeof(cliaddr);
	GetSockName(listenfd, (struct sockaddr *)&servaddr, &lenserv);
	printf("IP para conectar: %s\n", inet_ntoa(servaddr.sin_addr));
	printf("Numero de porta para voce se conectar: %d\n", ntohs(servaddr.sin_port));

	/* Server listening... */
    sscanf(argv[2], "%d", &backlog);
   	Listen(listenfd, backlog);

    Signal(SIGCHLD, sig_chld); // parent waits for signal to handle it

	for (;;) {
        sleep(3);
		/* Opening connection */
		connfd = Accept(listenfd, (struct sockaddr *)&cliaddr, &lencli);
		bzero(info_cliente, 25);
		snprintf(info_cliente, sizeof(info_cliente), "%s", sock_ntop((const struct sockaddr *)&cliaddr, lencli));
		info_cliente[strlen(info_cliente)] = 0;
		if ((pid = Fork()) == 0)
		{
			Close(listenfd); // filho fecha o socket de listen
            printf("Handling: %s\n", info_cliente);
            sleep(3); // para segurar a conexão um pouco
			Close(connfd); // filho fecha a conexao
			exit(0);
		}
		//parent closes connection
		Close(connfd);
	}
    return (0);  
}
