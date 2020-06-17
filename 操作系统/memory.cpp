#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sem.h>
#include <unistd.h>
#define number 100          //缓冲区的个数
#define size 1024
struct shared_memory{
    char text[number][size];          //一共100个共享内存
};
struct inoutflag{
    int in,out,length,flag;
};

int semid;      //两个信号，信号0表示空闲缓冲区或者已经被读过的缓冲区个数，信号1表示可用于读取的数据个数
void* buf;
shared_memory *shared;
void* inouttemp;
inoutflag *rw;
int read_proc,write_proc;
void P(int semid,int index);
void V(int semid,int index);

int main() {
    int shm = shmget(1, sizeof(shared_memory), IPC_CREAT | 0666);
    if (shm == -1) {
        printf("共享内存1创建失败！\n");
        exit(0);
    }
    buf=shmat(shm,NULL,SHM_R|SHM_W);
    shared=(shared_memory*)buf;

    int shm2=shmget(2,sizeof(inoutflag),IPC_CREAT|0666);
    if(shm2==-1)
    {
        printf("共享内存2创建失败！\n");
        exit(0);
    }
    inouttemp=shmat(shm2,NULL,SHM_R|SHM_W);
    rw=(inoutflag*)inouttemp;
    rw->in=0;
    rw->out=0;
    rw->length=0;
    rw->flag=0;

    semid = semget(3,2,IPC_CREAT|0666);    //创建信号灯集
    semctl(semid,0,SETVAL,100);      //0号灯表示空闲缓冲区个数，当前为number
    semctl(semid,1,SETVAL,0);      //1号灯表示可读取的缓冲区个数，当前为0

    if((read_proc=fork())==0)
    {
        execv("./read",NULL);
    }
    else
    {
        if((write_proc=fork())==0)
        {
            execv("./write",NULL);
        }
        wait(0);
        semctl(semid,0,IPC_RMID);
        shmctl(shm,IPC_RMID,0);
        shmctl(shm2,IPC_RMID,0);
    }
    return 0;
}
void P(int semid,int index)
{
    struct sembuf sem;
    sem.sem_num = index;
    sem.sem_op = -1;
    sem.sem_flg = 0;
    semop(semid,&sem,1);
}

void V(int semid,int index)
{
    struct sembuf sem;
    sem.sem_num = index;
    sem.sem_op = 1;
    sem.sem_flg = 0;
    semop(semid,&sem,1);
}