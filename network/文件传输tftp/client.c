#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#include <fcntl.h>

#define N  128
void process_help()
{

		printf("help : display all command \n");
		printf("list : display server dir  \n");
		printf("get  : get <file> \n");
		printf("put  : put <file> \n");
		printf("quit : quit \n");
		
}
void process_list(struct sockaddr_in * addr,char *buf)
{
	int sockefd;
	if ( (sockefd =socket(AF_INET,SOCK_STREAM,0) ) < 0)
	{
		perror("socket");
		exit(-1);
	}
	printf("sockefd = %d\n",sockefd);
	if(connect(sockefd,(struct sockaddr *)addr,sizeof(struct sockaddr_in)) < 0)
	{
		perror("connect");
		exit(-1);
	}
	if( send(sockefd,buf,N,0) < 0 )
	{
		perror("send");
	}
	while(recv(sockefd,buf,N,0) > 0)
	{
		printf("%s\n",buf);
	}
	close(sockefd);
	
	
}
void process_get(struct sockaddr_in *addr,char * buf)
{
	
	int sockefd;
	int fd,nbytes;
	if ( (sockefd =socket(AF_INET,SOCK_STREAM,0) ) < 0)
	{
		perror("socket");
		exit(-1);
	}
	printf("sockefd = %d\n",sockefd);
	if(connect(sockefd,(struct sockaddr *)addr,sizeof(struct sockaddr_in)) < 0)
	{
		perror("connect");
		exit(-1);
	}
	if( send(sockefd,buf,N,0) < 0 )
	{
		perror("send");
	}
	if ( (fd = open(buf+4,O_WRONLY|O_CREAT,0666)) < 0)
	{
		perror("open");
	}

	while(  (nbytes=recv(sockefd,buf,N,0))  > 0)
	{
		write(fd,buf,nbytes);
	}
	close(sockefd);
}
void process_put(struct sockaddr_in *addr,char *buf)
{
	int fd,nbytes;	
	int sockefd;
	if ( (sockefd =socket(AF_INET,SOCK_STREAM,0) ) < 0)
	{
		perror("socket");
		exit(-1);
	}
	printf("sockfd = %d\n",sockefd);
	if(connect(sockefd,(struct sockaddr *)addr,sizeof(struct sockaddr_in)) < 0)
	{
		perror("connect");
		exit(-1);
	}
	if( send(sockefd,buf,N,0) < 0 )
	{
		perror("send");
	}
	if ( (fd = open(buf+4,O_RDONLY)) < 0)
	{
		perror("open");
	}
	while( ( nbytes= read(fd,buf,N ) ) >0 )
	{
		send(sockefd,buf,nbytes,0);
	//	write(newsocketfd,buf,N);
	}
	close(fd);
	close(sockefd);
}

int main(int argc,char *argv[])
{
	char buf[N];
	struct sockaddr_in server_addr;
//	bzero(&server_addr,sizeof(server_addr));
	memset(&server_addr,0,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[2]));
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);

	while(1)
	{
		printf("<client>\n");
		fgets(buf,N,stdin);
		buf[strlen(buf) -1] ='\0';  //qudiao \n
		if(strncmp(buf,"help",4) ==0)
		{
			process_help();
		}
		else if(strncmp(buf,"list",4) ==0 )
		{
			process_list(& server_addr,buf);
		}
		else if(strncmp(buf,"get",3) == 0 ) 
		{
			process_get(&server_addr,buf);
		}
		else if(strncmp(buf,"put",3) == 0)
		{
			process_put(&server_addr,buf);
		}
		else if(strncmp(buf,"quit",4) == 0)
		{
			printf("bye\n");
			break;
		}
		
	}
	return 0;
}





