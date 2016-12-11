#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <linux/in.h>
#include <errno.h>

#define MAXLINE 1024

void str_echo(int sockfd){
	ssize_t n;
	char buf[MAXLINE];
	again:
		while((n=read(sockfd,buf,MAXLINE))>0)
			if(write(sockfd,buf,n)<0){
				printf("write error\n");
			}
		if(n<0&&errno==EINTR)
			goto again;
		else if(n<0)
			printf("str_echo:read error\n");
}
int main(int argc , char **argv){
	struct sockaddr_in servaddr,cliaddr;
	int port=1234;
	socklen_t clilen;
	int listenfd,connfd;

	listenfd=socket(PF_INET,SOCK_STREAM,0);
	if(listenfd<0){
		printf("socket error\n");
		return -1;
	}

	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	servaddr.sin_port=htons(port);
	if(bind(listenfd,(struct sockaddr *)&servaddr,sizeof(servaddr))==-1){
		printf("bind error\n");
		return -1;
	}

	if(listen(listenfd,20)==-1){
		printf("listen error\n");
		return -1;
	}

	for(;;){
		clilen=sizeof(cliaddr);
		connfd=accept(listenfd,(struct sockaddr *)&cliaddr,&clilen);
		if(connfd<0){
			printf("accept error\n");
			return -1;
		}
		str_echo(connfd);
		if(close(connfd)<0){
			printf("close error\n");
			return -1;
		}
	}
}

/*#include "unp.h"
void str_echo(int sockfd){
	ssize_t n;
	char buf[MAXLINE];
	again:
		while((n=read(sockfd,buf,MAXLINE))>0)
			Writen(sockfd,buf,n);
		if(n<0&&errno==EINTR)
			goto again;
		else if(n<0)
			err_sys("str_echo:read error");
}
int main(int argc,char **argv){
	int listenfd,connfd;
	socklen_t clilen;
	struct sockaddr_in cliaddr, servaddr;
	listenfd=Socket(AF_INET,SOCK_STREAM,0);
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	servaddr.sin_port=htons(SERV_PORT);
	Bind(listenfd,(SA*)&servaddr,sizeof(servaddr));
	Listen(listenfd,LISTENQ);
	for(;;){
		clilen=sizeof(cliaddr);
		connfd=Accept(listenfd,(SA*)&cliaddr,&clilen);
		str_echo(connfd);
		Close(connfd);
	}
}*/
