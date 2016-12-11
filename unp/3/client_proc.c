#include "unp.h"
#include <unistd.h>
/*void str_cli(FILE *fp, int sockfd){
	char sendline[MAXLINE],recvline[MAXLINE];
	while(Fgets(sendline,MAXLINE,fp)!=NULL){
		Writen(sockfd,sendline,strlen(sendline));
		if(Readline(sockfd,recvline,MAXLINE)==0){
			err_quit("str_cli:server terminated prematurely");
		}
		Fputs(recvline,stdout);
	}
}*/
struct CopytoArg {
	int infd;
	int sockfd;
};


int main(int argc, char **argv){
	int sockfd;
	//int stat;
	char sendline[MAXLINE],recvline[MAXLINE];
	struct sockaddr_in servaddr;
	
	if(argc!=2)
		err_quit("usage:tcpcli<IPaddress>");
	
	sockfd=Socket(AF_INET,SOCK_STREAM,0);
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(SERV_PORT);
	Inet_pton(AF_INET,argv[1],&servaddr.sin_addr);
	Connect(sockfd,(SA*)&servaddr,sizeof(servaddr));
	pid_t pid=fork();
	if(pid<0){
		printf("fork error\n");
	}
	else if(pid==0){
		//child
		str_cli(stdin,sockfd);
	}
	else{
		//parent
		while (1){
			size = Read(sockfd, rbuffer, sizeof(rbuffer));
			if (size == 0)
				return;

			Write(outfd, rbuffer, size);
		}
	}
	//str_cli(stdin,sockfd);
	exit(0);
}

