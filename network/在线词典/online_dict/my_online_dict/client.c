#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define N 512

typedef struct {
	char type;//消息类型
	char name[16];//用户名
	char text[N];//用于存放密码/单词/解释/提示信息
}MSG;

void do_register(int sockfd, MSG *msg);
void do_login(int sockfd, MSG *msg);
void do_query(int sockfd, MSG *msg);
void do_history(int sockfd, MSG *msg);

int main(int argc, const char *argv[])
{
	int sockfd, num;
	struct sockaddr_in server_addr;
	MSG msg;
	memset(&msg, 0, sizeof(MSG));

	if(3 != argc)
	{
		printf("usage:%s ip port\n",argv[0]);
		return -1;
	}
	
	if(-1 == (sockfd = socket(AF_INET, SOCK_STREAM, 0)))
	{
		perror("socket");
		return -1;
	}

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[2]));
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);

	if(-1 == connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)))
	{
		perror("connect");
		return -1;
	}

	while(1)
	{
		printf("*****************************\n");
		printf("* 1:register 2:login 3:quit *\n");
		printf("*****************************\n");

		printf("please select:");
		scanf("%d",&num);

		switch(num)
		{
		case 1:
			do_register(sockfd, &msg);
			break;
		case 2:
			do_login(sockfd, &msg);
			break;
		case 3:
			close(sockfd);
			exit(0);
		default:
			break;
		}//end switch
	}//end while

	return 0;
}

void do_register(int sockfd, MSG *msg)
{
	msg->type = 'R';
	printf("please input your name:");
	scanf("%s",msg->name);
	printf("please input your password:");
	scanf("%s",msg->text);

	if(-1 == send(sockfd, msg, sizeof(MSG), 0))
	{
		perror("send");
		exit(-1);
	}

	if(-1 ==  recv(sockfd, msg, sizeof(MSG), 0))
	{
		perror("recv");
		exit(-1);
	}

	printf("%s\n",msg->text);
}

void do_login(int sockfd, MSG *msg)
{
	msg->type = 'L';
	printf("please input your name:");
	scanf("%s",msg->name);
	printf("please input your password:");
	scanf("%s",msg->text);

	if(-1 == send(sockfd, msg, sizeof(MSG), 0))
	{
		perror("send");
		exit(-1);
	}

	if(-1 ==  recv(sockfd, msg, sizeof(MSG), 0))
	{
		perror("recv");
		exit(-1);
	}

	printf("%s\n",msg->text);

	if(0 == strncmp(msg->text, "OK", 2))
	{
		int num;

		while(1)
		{
			printf("*********************************\n");
			printf("* 1:query_word 2:history 3:quit *\n");
			printf("*********************************\n");

			printf("please select:");
			scanf("%d",&num);

			switch(num)
			{
				case 1:
					do_query(sockfd, msg);
					break;
				case 2:
					do_history(sockfd, msg);
					break;
				case 3:
					exit(0);
				default:
					break;
			}//end switch
		}//end while
	}//login success
}

void do_query(int sockfd, MSG *msg)
{
	printf("you can input # to end the query\n");
	msg->type = 'Q';

	while(1)
	{
		printf("please input the word:");
		scanf("%s",msg->text);

		if(0 == strncmp(msg->text, "#", 1))
			break;

		if(-1 == send(sockfd, msg, sizeof(MSG), 0))
		{
			perror("send");
			exit(-1);
		}

		if(-1 ==  recv(sockfd, msg, sizeof(MSG), 0))
		{
			perror("recv");
			exit(-1);
		}

		printf("%s",msg->text);
	}
}

void do_history(int sockfd, MSG *msg)
{
	msg->type = 'H';
	if(-1 == send(sockfd, msg, sizeof(MSG), 0))
	{
		perror("send");
		exit(-1);
	}

	printf("history:\n");
	while(1)
	{
		if(-1 ==  recv(sockfd, msg, sizeof(MSG), 0))
		{
			perror("recv");
			exit(-1);
		}

		if(msg->type != '\0')
			break;

		printf("%s\n",msg->text);
	}
}

