#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <linux/in.h>
#include <errno.h>

#define MAXLINE 1024

static int  read_cnt; 
static char read_buf[MAXLINE];
static char *read_ptr;  

static ssize_t my_read(int fd, char *ptr)    
{    
    if (read_cnt <= 0)     
    {    
again:    
        if ( (read_cnt = read(fd, read_buf, sizeof(read_buf))) < 0)     
        {    
            if (errno == EINTR)    
                goto again;    
            return(-1);    
        }     
        else if (read_cnt == 0)    
            return(0);    
        read_ptr = read_buf;    
    }    
  
    read_cnt--;    
    *ptr = *read_ptr++;    
    return 1;    
}  

ssize_t readline(int fd, void *vptr, size_t maxlen)    
{    
    ssize_t n, rc;    
    char    c, *ptr;    
  
    ptr = vptr;    
    for (n = 1; n < maxlen; n++)    
    {    
        if ( (rc = my_read(fd, &c)) == 1)     
        {    
            *ptr++ = c;    
            if (c == '\n')    
                break;  /* newline is stored, like fgets() */    
        }     
        else if (rc == 0)    
        {    
            *ptr = 0;    
            return(n - 1);  /* EOF, n - 1 bytes were read */    
        }    
        else    
            return(-1);     /* error, errno set by read() */    
    }    
    *ptr = 0;   /* null terminate like fgets() */    
    return n;    
}  

void str_cli(FILE *fp, int sockfd){
	char sendline[MAXLINE],recvline[MAXLINE];
	char *rptr/*=fgets(sendline,MAXLINE,fp)*/;
	/*if ( rptr == NULL && ferror(fp) )    
        	printf("fgets error\n"); */
	while((rptr=fgets(sendline,MAXLINE,fp))!=NULL){
		if ( rptr == NULL && ferror(fp) ){    
        		printf("fgets error\n");
		} 
		if(write(sockfd,sendline,strlen(sendline))<0){
			printf("writen error\n");
		}
		ssize_t n=readline(sockfd,recvline,MAXLINE);
		if(n<0){
			printf("readline error\n");
		}
		if(n==0){
			printf("str_cli:server terminated prematurely");
		}
		if(fputs(recvline,stdout)<0){
			printf("fputs error\n");
		}
	}
}

int main(int argc, char **argv){
	int sockfd;
	struct sockaddr_in servaddr;
	int port=1234;

	if(argc!=2){
		printf("usage:tcpcli<IPaddress>");
		return -1;
	}
	sockfd=socket(PF_INET,SOCK_STREAM,0);
	if(sockfd<0){
		printf("socket error\n");
		return -1;
	}
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(port);
	if(inet_pton(AF_INET,argv[1],&servaddr.sin_addr)<0){
		printf("inet_pton error\n");
		return -1;
	}
	if(connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr))<0){
		printf("connect error\n");
		return -1;
	}
	str_cli(stdin,sockfd);
	exit(0);
}

/*void str_cli(FILE *fp, int sockfd){
	char sendline[MAXLINE],recvline[MAXLINE];
	while(Fgets(sendline,MAXLINE,fp)!=NULL){
		Writen(sockfd,sendline,strlen(sendline));
		if(Readline(sockfd,recvline,MAXLINE)==0){
			err_quit("str_cli:server terminated prematurely");
		}
		Fputs(recvline,stdout);
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
*/
