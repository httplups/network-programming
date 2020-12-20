#include "mysockfunctions.h"

#define SERV_PORT 9877
#define MAXLINE 10000

void dg_cli(FILE *fp, int sockfd, const SA *pservaddr, socklen_t servlen) {
    int     n;
    char    sendline[MAXLINE], recvline[MAXLINE + 1];
    socklen_t len;
    struct sockaddr *preply_addr;

    preply_addr = malloc(servlen);

    while (Fgets(sendline, MAXLINE, fp) != NULL) {

        Sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);

        len = servlen;
        n = Recvfrom(sockfd, recvline, MAXLINE, 0, preply_addr, &len);

        if (len != servlen || memcmp(pservaddr, preply_addr, len) != 0) {
            printf("reply from %s (ignored)\n", sock_ntop(preply_addr, len));
            continue;
        }

        recvline[n] = 0;        /* null terminate */
        Fputs(recvline, stdout);
    }
}

int main(int argc, char **argv) {
    int     sockfd;
    struct sockaddr_in servaddr;
    if(argc != 2)
        err_quit("usage: udpcli <IPaddress>");
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(7); // standart echo server

    Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
    sockfd = Socket(AF_INET, SOCK_DGRAM, 0);

    dg_cli(stdin, sockfd, (SA *) &servaddr, sizeof(servaddr));
    exit(0);
}

