#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <assert.h>

#define MAXLINE 1024
#define LISTENQ     5

int main(){
	struct epoll_event ev,events[20];
	int epfd=epoll_create(256);
	struct sockaddr_in clientaddr;
	struct sockaddr_in serveraddr;
	int listenfd = socket(AF_INET, SOCK_STREAM, 0);
	ev.data.fd=listenfd;
	ev.events=EPOLLIN|EPOLLET;  
	epoll_ctl(epfd,EPOLL_CTL_ADD, listenfd,&ev);
	bzero(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	char *local_addr="127.0.0.1";
    	inet_aton(local_addr,&(serveraddr.sin_addr));
	serveraddr.sin_port=htons(8887);
	bind(listenfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr));
	listen(listenfd, LISTENQ);
	int sockfd;
	int i,n;
	socklen_t clilen;
    	clilen = sizeof(clientaddr);
	char line[MAXLINE] = {0};
	
	for ( ; ; ) {
		int nfds=epoll_wait(epfd,events,20,500);
		for(i=0;i<nfds;++i) {
		    	if(events[i].data.fd==listenfd) {//新的连接 
			    	int connfd = accept(listenfd,(struct sockaddr *)&clientaddr, &clilen);
			     	if(connfd < 0){return 1;  }
				//char *str = inet_ntoa(clientaddr.sin_addr);
				ev.data.fd=connfd;
				ev.events=EPOLLIN|EPOLLET;
				epoll_ctl(epfd,EPOLL_CTL_ADD,connfd,&ev);
			}
			else if(events[i].events&EPOLLIN){
			//如果是已经连接的用户，并且收到数据，那么进行读入。
				
				if ( (sockfd = events[i].data.fd) < 0)
				    continue;
				if ( (n = read(sockfd, line, MAXLINE)) < 0) {
					if (errno == ECONNRESET) {
						close(sockfd);
						events[i].data.fd = -1;
				 	}
				}
				else if (n == 0) {
				    close(sockfd);
				     events[i].data.fd = -1;
				}
				
				printf("read:%s\n",line);                
				ev.data.fd=sockfd;
				ev.events=EPOLLOUT|EPOLLET;
				epoll_ctl(epfd,EPOLL_CTL_MOD,sockfd,&ev);
			}
			else if(events[i].events&EPOLLOUT) // 如果有数据发送
			    {
				sockfd = events[i].data.fd;
				write(sockfd, line, strlen(line) +1);
				ev.data.fd=sockfd;
				ev.events=EPOLLIN|EPOLLET;
				epoll_ctl(epfd,EPOLL_CTL_MOD,sockfd,&ev);
			    }
		}
	}
	return 0;
}
