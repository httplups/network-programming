
// Client side implementation of UDP client-server model 
#include "mysockfunctions.h"
  
#define PORT     8080 
#define MAXLINE 1024 
  
// Driver code 
int main() { 
    int sockfd; 
    char sendline[MAXLINE], recvline[MAXLINE + 1]; 
    struct sockaddr_in     servaddr; 
  
    // Creating socket file descriptor 
    sockfd = Socket(AF_INET, SOCK_DGRAM, 0);
  
    memset(&servaddr, 0, sizeof(servaddr)); 
      
    // Filling server information 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_port = htons(PORT); 
    servaddr.sin_addr.s_addr = INADDR_ANY; 
      
    int n;
    socklen_t len; 

    while(fgets(sendline, MAXLINE, stdin) != NULL) {

        Sendto(sockfd, sendline, strlen(sendline),MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr)); 
        printf("Hello message sent.\n"); 
            
        n = Recvfrom(sockfd, &recvline, MAXLINE, MSG_WAITALL, (struct sockaddr *) &servaddr, &len); 
        recvline[n] = '\0'; 
        printf("Server : %s\n", recvline); 
    }
    close(sockfd); 
    return 0; 
} 
