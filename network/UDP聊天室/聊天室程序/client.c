#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define N 128

#define L 1
#define B 2
#define Q 3

typedef struct {
	int type;
	char name[N];
	char text[N];
}MSG;

int main(int argc, const char *argv[])
{
	int sockfd;
	pid_t pid;
	struct sockaddr_in server_addr;
	MSG msg;
	signal(SIGCHLD, SIG_IGN);

	if(-1 == (sockfd = socket(AF_INET, SOCK_DGRAM, 0)))
	{
		perror("socket");
		return -1;
	}

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[2]));
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);

	printf("please input your name:");
	memset(&msg, 0,sizeof(MSG));
	msg.type = L;
	fgets(msg.name, N, stdin);
	msg.name[strlen(msg.name)-1] = '\0';
	if(-1 == sendto(sockfd, &msg, sizeof(MSG), 0, (struct sockaddr *)&server_addr, sizeof(server_addr)))
	{
		perror("sendto");
		return -1;
	}

	if(-1 == (pid = fork()))
	{
		perror("fork");
		return -1;
	}

	if(pid == 0)//child sendto
	{
		printf(">");
		while(1)
		{
			printf(">");
			fgets(msg.text, N, stdin);
			msg.text[strlen(msg.text)-1] = '\0';

			if(0 == strncmp(msg.text, "quit", 4))
			{
				msg.type = Q;
				if(-1 == sendto(sockfd, &msg, sizeof(MSG), 0, (struct sockaddr *)&server_addr, sizeof(server_addr)))
				{
					perror("sendto");
					exit(-1);
				}
				kill(getppid(), SIGKILL);
				exit(0);
			}
			else
			{
				msg.type = B;
				if(-1 == sendto(sockfd, &msg, sizeof(MSG), 0, (struct sockaddr *)&server_addr, sizeof(server_addr)))
				{
					perror("sendto");
					exit(-1);
				}
			}		
		}
	}

	if(pid > 0)//father recvfrom
	{
		while(1)
		{
			if(-1 ==  recvfrom(sockfd, &msg, sizeof(MSG), 0, NULL, NULL))
			{
				perror("recvfrom");
				exit(-1);
			}
			printf("%s\n",msg.text);
		}
	}

	return 0;
}
