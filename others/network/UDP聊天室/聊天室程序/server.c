#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define N 128

#define L 1
#define B 2
#define Q 3

typedef struct node {
	struct sockaddr_in addr;
	struct node *next;
}listnode, *linklist;

typedef struct {
	int type;
	char name[N];
	char text[N];
}MSG;

linklist linklist_create(void);
void process_login(int sockfd, linklist H, MSG *msg, struct sockaddr_in *client_addr);
void process_chat(int sockfd, linklist H, MSG *msg, struct sockaddr_in *client_addr);
void process_logout(int sockfd, linklist H, MSG *msg, struct sockaddr_in *client_addr);
void process_child(int sockfd, MSG *msg, struct sockaddr_in *server_addr);

int main(int argc, const char *argv[])
{
	int sockfd;
	pid_t pid;
	struct sockaddr_in server_addr, client_addr;
	socklen_t len = sizeof(client_addr);
	MSG msg;
	memset(&msg, 0, sizeof(MSG));

	if(-1 == (sockfd = socket(AF_INET, SOCK_DGRAM, 0)))
	{
		perror("socket");
		return -1;
	}

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[2]));
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);
	if(-1 == bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)))
	{
		perror("bind");
		return -1;
	}

	if(-1 == (pid = fork()))
	{
		perror("fork");
		return -1;
	}

	if(pid == 0)//child sendto system message
	{
		process_child(sockfd, &msg, &server_addr);
	}

	if(pid > 0)//father recvfrom
	{
		linklist H = linklist_create();
		
		memset(&client_addr, 0, sizeof(client_addr));
		while(1)
		{
			if(-1 == recvfrom(sockfd, &msg, sizeof(MSG), 0, (struct sockaddr *)&client_addr, &len))
			{
				perror("recvfrom");
				exit(-1);
			}

			switch(msg.type)
			{
				case L:
					process_login(sockfd, H, &msg, &client_addr);
					break;
				case B:
					process_chat(sockfd, H, &msg, &client_addr);
					break;
				case Q:
					process_logout(sockfd, H, &msg, &client_addr);
					break;
				default:
					break;
			}
		}	
	}

	return 0;
}

linklist linklist_create(void)
{
	linklist H = (linklist)malloc(sizeof(listnode));
	H->next = NULL;

	return H;
}

void process_login(int sockfd, linklist H, MSG *msg, struct sockaddr_in *client_addr)
{
	sprintf(msg->text, "%s login",msg->name);
	linklist p = H->next, 
             new = (linklist)malloc(sizeof(listnode));
	new->addr = *client_addr;

	while(NULL != p){
		if(-1 == sendto(sockfd, msg, sizeof(MSG), 0, (struct sockaddr *)&p->addr, sizeof(struct sockaddr_in)))
		{
			perror("sendto");
			exit(-1);
		}
		p = p->next;
	}

	new->next = H->next;
	H->next = new;
}

void process_chat(int sockfd, linklist H, MSG *msg, struct sockaddr_in *client_addr)
{
	char buf[N];
	sprintf(buf, "%s said %s",msg->name, msg->text);
	strcpy(msg->text, buf);

	linklist p = H->next;

	while(NULL != p){
		if(0 != memcmp(client_addr, &p->addr, sizeof(*client_addr)))
		{
			if(-1 == sendto(sockfd, msg, sizeof(MSG), 0, (struct sockaddr *)&p->addr, sizeof(struct sockaddr_in)))
			{
				perror("sendto");
				exit(-1);
			}
		}
		p = p->next;
	}
}

void process_logout(int sockfd, linklist H, MSG *msg, struct sockaddr_in *client_addr)
{
	sprintf(msg->text, "%s logout",msg->name);

	linklist p = H,
			 tmp = NULL;

	while(NULL != p->next){
		if(0 != memcmp(client_addr, &p->next->addr, sizeof(*client_addr)))
		{
			if(-1 == sendto(sockfd, msg, sizeof(MSG), 0, (struct sockaddr *)&p->next->addr, sizeof(struct sockaddr_in)))
			{
				perror("sendto");
				exit(-1);
			}
			p = p->next;
		}
		else
		{
			tmp = p->next;
			p->next = tmp->next;
			free(tmp);
			tmp = NULL;
		}		
	}
}

void process_child(int sockfd, MSG *msg, struct sockaddr_in *server_addr)
{
	memset(msg, 0, sizeof(MSG));
	msg->type = B;
	strncpy(msg->name, "server", 6);

	while(1)
	{
		fgets(msg->text, N, stdin);
		msg->text[strlen(msg->text)-1] = '\0';

		if(-1 == sendto(sockfd, msg, sizeof(MSG), 0, (struct sockaddr *)server_addr, sizeof(*server_addr)))
		{
			perror("sendto");
			exit(-1);
		}
	}
}
