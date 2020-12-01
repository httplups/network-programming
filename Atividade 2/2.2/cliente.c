#include "mysockfunctions.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <pthread.h>

#define MAXLINE 4096
char stop[5];
pthread_t thread1, thread2;


/* Function to print the reserve string */
void print_strrev(char *str)
{
	int i, j, temp;
	i = 0;
	j = strlen(str) - 1;

	while (i < j)
	{
		temp = str[i];
		str[i++] = str[j];
		str[j--] = temp;
	}
	printf("Comando Invertido: %s\n", str);
	return;
}

/* Thread used to transfer data with server and execute their commands */
void *handle_server(void *p)
{
	int sockfd = *((int *)p);
	char recvline[MAXLINE + 1];
	char *sendline;
	long fsize;
	bzero(recvline, MAXLINE);
	while (Read(sockfd, recvline, MAXLINE) > 0)
	{
		/* getting command from the server */
		recvline[strcspn(recvline, "\n")] = 0;
		recvline[strlen(recvline)] = 0;

		/* show the reverse form of recvline */
		char reverse[strlen(recvline)];
		strcpy(reverse, recvline);
		print_strrev(reverse);

		/* I wanted to redirect the output of system command to output.txt file */
		strcat(recvline, " > output.txt");

		/* creating temporary file of output */
		FILE *output_file = fopen("output.txt", "a+");

		/* executing the command */
		int errcode = system(recvline);
		if (errcode)
			perror("error in system");

		/* Reading the output.txt */
		fseek(output_file, 0, SEEK_END);
		fsize = ftell(output_file);
		fseek(output_file, 0, SEEK_SET);

		if (fsize <= 0)
		{
			/* The command did not write on output file*/
			sendline = malloc(100 * sizeof(char));
			strcpy(sendline, "Nao obtive resposta para este comando...");
			sendline[strlen(sendline)] = 0;
		}
		else
		{
			/* We get an output */
			sendline = malloc((fsize + 1) * sizeof(char));
			fread(sendline, 1, fsize, output_file);
			sendline[fsize] = 0;
		}

		//printf("Resultado:\n%s\n", sendline);

		/* We send the output to server */
		Write(sockfd, sendline, strlen(sendline));
		bzero(recvline, MAXLINE);

		fclose(output_file);

		/* We remove the temporary file */
		if (remove("output.txt") != 0)
		{
			perror("Unable to remove file");
		}
    	}

	pthread_cancel(thread1); // cancel the thread who waits for the user typing 'exit'
	pthread_exit(NULL); // returns to main thread
}

void *read_stdin(void *p)
{
	bzero(stop, 5);

	/* waiting for the user to type 'exit' */
	while (((strcmp(fgets(stop, sizeof(stop), stdin), "exit") == 0)))
	{
		pthread_cancel(thread2); //cancel the thread above who process the server's request
		break;
	}

	pthread_exit(NULL);
}

int main(int argc, char **argv)
{

	int sockfd, server_port_number;
	char error[MAXLINE + 1], info_server[25];
	struct sockaddr_in servaddr, cliaddr;
	socklen_t lencli, lenserv;


	if (argc != 3)
	{
		strcpy(error, "uso: ");
		strcat(error, argv[0]);
		strcat(error, " <IPaddress>");
		perror(error);
		exit(1);
	}


	/* Creating socket */
	sockfd = Socket(AF_INET, SOCK_STREAM, 0);

	// inicializando o socket com zeros
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;

	// obtendo o numero de porta para se conectar ao servidor
	sscanf(argv[2], "%d", &server_port_number);
	servaddr.sin_port = htons(server_port_number);

	//obtendo o IP para se conectar ao servidor
	Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

	/* Connecting to the server */
	Connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	lencli = sizeof(cliaddr);
	lenserv = sizeof(servaddr);

	// Agora que a conexão foi feita, o connect implicitamente chamou o método bind e associou ao socket um numero de porta e ip local
	GetSockName(sockfd, (struct sockaddr *)&cliaddr, &lencli);
	printf("Informacoes do Socket Local:\n");
	printf("IP: %s\n", inet_ntoa(cliaddr.sin_addr));
	printf("Porta: %d\n", ntohs(cliaddr.sin_port));

    bzero(info_server, 25);
    snprintf(info_server, sizeof(info_server), "%s", sock_ntop((const struct sockaddr *)&servaddr, lenserv));
    info_server[strlen(info_server)] = 0;

	// GetPeerName(sockfd, (struct sockaddr *)&servaddr, &lenserv);
	printf("Informacoes do Socket Remoto:\n");
    printf("%s\n", info_server);
	// printf("IP: %s\n", inet_ntoa(servaddr.sin_addr));
	// printf("Porta: %d\n", ntohs(servaddr.sin_port));

	/* Calling these functions as threads because I want to listen user input while I receive data from server */
	pthread_create(&thread1, 0, read_stdin, NULL);
	pthread_create(&thread2, 0, handle_server, &sockfd);
	pthread_join(thread2, NULL);
	pthread_join(thread1, NULL);

	// terminates connection
	Close(sockfd);
	return 0;
}
