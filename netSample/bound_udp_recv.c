/*************************************************************************
	> File Name: udp_client.c
	> Author: eli
	> Mail: 1525843015@qq.com
	> Created Time: 2018年01月04日 星期四 14时08分29秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30

int main(int argc,char **argv){
    int sock,i;
    char message[BUF_SIZE];
    int str_len;
    socklen_t adr_sz;
    struct sockaddr_in my_adr,your_adr;
    if(argc!=2){
        printf("Usage: %s <port>\n",argv[0]);
        exit(1);
    }

    sock=socket(PF_INET,SOCK_DGRAM,0);
    if(sock==-1){
        printf("%s:%d error\n",__FILE__,__LINE__);
        return -1;
    }

    memset(&my_adr,0,sizeof(my_adr));
    my_adr.sin_family=AF_INET;
    my_adr.sin_addr.s_addr=htonl(INADDR_ANY);
    my_adr.sin_port=htons(atoi(argv[1]));

    if(bind(sock,(struct sockaddr*)&my_adr,sizeof(my_adr))==-1){
        printf("%s:%d error\n",__FILE__,__LINE__);
        return -1;
    }

    for(i=0;i<3;i++){
        sleep(5);
        adr_sz=sizeof(your_adr);
        str_len=recvfrom(sock,message,BUF_SIZE,0,(struct sockaddr*)&your_adr,&adr_sz);
        printf("message %d:%s \n",i+1,message);
        printf("recv done %d\n",i+1);
    }

    close(sock);
    return 0;
}

