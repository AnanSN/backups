#include <stdio.h>
#include <string.h>

//这里需要特别注意，N的值一定要大于dict.txt中一行的最大字节数；
//否则，在查词时会出现错误。
#define N 512

int main(int argc, const char *argv[])
{
	FILE *fp;
	char buf[N] = {0};
	char *p;
	int ret, word_len;

	if(2 != argc)
	{
		printf("usage:%s word\n",argv[0]);
		return -1;
	}

	fp = fopen("dict.txt", "r");
	word_len = strlen(argv[1]);

	while(NULL != fgets(buf, N, fp))
	{
		ret = strncmp(buf, argv[1], word_len);

		//注意：我们应该尽量避免使用过多的if……else……的结构，因为那样程序的可读性差；
		//以下的并行结构是比较可取的。
		if(ret < 0)//没找到，继续
			continue;

		if(ret > 0 || buf[word_len] != ' ')//过了，没有
		{
			printf("%s is not found\n", argv[1]);
			break;
		}

		//找到了
		p = buf + word_len;
		while(*p == ' ')
			p ++;
		printf("%s\n",argv[1]);
		printf("%s",p);
		break;
	}//end while

	fclose(fp);

	return 0;
}
