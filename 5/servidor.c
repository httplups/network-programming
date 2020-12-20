
// Server side implementation of UDP client-server model 
#include "mysockfunctions.h"
  
#define PORT     8080 
#define MAXLINE 1024 
  
// Driver code 
int main() { 
    int sockfd, n;
    socklen_t len; 
    char buffer[MAXLINE]; 
    char *hello = "Hello from server"; 
    struct sockaddr_in servaddr, cliaddr; 
      
    // Creating socket file descriptor 
    sockfd = Socket(AF_INET, SOCK_DGRAM, 0);
      
    memset(&servaddr, 0, sizeof(servaddr)); 
    memset(&cliaddr, 0, sizeof(cliaddr)); 
      
    // Filling server information 
    servaddr.sin_family    = AF_INET; // IPv4 
    servaddr.sin_addr.s_addr = INADDR_ANY; 
    servaddr.sin_port = htons(PORT); 
      
    // Bind the socket with the server address 
    Bind(sockfd, (const struct sockaddr *)&servaddr,sizeof(servaddr));
      
    while (true) {
        len = sizeof(cliaddr);  //len is value/resuslt 
        n = Recvfrom(sockfd, &buffer, MAXLINE, MSG_WAITALL, (struct sockaddr *) &cliaddr, &len);
        buffer[n] = '\0'; 
        printf("Client : %s\n", buffer); 
        Sendto(sockfd, hello, strlen(hello),MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len); 
        printf("Hello message sent.\n");  
    }
      
    return 0; 
} 
