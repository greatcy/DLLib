/*************************************************************************
	> File Name: select.c
	> Author: eli
	> Mail: 1525843015@qq.com
	> Created Time: 2018年01月22日 星期一 20时21分58秒
 ************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>

#define BUF_SIZE 30

int main(int argc,char **argv){
    fd_set reads,temps;

    int result,str_len;

    char buf[BUF_SIZE];
    struct timeval timeout;

    FD_ZERO(&reads);
    FD_SET(0,&reads);


    //timeout.tv_sec=5;
    //timeout.tv_usec=5000;

    while(1){
        temps=reads;
        timeout.tv_sec=5;
        timeout.tv_usec=0;
        result=select(1,&temps,0,0,&timeout);
        if(result==-1){
            puts("select error!\n");
            break;
        }
        else if(result==0){
            puts("Time-out!");
        }
        else{
            if(FD_ISSET(0,&temps)){//isset return 1 if read temps change 
                str_len=read(0,buf,BUF_SIZE);
                buf[str_len]=0;
                printf("message from console:%s",buf);
            }
        }
    }
    return 0;
}
