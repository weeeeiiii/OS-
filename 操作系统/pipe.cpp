#include<stdio.h>
#include<signal.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<string.h>
#include<unistd.h>

int w;
void waiting()
{
    while(w!=0);
}
void stop1(int sig)
{
    printf("Child process 1 is killed by parent!\n");
    exit(0);
}
void stop2(int sig)
{
    printf("Child process 2 is killed by parent!\n");
    exit(0);
}
void stop(int sig)
{
    w=0;
}

int main()
{
    int p1,p2;
    int pipefd[2];
    if(pipe(pipefd)<0)
    {
        printf("create pipe error!\n");
        exit(0);
    }

    if((p1=fork())==0)           //子进程1
    {
        int count=0;
        while(1)
        {
            signal(SIGUSR1,stop1);signal(2, SIG_IGN);
            char buf[30];
            sprintf(buf,"I send u %d times\n",++count);
            write(pipefd[1],buf, sizeof(buf));
            sleep(1);
        }
    }
    else
    {
        if((p2=fork())==0)     //子进程2
        {
            while(1)
            {
                signal(SIGUSR2,stop2);signal(2, SIG_IGN);
                char buf[30];
                read(pipefd[0],buf, sizeof(buf));
                printf("%s",buf);
            }
        }
        else
        {
            w=1;
            signal(SIGINT,stop);
            waiting();
            kill(p1,SIGUSR1);
            wait(0);
            kill(p2,SIGUSR2);
            wait(0);
            printf("Parent process is killed!\n");/*接收到子进程死信号后，杀死父进程*/
        }

    }
    return 0;
}
