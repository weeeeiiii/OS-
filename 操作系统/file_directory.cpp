//
// Created by 蒋为 on 2019/12/12.
//

#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
void printstat(struct stat s)
{
    if(S_ISDIR(s.st_mode)) putchar('d'); else printf(" ");          //如果是文件夹打输出d，不是则输出空格
    if(S_IRUSR&s.st_mode) putchar('r'); else putchar('-');              //所有者权限
    if(S_IWUSR&s.st_mode) putchar('w'); else putchar('-');
    if(S_IXUSR&s.st_mode) putchar('x'); else putchar('-');
    if(S_IRGRP&s.st_mode) putchar('r'); else putchar('-');              //组用户权限，一组中除所有者拥有的权限
    if(S_IWGRP&s.st_mode) putchar('w'); else putchar('-');
    if(S_IXGRP&s.st_mode) putchar('x'); else putchar('-');
    if(S_IROTH&s.st_mode) putchar('r'); else putchar('-');              //其它用户（组）权限
    if(S_IWOTH&s.st_mode) putchar('w'); else putchar('-');
    if(S_IXOTH&s.st_mode) putchar('x'); else putchar('-');
    putchar(' ');
    printf("%-2d ",s.st_nlink);                                         //对于普通文件是链接数，对于目录文件是第一级子目录数

    struct passwd* pwd;
    pwd = getpwuid(s.st_uid);

    printf("%-6s",pwd->pw_name);                                          //用户名

    struct group* grp;
    grp = getgrgid(s.st_gid);
    printf("%-10s",grp->gr_name);                                        //组名

    printf("%-10ld",s.st_size);                                           //文件大小

    time_t t = s.st_mtime;
    struct tm time;
    localtime_r(&t,&time);
    printf("%-5d年",time.tm_year+1900);                                  //时间
    printf("%-2d月",time.tm_mon+1);
    printf("%-2d日 ",time.tm_mday);
}
void printdir(char *dir,int depth)
{
    DIR* dp;
    dirent* entry;   //当前目录
    struct stat* statbuf=(struct stat*)malloc(sizeof(struct stat));   //存放文件信息
    if((dp=opendir(dir))==NULL)
    {
        printf("打开%s文件失败!\n",dir);
        return;
    }
    chdir(dir);         //进入当前这个目录
    while((entry=readdir(dp))!=NULL)     //读取一个目录项成功
    {
        lstat(entry->d_name,statbuf);       //用目录项名字读取目录信息
        if(S_ISDIR(statbuf->st_mode))
        {
            if(strcmp(entry->d_name,"..")==0||strcmp(entry->d_name,".")==0) //目录为..或.
            {
                continue;
            }
            printf("depth:%3d  ",depth);
            for(int i=0;i<depth;i++)
            {
                printf(".");
            }
//            打印目录项的深度、目录名等信息
//            递归调用printdir,打印子目录的信息,其中的depth+4;
            printstat(*statbuf);
            printf("%s ",entry->d_name);
            printf("\n");
            printdir(entry->d_name,depth+4);
        }
        else     //不是目录而是文件
        {
            printf("depth:%3d  ",depth);
            for(int i=0;i<depth;i++)
            {
                printf(".");
            }
//            打印文件的深度、文件名等信息
            printstat(*statbuf);
            printf("%s\n",entry->d_name);
        }
    }
    chdir("../");      //返回父目录
    closedir(dp);
}

int main()
{
    char *dir;
    printf("请输入目录：");
    scanf("%s",dir);
    printf("%s\n",dir);
    printdir(dir,0);
    return 0;
}

// /Users/wei/CLionProjects/wei
// /Users/wei/Desktop/CPU设计实验
// /Users/wei/Desktop/计网实验2