#include "mysockfunctions.h"
// #define FD_SETSIZE 10

int n_users = 0;

typedef struct {
    char ip[15];
    int port;
    int socket_conn;
} Client;

Client clients[FD_SETSIZE];

void initialize_clients() {
    int i;
    for(i=0; i< FD_SETSIZE; i++) {
        strcpy(clients[i].ip, "NULL");
        clients[i].port = -1;
        clients[i].socket_conn = -1; /* -1 indicates available entry */
    }
}

void show_clients() {
    printf("\nID\tIP\tPort\n");
    int i;
    for(i=0; i< FD_SETSIZE; i++) {
        if (clients[i].socket_conn > 0)
            // printf("%d -\t%s\n",i,clients[i].username);
            printf("%d\t%s\t%d\n", i, clients[i].ip, clients[i].port);
    }
}

char * show_other_players(int socket) {
    char info_players[MAXDATASIZE];
    char aux[MAXDATASIZE];
    
    snprintf(info_players, sizeof(info_players),"\nID\tIP\tPort\n");
    // printf("\nID\tIP\tPort\n");
    // int i;
    // for(i=0; i< FD_SETSIZE; i++) {
    //     if ((clients[i].socket_conn > 0) && (clients[i].socket_conn != socket)) {
    //         snprintf(aux, sizeof(aux),"%d\t%s\t%d\n", i, clients[i].ip, clients[i].port);
    //         strcat(info_players, aux);
    //     }
    // }

    return info_players;
}

int insert_client_socket(int socket) {
    int i;
    for (i = 0; i < FD_SETSIZE; i++) {
        /* Add client at the first avaiable position */
        if (clients[i].socket_conn < 0) {
            clients[i].socket_conn = socket;
            break;
        }
    }
    printf("New client inserted at %d\n", i);
    /*The client's info is still empty */
    return i;
    
}
int  insert_client(char * ip, int port, int socket) {
    int i;
    for (i = 0; i < FD_SETSIZE; i++) {
        /* Add client at the first avaiable position */
        if (clients[i].socket_conn < 0) {
            strcpy(clients[i].ip, ip);
            clients[i].port = port;
            clients[i].socket_conn = socket;
            break;
        }
    }

    printf("(%s:%d) inserted at %d\n", ip, port, i);

    return i;
}

int main(int argc, char **argv) {
    int     i, maxi, maxfd, listenfd, connfd, sockfd, port;
    int     nready;
    ssize_t n;
    fd_set  rset, allset;
    char    buf[MAXLINE], option;
    socklen_t  clilen;
    struct sockaddr_in cliaddr, servaddr;

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    sscanf(argv[1], "%d", &port); // getting port number from argv
	servaddr.sin_port = htons(port);

    // servaddr.sin_port = htons(SERV_PORT);

    Bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    Listen(listenfd, LISTENQ);

    maxfd = listenfd;            /* initialize */
    maxi = -1;                   /* index into client[] array */
    // for (i = 0; i < FD_SETSIZE;  i++)
    //     client[i] = -1;          /* -1 indicates available entry */
    initialize_clients();
    FD_ZERO(&allset);
    FD_SET(listenfd, &allset);

    for ( ; ; ) {
        rset = allset;          /* structure assignment */
        nready = Select(maxfd + 1, &rset, NULL, NULL, NULL);
        
        if (FD_ISSET(listenfd, &rset)) {       /* new client connection */
            clilen = sizeof(cliaddr);
            // connfd = Accept(listenfd, (SA *) &cliaddr, &clilen);
            connfd = Accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);

            // char username[15], otheruser[15];
            // memset(username, 0, strlen(username));
            // memset(otheruser, 0, strlen(otheruser));

            // Write(connfd, welcome, strlen(welcome));
            // Read(connfd, username, 15);
            // username[strlen(username) -1] = 0;
            GetPeerName(connfd, (struct sockaddr *)&cliaddr, &clilen);
            i = insert_client(inet_ntoa(cliaddr.sin_addr),ntohs(cliaddr.sin_port), connfd);
            
            // for (i = 0; i < FD_SETSIZE; i++)
            //     if (client[i] < 0) {
            //         client[i] = connfd; /* save descriptor */
            //         break;
            //     }

            
            // i = insert_client_socket(connfd);
            if (i == FD_SETSIZE) {
                printf("too many clients");
                exit(1);
            }

            FD_SET(connfd, &allset);       /* add new descriptor to set */

            if (connfd > maxfd)
                maxfd = connfd; /* for select */
            if (i > maxi)
                maxi = i;          /* max index in client[] array */

            if (--nready <= 0)
                continue;          /* no more readable descriptors */
            
        }
        
        for (i = 0; i <= maxi; i++) {       /* check all clients for data */

            if ( (sockfd = clients[i].socket_conn) < 0) /* check connection socket for each client */
                continue;

            if (FD_ISSET(sockfd, &rset)) {
                if ( (n = Read(sockfd, buf, MAXLINE)) == 0) {
                    /* connection closed by client */
                    Close(sockfd);
                    FD_CLR(sockfd, &allset);
                    clients[i].socket_conn = -1;

                } 
                else{
                    printf("%s\n", buf);

                    if (atoi(buf) == 1) {
                        /* send the players available */

                    }
                    // Write(sockfd, buf, n);
                }                   
                   
                
                if (--nready <= 0)
                    break;         /* no more readable descriptors */
            }
        } 
    }
}   