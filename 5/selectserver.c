#include "mysockfunctions.h"
// #define FD_SETSIZE 10

int n_users = 0;

typedef struct {
    char ip[15];
    int port;
    int socket_conn;
    int available;
} Client;

Client clients[FD_SETSIZE];

void initialize_clients() {
    int i;
    for(i=0; i< FD_SETSIZE; i++) {
        strcpy(clients[i].ip, "NULL");
        clients[i].port = -1;
        clients[i].socket_conn = -1; /* -1 indicates available entry */
        clients[i].available = 1;
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

int get_index_by_port(int port) {
    for i;
    for (i = 0; i < FD_SETSIZE; i++) {
        if(clients[i].port == port)
            break;
    }
    return i;
}

char * show_other_players(int socket) {
    char info_players[MAXDATASIZE];
    char * i_ptr = info_players;
    char aux[MAXDATASIZE];

    memset(info_players, 0, strlen(info_players));
    memset(aux, 0, strlen(aux));
    
    // printf("\nID\tIP\tPort\n");
    int i;
    for(i=0; i< FD_SETSIZE; i++) {
        if ((clients[i].socket_conn > 0) && (clients[i].socket_conn != socket) && (clients[i].available == 1)) {
            snprintf(aux, sizeof(aux),"%d\t%s\t%d\n", i, clients[i].ip, clients[i].port);
            strcat(info_players, aux);
        }
    }

    // printf("1 - %s\n", info_players);
    if ( strlen(info_players) == 0) 
        snprintf(info_players, sizeof(info_players),"NULL");

    info_players[strlen(info_players)] = 0;
    // printf("2 - %s\n", info_players);
    return i_ptr;
}

char * get_info_player(int pos) {
    char info[10];
    char * i_ptr = info;

    snprintf(info, sizeof(info),"%d",clients[pos].port);
    return i_ptr;
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

void set_as_offline(int pos) {
    clients[pos].available = 0;
}

void set_as_online(int pos) {
    clients[pos].available = 1;
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
    char    buf[MAXLINE], resp[MAXLINE], otherclient[100], player[100];
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
                memset(buf, 0, strlen(buf));
                if ( (n = Read(sockfd, buf, MAXLINE)) == 0) {
                    /* connection closed by client */
                    Close(sockfd);
                    FD_CLR(sockfd, &allset);
                    clients[i].socket_conn = -1;

                } 
                else{
                    printf("got from client: %s\n", buf);

                    if (strcmp(buf, "get") == 0) {
                        /* send the players available */
                        memset(resp, 0, strlen(resp));
                        memset(otherclient, 0, strlen(otherclient));

                        strcpy(resp, show_other_players(sockfd));
                        Write(sockfd, resp, strlen(resp));

                        Read(sockfd, otherclient, 100); /*this call is blocking*/
                        printf("Id of other: %s\n", otherclient);

                        if (atoi(otherclient) == -1)
                            continue;                         

                        /* Sending port of client */
                        memset(resp, 0, strlen(resp));
                        strcpy(resp, get_info_player(atoi(otherclient)));
                        printf("resp %s\n", resp);
                        Write(sockfd, resp, strlen(resp));

                        // set_as_offline(i);
                        // set_as_offline(otherclient);

                    }
                    else if (strcmp(buf, "back") == 0) {
                        /* Put the client again on the array */
                        memset(player, 0, strlen(player));

                        /* Getting the port number of the player playing with client i */
                        Read(sockfd, player, 100);
                        set_as_online(i);
                        set_as_online(get_index_by_port(player));
                    }
                    else if (strcmp(buf, "playing") == 0) {

                        memset(player, 0, strlen(player));

                        /* Getting the port number of the player playing with client i */
                        Read(sockfd, player, 100);
                        set_as_offline(i);
                        set_as_offline(get_index_by_port(player));
                    }
                    else if (strcmp(buf, "points") == 0) {
                        continue;
                    }
                    // Write(sockfd, buf, n);
                }                   
                   
                
                if (--nready <= 0)
                    break;         /* no more readable descriptors */
            }
        } 
    }
}   