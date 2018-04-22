#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <signal.h>

void func1(int sig)
{
    printf("get your signal %d \n", sig);
    signal(sig, SIG_DFL);
}

int main(int argc, const char *argv[])
{
    int mtime = 1;

    signal(SIGINT, func1);
    
    while(1){
        printf("waiting singal (%d seconds)... \n", mtime);
        sleep(mtime);
    }
    return 0;
}
