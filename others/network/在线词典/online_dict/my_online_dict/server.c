#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sqlite3.h>
#include <time.h>

#define N 512

#define DATABASE "my.db"

typedef struct {
	char type;//消息类型
	char name[16];//用户名
	char text[N];//用于存放密码/单词/解释/提示信息
}MSG;

void do_register(int connectfd, MSG *msg, sqlite3 *db);
void do_login(int connectfd, MSG *msg, sqlite3 *db);
void do_query(int connectfd, MSG *msg, sqlite3 *db);
void do_history(int connectfd, MSG *msg, sqlite3 *db);
char *get_date();
void search_word(char *word);
int callback(void *para, int f_num, char **f_value, char **f_name);

int main(int argc, const char *argv[])
{
	int listenfd, connectfd;
	pid_t pid;
	struct sockaddr_in server_addr, client_addr;
	sqlite3 * db;
	socklen_t len = sizeof(client_addr);
	signal(SIGCHLD, SIG_IGN);//用于避免僵尸进程

	if(3 != argc)
	{
		printf("usage:%s ip port\n",argv[0]);
		return -1;
	}
	
	//最好是在父进程打开数据库，这样只需打开一次；
	//若在子进程中打开，则数据库会被打开多次，即浪费CPU时间，又浪费资源(会有多个数据库的文件描述符)
	//一个进程最多可以打开1024个文件，对应文件描述符0～1023.
	if(SQLITE_OK != sqlite3_open(DATABASE, &db))
	{
		printf("error:%s\n",sqlite3_errmsg(db));
		return -1;
	}

	if(-1 == (listenfd = socket(AF_INET, SOCK_STREAM, 0)))
	{
		perror("socket");
		return -1;
	}
	printf("listenfd=%d\n",listenfd);

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[2]));
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);
	if(-1 == bind(listenfd, (struct sockaddr *)&server_addr, sizeof(server_addr)))
	{
		perror("bind");
		return -1;
	}

	if(-1 == listen(listenfd, 5))
	{
		perror("listen");
		return -1;
	}

	while(1)
	{
		memset(&client_addr, 0, sizeof(client_addr));
		if(-1 == (connectfd = accept(listenfd, (struct sockaddr *)&client_addr, &len)))
		{
			perror("accept");
			return -1;
		}
		//printf("from client ip:%s port:%d\n",inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
		printf("connectfd=%d\n",connectfd);

		if(-1 == (pid = fork()))
		{
			perror("fork");
			return -1;
		}

		if(pid == 0)//子进程处理客户端的请求
		{
			close(listenfd);
			MSG msg;
			ssize_t n;
			memset(&msg, 0, sizeof(MSG));

			while(1)
			{
				if(-1 == (n = recv(connectfd, &msg, sizeof(MSG), 0)))
				{
					perror("recv");
					return -1;
				}

				if(n == 0)//客户端退出
				{
					close(connectfd);
					printf("client logout\n");
					exit(0);
				}

				printf("type:%c name:%s text:%s\n",msg.type, msg.name, msg.text);

				switch(msg.type)
				{
					case 'R':
						do_register(connectfd, &msg, db);
						break;
					case 'L':
						do_login(connectfd, &msg, db);
						break;
					case 'Q':
						do_query(connectfd, &msg, db);
						break;
					case 'H':
						do_history(connectfd, &msg, db);
						break;
					default:
						break;
				}//end switch
			}
		}//end child
		close(connectfd);
	}//end father

	return 0;
}

void do_register(int connectfd, MSG *msg, sqlite3 *db)
{
	char sql[N] = {0}, *errmsg;
	sprintf(sql, "insert into usr values('%s','%s');",msg->name, msg->text);
	printf("%s\n",sql);

	memset(msg->text, 0, N);
	if(SQLITE_OK != sqlite3_exec(db, sql, NULL, NULL, &errmsg))
	{
		printf("error:%s\n",errmsg);
		sprintf(msg->text, "%s already exist!!!",msg->name);
	}
	else
		sprintf(msg->text, "OK");

	if(-1 == send(connectfd, msg, sizeof(MSG), 0))
	{
		perror("send");
		exit(-1);
	}
}

void do_login(int connectfd, MSG *msg, sqlite3 *db)
{
	char *errmsg, **resultp;
	int nrow;
	char sql[N] = {0};
	sprintf(sql, "select * from usr where name='%s' and password='%s';",msg->name, msg->text);

	//在一些网页上涉及登录功能时，都是会判断用户是否存在的，我们就常常利用sqlite3_get_table函数来进行判断；
	//其中的参数nrow即为满足条件的记录的数目，若此用户不存在，则nrow=0。
	if(SQLITE_OK != sqlite3_get_table(db, sql, &resultp, &nrow, NULL, &errmsg))
	{
		printf("error:%s\n",errmsg);
		exit(-1);
	}

	if(nrow == 0)
		sprintf(msg->text, "WRONG");
	else
		sprintf(msg->text, "OK");

	if(-1 == send(connectfd, msg, sizeof(MSG), 0))
	{
		perror("send");
		exit(-1);
	}
}

//查单词函数
//注意：函数传参数时：传一级指针，可以改变指针的指向的内容；
//而传二级指针，则是可以改变指针的指向。
void search_word(char *word)
{
	FILE *fp;
	char buf[N] = {0}, *p;
	int ret, word_len;

	fp = fopen("dict.txt", "r");
	word_len = strlen(word);

	while(NULL != fgets(buf, N, fp))
	{
		ret = strncmp(buf, word, word_len);

		if(ret < 0)//没有找到，继续
			continue;

		if(ret > 0 || buf[word_len] != ' ')//没有找到，退出
		{
			word = NULL;
			break;
		}
		
		//找到了
		p = buf + word_len;
		while(*p == ' ')
			p ++;
		strcpy(word, p);
		break;
	}

	fclose(fp);
}

//得到系统时间，通过指针函数将时间返回；
//指针函数需要特别注意的是：局部变量的地址不可以返回。
char *get_date()
{
	time_t s;
	struct tm *t;
	static char date[32] = {0};

	time(&s);
	t = localtime(&s);
	strftime(date, N, "%Y-%m-%d %H:%M:%S", t);

	return date;
}

void do_query(int connectfd, MSG *msg, sqlite3 *db)
{
	char sql[N] = {0}, word[16] = {0}, *errmsg;
	strcpy(word, msg->text);

	search_word(msg->text);
	if(msg->text != NULL)
	{
		//插入历史记录
		sprintf(sql, "insert into record values('%s','%s','%s');",msg->name, get_date(), word);
		printf("%s\n",sql);
		if(SQLITE_OK != sqlite3_exec(db, sql, NULL, NULL, &errmsg))
		{
			printf("error:%s\n",errmsg);
			exit(-1);
		}
		printf("insert record success!\n");
	}
	else
		sprintf(msg->text, "%s is not found.\n",msg->name);

	if(-1 == send(connectfd, msg, sizeof(MSG), 0))
	{
		perror("send");
		exit(-1);
	}
}

int callback(void *para, int f_num, char **f_value, char **f_name)
{
	MSG msg;
	memset(&msg, 0, sizeof(MSG));
	strcpy(msg.name, f_value[0]);
	sprintf(msg.text, "%s : %s",f_value[1], f_value[2]);

	if(-1 == send((*((int *)para)), &msg, sizeof(MSG), 0))
	{
		perror("send");
		exit(-1);
	}

	return 0;
}

void do_history(int connectfd, MSG *msg, sqlite3 *db)
{
	char sql[N] = {0}, *errmsg;
	sprintf(sql, "select * from record where name='%s';",msg->name);

	if(SQLITE_OK != sqlite3_exec(db, sql, callback, &connectfd, &errmsg))
	{
		printf("error:%s\n",errmsg);
		exit(-1);
	}
	printf("send history over\n");

	if(-1 == send(connectfd, msg, sizeof(MSG), 0))
	{
		perror("send");
		exit(-1);
	}
}

