/*************************************************************************
	> File Name: udp_server.c
	> Author: eli
	> Mail: 1525843015@qq.com
	> Created Time: 2018年01月04日 星期四 14时09分46秒
    > Desc:A UDP server(not suitable) sample
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main(int argc,char *argv[]){
    int sock;
    char msg1[]="HI!";
    char msg2[]="I'm another UDP host!";
    char msg3[]="Nice to meet you";


    socklen_t your_adr_sz;
    struct sockaddr_in your_adr;

    if(argc!=3){
        printf("Usage: %s <IP> <port>\n",argv[0]);
        exit(1);
    }

    sock=socket(PF_INET,SOCK_DGRAM,0);
    if(sock==-1){
        printf("%s:%d error\n",__FILE__,__LINE__);
        return -1;
    }

    memset(&your_adr,0,sizeof(your_adr));
    your_adr.sin_family=AF_INET;
    your_adr.sin_addr.s_addr=inet_addr(argv[1]);
    your_adr.sin_port=htons(atoi(argv[2]));

    sendto(sock,msg1,sizeof(msg1),0,(struct sockaddr *)&your_adr,sizeof(your_adr));
    sendto(sock,msg2,sizeof(msg2),0,(struct sockaddr *)&your_adr,sizeof(your_adr));
    sendto(sock,msg3,sizeof(msg3),0,(struct sockaddr *)&your_adr,sizeof(your_adr));

    close(sock);
    return 0;
}

