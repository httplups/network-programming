#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>

#define LISTENQ 10
#define MAXDATASIZE 4096
#define MAXLINE 4096
#define max(a,b) (((a)>(b))?(a):(b))

char* integer_to_string(int x)
{
    char* buffer = malloc(sizeof(char) * sizeof(int) * 4 + 1);
    if (buffer)
    {
         sprintf(buffer, "%d", x);
    }
    return buffer; // caller is expected to invoke free() on this buffer to release memory
}

int Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout){

    return select(nfds, readfds, writefds, exceptfds, timeout);

}

int Socket(int family, int type, int flags) {
	int sockfd;
	if((sockfd = socket(family, type, flags)) < 0){
		perror("socket");
		exit(1);
	}
	else
		return sockfd;
}

void Listen(int fd, int backlog) {
	char *ptr;

	if ((ptr = getenv("LISTENQ")) != NULL) 
		backlog = atoi(ptr);
	if (listen(fd,backlog) < 0) {
		perror("listen error");
		exit(1);
	}
}

void Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
	if (bind(sockfd, addr, addrlen) == -1) {
		perror("bind");
		exit(1);
	}
}

int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
	int connfd;
	if ((connfd = accept(sockfd, addr, addrlen)) == -1) {
		// possible error in using invalid socket
		perror("accept");
		exit(1);
	}
	else
		return connfd;
}

void GetSockName(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
     if (getsockname(sockfd, addr, addrlen) == -1) {
        perror("getsockname");
        exit(1);
    }
}

void GetPeerName(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {

     if (getpeername(sockfd, addr, addrlen) == -1) {
        perror("getpeername");
        exit(1);
    }
}

void Write(int fd, const void *buf, size_t count){ 
    ssize_t n = write(fd, buf, count); 
    if (n == -1) {
	perror("write");
	exit(1);
    }
    //printf("return value of write: %li\n", n);
}

int Read(int fd, void *buf, size_t count){ 
    ssize_t n;
    n = read(fd, buf, count);
    if (n < 0) {
        perror("read error");
        exit(1);
    }
    else 
        return n;
}

void Inet_pton(int af, const char *src, void *dst) {
    if (inet_pton(af, src, dst) <= 0) {
        perror("inet_pton error");
        exit(1);
    }
}

void Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    if (connect(sockfd, addr, addrlen) < 0) {
        perror("connect error");
        exit(1);
    }
}

void Close(int fd) {
    close(fd);
}

pid_t Fork() {
	pid_t pid;
	pid = fork();
	return pid;
}

void Shutdown(int socket, int how) {
	if (shutdown(socket, how) == -1) 
		perror("shutdown");
}

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

ssize_t Recvfrom(int sockfd, void *buf, size_t len, int flags,
                 struct sockaddr *src_addr, socklen_t *addrlen) {
    return recvfrom(sockfd,buf,len,flags,src_addr,addrlen);
}

ssize_t Sendto(int sockfd, const void *buf, size_t len, int flags,
               const struct sockaddr *dest_addr, socklen_t addrlen) {
    return sendto(sockfd,buf,len,flags,dest_addr,addrlen);
}