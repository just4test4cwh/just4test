#include      "unp.h"

void str_echo(int sockfd) {
	ssize_t n;
	char    buf[MAXLINE];

	again:
		while ( (n = read(sockfd, buf, MAXLINE)) > 0){
			Writen(sockfd, buf, n);
		}
		if (n < 0 && errno == EINTR) 
			goto again; 
		else if (n < 0) 
			err_sys("str_echo: read error"); 
}

int main(int argc, char **argv){
	int listenfd, connfd;
	socklen_t clilen; 
	struct sockaddr_in cliaddr, servaddr;
	listenfd = Socket (AF_INET, SOCK_STREAM, 0);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl (INADDR_ANY);
	servaddr.sin_port = htons (SERV_PORT);

	Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

	Listen(listenfd, LISTENQ);

	for ( ; ; )  {
		clilen = sizeof(cliaddr);
		connfd = Accept(listenfd, (SA *) &cliaddr, &clilen);
		str_echo(connfd);
		Close(connfd);
	} 
}


