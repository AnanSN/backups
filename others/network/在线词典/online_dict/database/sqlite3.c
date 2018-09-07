#include <stdio.h>
#include <sqlite3.h>
//注意：宏SQLITE_OK即为0值。

int callback(void *para, int f_num, char **f_value, char **f_name);

int main(int argc, const char *argv[])
{
	sqlite3 *db;
	char *sql = "select * from usr;";
	char *errmsg;
	int nrow, ncolumn, i, j, index;
	char **resultp;

	if(SQLITE_OK != sqlite3_open("my.db", &db))
	{
		printf("error:%s\n",sqlite3_errmsg(db));
		return -1;
	}

	//注意：第四个参数为void*型的，是用于传递给callback回调函数的参数，在callback中使用时要进行强转。
	if(SQLITE_OK != sqlite3_exec(db, sql, callback, NULL, &errmsg))
	{
		printf("error:%s\n",errmsg);
		return -1;
	}

	printf("******************************\n");

	//注意：该函数的第三个参数为一个三级指针，且该参数必须传入，不能为NULL；
	//nrow：记录数即行数；ncolumn：字段数即列数
	//resultp：用来指向sql执行结果的指针
	if(SQLITE_OK != sqlite3_get_table(db, sql, &resultp, &nrow, &ncolumn, &errmsg))
	{
		printf("error:%s\n",errmsg);
		return -1;
	}
	printf("nrow=%d ncolumn=%d\n",nrow, ncolumn);

	index = ncolumn;//第一条记录的第一个字段的下标。

	//printf("%s %s\n",resultp[0], resultp[1]);
	for(i = 0; i < nrow; i ++)
	{
		for(j = 0; j < ncolumn; j ++)
			printf("%s:%s ",resultp[j], resultp[index ++]);
		printf("\n");
	}

	return 0;
}

//注意：回调函数的参数是由系统自动给赋值的；
//f_num：字段数即列数；f_value：包含每个字段值的指针数组；f_name：包含每个字段名称的指针数组
int callback(void *para, int f_num, char **f_value, char **f_name)
{
	int i;

	//printf("f_num=%d\n",f_num);
	for(i = 0; i < f_num; i ++)
		printf("%s:%s ",f_name[i], f_value[i]);
	printf("\n");

	return 0;
}

