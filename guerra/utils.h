#include <signal.h>

int Socket(int family, int type, int flags){
	int sockfd;
	if ((sockfd = socket(family, type, flags)) < 0){
		perror("socket");
		exit(1);
	}else
		return sockfd;
}

char *sock_ntop(const struct sockaddr *sa, socklen_t salen){
	char portstr[8];
	static char str[128];

	switch(sa->sa_family){
		case AF_INET: {
			struct sockaddr_in *sin = (struct sockaddr_in*) sa;
			if (inet_ntop(AF_INET,&sin->sin_addr,str, sizeof(str)) == NULL)
			{
				return NULL;
			}
			if (ntohs(sin->sin_port) != 0)
			{
				snprintf(portstr, sizeof(portstr), ":%d",ntohs(sin->sin_port));
				strcat(str,portstr);
				return (str);
			}
		}
	}
	return NULL;
}

void Inet_pton(int af, const char *src, void *dst){
	if(inet_pton(af, src, dst) <= 0){
		perror("inet_pton error");
		exit(1);
	}
}

void Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen){
	if (connect(sockfd, addr, addrlen) < 0){
		perror("connect error");
		exit(1);
	}
}

void Getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen){
	if(getsockname(sockfd, addr, addrlen) == -1){
		perror("sockName");
		exit(1);
	}
}

void Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen){
	if (bind(sockfd, addr, addrlen) == -1){
		perror("bind");
		exit(1);
	}
}

void Listen(int sockfd, int backlog){
	if (listen(sockfd, backlog) == -1){
		perror("listen");
		exit(1);
	}
}

int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen){
	int connfd;
	connfd = accept(sockfd, addr, addrlen);
	if (connfd == -1){
		perror("accept");
		exit(1);
	}
	return connfd;
}

void reverse(char * string, int begin, int end){
	char c;

	if(begin >= end)
		return;

	c = *(string+begin);
	*(string+begin) = *(string+end);
	*(string+end) = c;
	reverse(string, ++begin, --end);
}

typedef void Sigfunc(int);
Sigfunc * Signal (int signo, Sigfunc *func){
	struct sigaction act, oact;
	act.sa_handler = func;
	sigemptyset(&act.sa_mask); /* outros sinais não são bloqueados */
	act.sa_flags = 0;
	if (signo == SIGALRM) { /* Para reiniciar chamadas interrompidas */
#ifdef SA_INTERRUPT
	act.sa_flags |= SA_INTERRUPT; /* SunOS 4.x */
#endif
	} else{
#ifdef SA_RESTART
	act.sa_flags |= SA_RESTART; /* SVR4, 4.4BSD */
#endif
	}
	if(sigaction(signo, &act, &oact) < 0)
		return (SIG_ERR);
	return (oact.sa_handler);
}