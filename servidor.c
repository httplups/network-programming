#include <time.h>
#include "mysockfunctions.h"
#include <stdint.h>

#define LISTENQ 10
#define MAXDATASIZE 4096

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
	int listenfd, connfd, port, i, num_lines, bufsize;
	struct sockaddr_in servaddr, cliaddr;
	pid_t pid;
    char   buf[MAXDATASIZE];
    time_t ticks;
	socklen_t lenserv, lencli;
	char c, info_cliente[25], resultado[MAXDATASIZE + 25], recvline[MAXDATASIZE];

	if (argc != 2)
	{
		perror("Numero de argumentos insuficientes.\n");
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
   	Listen(listenfd, LISTENQ);

	for (;;) {
		/* Opening connection */
		connfd = Accept(listenfd, (struct sockaddr *)&cliaddr, &lencli);

		bzero(info_cliente, 25);
		snprintf(info_cliente, sizeof(info_cliente), "%s", sock_ntop((const struct sockaddr *)&cliaddr, lencli));
		info_cliente[strlen(info_cliente)] = 0;

		
		if ((pid = Fork()) == 0)
		{
            printf("%s\n", info_cliente);
			Close(listenfd); // filho fecha o socket de listen
            ticks = time(NULL);
            snprintf(buf, sizeof(buf), "%.24s\r\n", ctime(&ticks));
            write(connfd, buf, strlen(buf));
			sleep(10);  
			Close(connfd); // filho fecha a conexao
			exit(0);
		}
		//parent closes connection
		Close(connfd);
	}
    	return (0);
}
