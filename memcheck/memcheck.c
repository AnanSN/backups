#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static char *s;

int func2_1_8(void)
{
    /* definitely lost */
    char *p = malloc(sizeof(char)); // 申请空间没有释放便退出
    printf("nshao\n");
    sleep(5);
    
    /* still reachable */
    //char *p = malloc(sizeof(char));
    //s = p; // p虽然没有free但是任然可以找到，因为是static变量s保存着它

    /* indirectly lost */
    //char *p = malloc(sizeof(char)); // 在p和q都没有释放的前提下，p属于间接丢失
    //long *q = malloc(sizeof(long)); // 因为p值已经存在了堆中，而q则是直接丢失。
    //*q = (long)p;

    /* possibly lost */
    //char *p = malloc(sizeof(char)*2);
    //s = p+1; // p+1仍然在申请的空间内，若出了这个空间则为绝对丢失
    return 0;
}

int func2_1_7(void)
{
    //char *p = malloc(0xfffffffffffffffff); // 64位机器要超过16个f才会报错
    //free(p);

    char *q = malloc(-1); // 负数也会报错
    free(q);
    return 0;
}

int func2_1_6(void)
{
    char *p = malloc(sizeof(char)*3);
    *p = 0;
    //memcpy(p+1, p, 3); // 申请3个字节，将初始化后的内容覆盖到自己位置向后偏移1个字节
    free(p);           // 不仅犯了内存重叠的错，还有非法写入的错。
    return 0;
}

int func2_1_5(void)
{
    /* malloc, calloc, realloc, valloc, memalign --> free */
    return 0;
}

int func2_1_4(void)
{
    char *p = malloc(sizeof(char));
    free(p);
    //free(p); /* 重复释放 */
    return 0;
}

int func2_1_3(void)
{
    char *p = malloc(sizeof(char)*2);
    //write(1, p, 10); /* 与func2_1_1不同的是write是系统调用 */

    //exit(*p); /* write针对的p指针，exit针对地址上的内容 */
    free(p);
    return 0;
}

int func2_1_2(void)
{
    char a[2] = {0};
    char *p = malloc(sizeof(char));
    //strcpy(a, p); /* 新申请的地址没有初始化便复制给a数组 */
    free(p);
    return 0;
}

int func2_1_1(void)
{
    char a[2] = {0};
    char *p = 0;
    //*p = 1; /* 向0地址写入值是非法写入 */
    printf("nshao\n");
    sleep(5);

    //strcpy(a, p); /* 从0地址读值给a数据是非法读取 */
    return 0;
}

int main(int argc, const char *argv[])
{
    func2_1_7();
    while(1) {
        func2_1_8();
    }
    //func2_1_1();
    //func2_1_2();
    //func2_1_3();
    //func2_1_4();
    //func2_1_5();
    //func2_1_6();
    //func2_1_7();
    return 0;
}
