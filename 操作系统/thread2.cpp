#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <pthread.h>
#include <unistd.h>
using namespace std;
int sum=100;
int semid;
union semnum
{
    int val;
};
union semnum mynum;
sembuf mybuf;

void P(int semid, unsigned short num) //P操作函数
{
    mybuf.sem_num = num; //第一个信号灯,(信号灯编号)
    mybuf.sem_op = -1; //进行P操作, 为 1 时表示V操作
    mybuf.sem_flg = 0; //阻塞
    semop(semid, &mybuf, 1); //最后一个参数，表示操作信号灯的个数
}

//参数 信号灯集id 和 是哪个信号灯
void V(int semid, unsigned short num) //V操作函数
{
    mybuf.sem_num = num;
    mybuf.sem_op = 1;  //1 表示V 操作
    mybuf.sem_flg = 0; //阻塞
    semop(semid, &mybuf, 1);  //操作的 mybuf 全部变量信号灯集
}

void* thread1(void* _val);
void* thread2(void* _val);
int main()
{
    semid = semget(IPC_PRIVATE, 2, IPC_CREAT|IPC_EXCL|0666); //创建信号灯,IPC_EXCL 问信号灯存不存在
    if(semid<0)
    {
        printf("create semaphore error!\n");
        return 0;
    }
    pthread_t t1, t2;
    mynum.val = 1;  //设置信号灯值
    semctl(semid,0,SETVAL,mynum); //初始化 0 号信号灯，此处使用共用体设置信号灯值，初始化完成后可以继续给下一个信号灯设置值

    int* p = (int*)malloc(sizeof(int));
    *p = 1;

    int* p1 = (int*)malloc(sizeof(int));
    *p1 = 1;
    pthread_create(&t1, NULL, thread2, p1);
    pthread_create(&t2, NULL, thread1, p);

    pthread_join(t1,NULL);
    pthread_join(t2,NULL);
    semctl(semid,0,IPC_RMID); //删除信号灯
    semctl(semid,1,IPC_RMID); //删除信号灯
    return 0;
}
void* thread1(void* _val)
{
    while(sum>=0)
    {
        P(semid,0);
        if(sum<=0) break;
        sum--;
        printf("售票窗口1售票成功，剩余票数%d\n",sum);
        V(semid,0);
    }
}

void* thread2(void* _val)
{
    while(sum>0)
    {
        P(semid,0);
        if(sum<=0) break;
        sum--;
        printf("售票窗口2售票成功，剩余票数%d\n",sum);
        V(semid,0);
    }
}