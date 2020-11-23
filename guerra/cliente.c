#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include "utils.h"

#define MAXLINE 4096
#define MAX_STRING_SIZE 100

int main(int argc, char **argv) {
   int    sockfd;
   char   error[MAXLINE + 1];
   struct sockaddr_in servaddr;
   char quit[MAXLINE + 1];

   strcpy(quit, "quit");

   if (argc != 3) {
      strcpy(error,"uso: ");
      strcat(error,argv[0]);
      strcat(error," <IPaddress>");
      perror(error);
      exit(1);
   }

   sockfd = Socket(AF_INET, SOCK_STREAM, 0); // cria o socket para conexão
    
   int portNumber;
   sscanf(argv[2],"%d",&portNumber); // recebe o numero da porta do servidor por parametro
   bzero(&servaddr, sizeof(servaddr));
   servaddr.sin_family = AF_INET; 
   servaddr.sin_port   = htons(portNumber);
   Inet_pton(AF_INET, argv[1], &servaddr.sin_addr); //converte o IP do servidor (texto) para binário

   Connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)); //conecta ao servidor através do socket criado anteriormente
   
   socklen_t len = sizeof(servaddr);
   Getsockname(sockfd, (struct sockaddr *) & servaddr, &len); //recupera informações do socket local
   printf("Local port number %d\n", ntohs(servaddr.sin_port)); //printa o número da porta local
   printf("Local IP address %s\n", inet_ntoa(servaddr.sin_addr)); //printa o IP local

   sleep(3);

   exit(0);
}
