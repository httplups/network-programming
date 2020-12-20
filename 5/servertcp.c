#include <time.h>
#include "mysockfunctions.h"
#include <stdint.h>

#define LISTENQ 10
#define MAXDATASIZE 4096

typedef struct {
    char username[15];
    char ip[15];
    int port;
} User;

void clear_users(User *users[10]) {
    int i;
    for(i=0; i<10; i++) {
        strcpy(users[i]->username, "NULL");
        strcpy(users[i]->ip, "NULL");
        users[i]->port = NULL;
    }
}
int main(int argc, char **argv)
{
	int listenfd, connfd, port, i, num_lines, bufsize;
    int participantes[10];
	struct sockaddr_in servaddr, cliaddr;
	pid_t pid;
	socklen_t lenserv, lencli;
    User users[10];
	char c, info_cliente[25], resultado[MAXDATASIZE + 25], recvline[MAXDATASIZE];

	if (argc != 2)
	{
		perror("Wrong number os args\n");
		exit(0);
	}

    clear_users(&users);
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

        // Adiciona-o à fila de jogadores
        
        // participantes[sizeof(participantes)]
		bzero(info_cliente, 25);
		snprintf(info_cliente, sizeof(info_cliente), "%s", sock_ntop((const struct sockaddr *)&cliaddr, lencli));
		info_cliente[strlen(info_cliente)] = 0;
        printf("%s\n", info_cliente);
		
		// if ((pid = Fork()) == 0)
		// {

		// 	Close(listenfd); // filho fecha o socket de listen
		// 	bzero(recvline, MAXDATASIZE);

        //     bufsize = Read(connfd, recvline, MAXDATASIZE);

        //     if (bufsize > 0)
        //     {
        //         // ip e porta do cliente sem a resposta do resultado
        //         printf("%s\n", info_cliente);
                
                
        //         fputs(resultado, stdout);

        //         /* Coloquei este sleep apenas para conseguir ver
        //             o funcionamento de uma forma mais devagar,
        //             ele não impede a concorrencia
        //         */
        //     }
        //     else
        //     {
        //         // Se não recebe mais dados do servidor, termina
        //         break;
        //     }
				
		// 	Close(connfd); // filho fecha a conexao
			
		// 	exit(0);
		// }
		//parent closes connection
		Close(connfd);
	}
    	return (0);
}
