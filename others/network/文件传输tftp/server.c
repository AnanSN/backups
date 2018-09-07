#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>


#define N 128
void process_list(int newsocketfd ,char *buf )
{
	DIR * mydir = NULL;
	struct dirent * mydirent = NULL;
	mydir = opendir(".");
	while(  (mydirent = readdir(mydir))  != NULL)
	{
		strcpy(buf,mydirent->d_name);
		if(strncmp(buf,".",1) == 0  )  continue;
		send(newsocketfd,buf,N,0);
	}
	closedir(mydir);
	close(newsocketfd);


}
void process_get(int newsocketfd,char * buf)
{
	int fd,nbytes;	
	if ( (fd = open(buf+4,O_RDONLY)) < 0)
	{
		perror("open");
	}
	while( ( nbytes= read(fd,buf,N ) ) >0 )
	{
		send(newsocketfd,buf,nbytes,0);
	//	write(newsocketfd,buf,N);
	}
	close(fd);
	close(newsocketfd);
}
void process_put(int newsocketfd,char * buf)
{
	int fd,nbytes;	
	if ( (fd = open(buf+4,O_WRONLY|O_CREAT,0666)) < 0)
	{
		perror("open");
	}
	while( ( nbytes= recv(newsocketfd,buf,N,0 ) ) >0 )
	{
		write(fd,buf,nbytes);
	//	write(newsocketfd,buf,N);
	}
	close(fd);
	close(newsocketfd);
}
int main(int argc,char *argv[])
{

	char buf[N];
	int listenfd,newsocketfd;
	struct sockaddr_in server_addr;
//	bzero(&server_addr,sizeof(server_addr));
	memset(&server_addr,0,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[2]));
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);
	if ( (listenfd =socket(AF_INET,SOCK_STREAM,0)) < 0)
	{
		perror("socket");
		exit(-1);
	}
	if(bind(listenfd,(struct sockaddr *)&server_addr, sizeof(server_addr) )  < 0)
	{
		perror("bind");
		exit(-1);
	}
	listen(listenfd,10);
	while(1)
	{
		if( (newsocketfd= accept(listenfd,NULL,NULL))  < 0)
		{
			perror("accept");
		}
		if( recv(newsocketfd,buf,N,0) < 0)
		{
			perror("recv");
		}
		if(strncmp(buf,"list",4) == 0)
		{
			process_list(newsocketfd,buf);
		}
		else if(strncmp(buf,"get",3) == 0)
		{
			process_get(newsocketfd,buf);
		}
		else if(strncmp(buf,"put",3) ==0 ) 
		{
			process_put(newsocketfd,buf);
		}
		
	}
	return 0;
}





