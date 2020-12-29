
// Client side implementation of UDP client-server model 
#include "mysockfunctions.h"
  
#define PORT     8080 
  
// Driver code 
int main(int argc, char **argv) { 
    int sockfd, port; 
    char sendline[MAXLINE], recvline[MAXLINE + 1]; 
    struct sockaddr_in     servaddr; 
  
    // Creating socket file descriptor 
    sockfd = Socket(AF_INET, SOCK_DGRAM, 0);
  
    memset(&servaddr, 0, sizeof(servaddr)); 
    printf("oi\n");
      
    // Filling server information 
    servaddr.sin_family = AF_INET; 

    sscanf(argv[1], "%d", &port);
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = INADDR_ANY; 
      
    int n;
    socklen_t len; 

    Connect(sockfd, (const struct sockaddr *) &servaddr, sizeof(servaddr));

    while(fgets(sendline, MAXLINE, stdin) != NULL) {

        Write(sockfd, sendline, strlen(sendline));
        // Sendto(sockfd, sendline, strlen(sendline),MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr)); 
        printf("Hello message sent.\n"); 
        
        n = Read(sockfd, recvline, MAXLINE);
        // n = Recvfrom(sockfd, &recvline, MAXLINE, MSG_WAITALL, (struct sockaddr *) &servaddr, &len); 
        recvline[n] = '\0'; 
        printf("Server : %s\n", recvline); 
    }
    close(sockfd); 
    return 0; 
} 
