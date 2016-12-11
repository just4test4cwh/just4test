#include "unp.h"
#include <pthread.h>

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

void * copyto(void *arg)
{
	int size;
	struct CopytoArg *carg = (struct CopytoArg*)arg;
	char sbuffer[2048];

	while (1)
	{
		size = Read(carg->infd, sbuffer, sizeof(sbuffer));
		if (size == 0) {
			Shutdown(carg->sockfd, SHUT_WR);
			return NULL;	/* EOF */
		}

		Write(carg->sockfd, sbuffer, size);
	}
}
void str_cli3(int infd, int outfd, int sockfd)
{
	size_t size;
	pthread_t tid;
	struct CopytoArg arg;
	char rbuffer[2048];

	arg.infd = infd;
	arg.sockfd = sockfd;
	pthread_create(&tid, NULL, copyto, (void*)&arg);
	
	while (1)
	{
		size = Read(sockfd, rbuffer, sizeof(rbuffer));
		if (size == 0)
			return;

		Write(outfd, rbuffer, size);
	}
}

int main(int argc, char **argv){
	int sockfd;
	struct sockaddr_in servaddr;
	
	if(argc!=2)
		err_quit("usage:tcpcli<IPaddress>");
	
	sockfd=Socket(AF_INET,SOCK_STREAM,0);
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(SERV_PORT);
	Inet_pton(AF_INET,argv[1],&servaddr.sin_addr);
	Connect(sockfd,(SA*)&servaddr,sizeof(servaddr));
	str_cli(stdin,sockfd);
	exit(0);
}

