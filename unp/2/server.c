#include      "unp.h"

void str_echo(int sockfd) {
	ssize_t n;
	char    buf[MAXLINE],temp[MAXLINE];
	
	time_t now;
	struct tm  *timenow; 
	int len;
	again:
		while ( (n = read(sockfd, buf, MAXLINE)) > 0){

			time(&now);
			timenow = localtime(&now);
			len=sprintf(temp,"%sserver time:%s",buf,asctime(timenow));
			printf("%s",temp);
			Writen(sockfd, temp, len);
			memset(temp,0,sizeof(temp));
			memset(buf,0,sizeof(buf));
		}
		if (n < 0 && errno == EINTR) 
			goto again; 
		else if (n < 0) 
			err_sys("str_echo: read error"); 
}

void sig_child(int sign){	
	pid_t pid;	
	int stat;	// WHOHANG:若子进程没有结束，waitpid()函数返回0，不予等待。    
	while ((pid = waitpid(-1,&stat,WNOHANG)) > 0) {    	
		printf("child %d terminated.\n", pid);    	
		return;    
	}
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
		int pid;
		if((pid=fork())==-1){
			printf("fork error\n");
		}else if(pid == 0){
			close(listenfd);
			str_echo(connfd);
			exit(EXIT_SUCCESS);
		}else{
			Close(connfd);
			signal(SIGCHLD, sig_child);
		}
	} 
}


