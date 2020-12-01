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

/* Function to write in the log.txt file the current time of host
   Params: IP:port from client and flag of connect or disconnected message
 */
void setCurrentTime(char *cliente, int flag)
{
	time_t now = time(NULL);
	FILE *f;
	char time[100];
	char buf[150];
	bzero(time, 100);
	bzero(buf, 150);

	if (flag)
		snprintf(buf, sizeof(buf), "%s", "Connected: ");
	else
		snprintf(buf, sizeof(buf), "%s", "Disconnected: ");

	f = fopen("log.txt", "a");

	/* Get the current time */
	strftime(time, sizeof time, "%F %H:%M:%S\n\n", localtime(&now));
	time[strlen(time)] = 0;
	//printf("time: %s",time);

	strcat(buf, cliente);
	buf[strlen(buf)] = '\n';
	buf[strlen(buf)] = 0;
	strcat(buf, time);

	if (f)
		fputs(buf, f);

	fclose(f);
}

int main(int argc, char **argv)
{
	int listenfd, connfd, port, i, num_lines, bufsize;
	struct sockaddr_in servaddr, cliaddr;
	pid_t pid;
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

	printf("Enter a number of commands:\n");
	scanf("%d", &num_lines);

    	// tem um \n que ta atrapalhando
    	scanf("%c", &c);
    	printf("Type your commands\n");
    	char **commands = malloc(num_lines * sizeof(char *));
	
	for (i = 0; i < num_lines; i++)
	{
		commands[i] = malloc(1024 * sizeof(char));
	}

	/* Receiving list of commands from user */
	for (i = 0; i < num_lines; i++)	{
		fgets(commands[i], 1024, stdin);
		commands[i][strlen(commands[i]) - 1] = 0;
		//printf("%s\n", commands[i]);
	}

	for (;;) {
		/* Opening connection */
		connfd = Accept(listenfd, (struct sockaddr *)&cliaddr, &lencli);

		bzero(info_cliente, 25);
		snprintf(info_cliente, sizeof(info_cliente), "%s", sock_ntop((const struct sockaddr *)&cliaddr, lencli));
		info_cliente[strlen(info_cliente)] = 0;
		setCurrentTime(info_cliente, 1); //sets times that client connected

		
		if ((pid = Fork()) == 0)
		{

			Close(listenfd); // filho fecha o socket de listen
			bzero(recvline, MAXDATASIZE);

			for (i = 0; i < num_lines; i++)
			{
				FILE *f = fopen("log.txt", "a");
				bzero(resultado, MAXDATASIZE + 25);
				
				/* Coloca cabeçalho com informações do cliente para escrever no arquivo */
				strcpy(resultado, info_cliente);
				resultado[strlen(resultado)] = '\n';
				resultado[(strlen(resultado))] = 0;
				//printf("Resultado: %s\n", resultado);

				bzero(recvline, MAXDATASIZE);
				//printf("Command: %s\n", commands[i]);

				/* Envia comando para o cliente */
				Write(connfd, commands[i], strlen(commands[i]));

				bufsize = Read(connfd, recvline, MAXDATASIZE);

				if (bufsize > 0)
				{
					strcat(resultado, recvline);

					// ip e porta do cliente sem a resposta do resultado
					printf("%s\n", info_cliente);
					
					// SAÍDA PADRÃO: ip, porta e resposta do cliente
					//printf("Resultado:\n%s\n", resultado);

					strcat(resultado, "\n\n");
					if (f)
						fputs(resultado, f);

					/* Coloquei este sleep apenas para conseguir ver
					   o funcionamento de uma forma mais devagar,
					   ele não impede a concorrencia
					*/
					sleep(3);
				}
				else
				{
					// Se não recebe mais dados do servidor, termina
					break;
				}
				fclose(f);
			}

			Close(connfd); // filho fecha a conexao
			setCurrentTime(info_cliente, 0); // sets time that client disconnected
			exit(0);
		}
		//parent closes connection
		Close(connfd);
	}
    	return (0);
}
