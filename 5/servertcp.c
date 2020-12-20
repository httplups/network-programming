#include <time.h>
#include "mysockfunctions.h"
#include <stdint.h>

#define LISTENQ 10
#define MAXDATASIZE 4096
int n_users = 0;

typedef struct {
    char username[15];
    char ip[15];
    int port;
} User;

void clear_users(User users[10]) {
    int i;
    for(i=0; i<10; i++) {
        strcpy(users[i].username, "NULL");
        strcpy(users[i].ip, "NULL");
        users[i].port = 0;
    }
}

void show_users(User users[10]) {
    int i;
    for(i=0; i<10; i++) {
        printf("%d -\t%s\n",i,users[i].username);
    }
}

void insert_user(char *username, char * ip, int port, User users[10]) {

    strcpy(users[n_users].username,  username);
    strcpy(users[n_users].ip, ip);
    users[n_users].port = port;

    printf("New user: %s\n%s\t%d\n", username, ip, port);
    
}

int main(int argc, char **argv)
{
	int listenfd, connfd, port, i, num_lines, bufsize, n;
	struct sockaddr_in servaddr, cliaddr;
	pid_t pid;
	socklen_t lenserv, lencli;
    User users[10];
	char recvline[MAXDATASIZE], option;
    char *welcome = "Welcome! If you want to join the game, enter your username:\n"; 
    char *menu = "Choose one option below:\n\n1 - Invite someone to play with\n3 - Quit\n";

	if (argc != 2)
	{
		perror("Wrong number os args\n");
		exit(0);
	}

    clear_users(users);

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
	// GetSockName(listenfd, (struct sockaddr *)&servaddr, &lenserv);
	// printf("IP: %s\n", inet_ntoa(servaddr.sin_addr));
	// printf("Porta: %d\n", ntohs(servaddr.sin_port));

	/* Server listening... */
   	Listen(listenfd, LISTENQ);

	for (;;) {
		/* Opening connection */
		connfd = Accept(listenfd, (struct sockaddr *)&cliaddr, &lencli);
        
		
		if ((pid = Fork()) == 0)
		{
            char username[10], otheruser[10];

			Close(listenfd); // filho fecha o socket de listen
            memset(username, 0, strlen(username));
            memset(otheruser, 0, strlen(otheruser));


            Write(connfd, welcome, strlen(welcome));
            Read(connfd, username, 10);
            username[strlen(username) -1] = 0;
            GetPeerName(connfd, (struct sockaddr *)&cliaddr, &lencli);
            insert_user(username, inet_ntoa(cliaddr.sin_addr),ntohs(cliaddr.sin_port), users);

            do {
                Write(connfd, menu, strlen(menu));
                n = Read(connfd, &option, 1);
                printf("op: %c\n", option);
                switch (option) {
                    case '1': {
                        // show users
                        show_users(users);
                        Read(connfd, otheruser, 10);
                        otheruser[strlen(otheruser) -1] = 0;
                        printf("O %s quer jogar com %s\n", username, otheruser);
                    }
                    case '2': {
                        // close conn
                        n = 0;
                        break;
                    }
                }
            } while(n > 0);

            // bzero(info_cliente, 25);
            // snprintf(info_cliente, sizeof(info_cliente), "%s", sock_ntop((const struct sockaddr *)&cliaddr, lencli));
            // info_cliente[strlen(info_cliente)] = 0;
            // printf("%s\n", info_cliente);

            // // Adiciona-o à fila de jogadores
            // insert_user();


            // bufsize = Read(connfd, recvline, MAXDATASIZE);

            // if (bufsize > 0)
            // {
            //     // ip e porta do cliente sem a resposta do resultado
            //     printf("%s\n", info_cliente);
                
                
            //     fputs(resultado, stdout);

            //     /* Coloquei este sleep apenas para conseguir ver
            //         o funcionamento de uma forma mais devagar,
            //         ele não impede a concorrencia
            //     */
            // }
            // else
            // {
            //     // Se não recebe mais dados do servidor, termina
            //     break;
            // }
				
			Close(connfd); // filho fecha a conexao
			
			exit(0);
		}
		//parent closes connection
		Close(connfd);
	}
    	return (0);
}
