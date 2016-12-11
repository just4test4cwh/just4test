#include      "unp.h"
#include <pthread.h>

void str_echo2(int sockfd, char* buf){	
	ssize_t n;	
	while (1) {		
		n = Read(sockfd, buf, MAXLINE);		
		if (n > 0)			
			Write(sockfd, buf, n);		
		else			
			return;
	}
}

void* str_echo3(void *arg) {
	int sockfd=(int)arg;
	char buf[MAXLINE];

	pthread_detach(pthread_self());
	str_echo2(sockfd,buf);
	close(sockfd);
	return NULL;
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
		/*clilen = sizeof(cliaddr);
		connfd = Accept(listenfd, (SA *) &cliaddr, &clilen);
		str_echo(connfd);
		Close(connfd);*/
		pthread_t tid;
		clilen=sizeof(cliaddr);
		connfd=Accept(listenfd,(SA *)&cliaddr,&clilen);

		pthread_create(&tid,NULL,str_echo3,(void*)connfd);
	} 
	return 0;
}


