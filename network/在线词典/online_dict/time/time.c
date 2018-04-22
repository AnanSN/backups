#include <stdio.h>
#include <time.h>

#define N 512

int main(int argc, const char *argv[])
{
	time_t s;
	struct tm *t;
	char date[N] = {0};

	time(&s);
	t = localtime(&s);

#if 0
	sprintf(date, "%d-%d-%d %d:%d:%d",t->tm_year+1900, t->tm_mon+1,\
			t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
	printf("%s\n",date);
#else
	strftime(date, N, "%Y-%m-%d %H:%M:%S", t);
	printf("%s\n",date);
#endif

	//注意：ctime返回的字符串是以'\n'结束的。
	printf("%s",ctime(&s));

	return 0;
}


