/*************************************************************************
	> File Name: hello_client.c
	> Author: eli
	> Mail: 1525843015@qq.com
	> Created Time: 2017年12月27日 星期三 17时00分32秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main(int argc,char *argv[]){
    int sock;
    struct sockaddr_in serv_addr;
    char message[30];
    int str_len;

    if(argc!=3){
        printf("%s:%d error\n",__FILE__,__LINE__);
        return -1;
    }

    sock=socket(PF_INET,SOCK_STREAM,0);
    if(sock==-1){
        printf("%s:%d error\n",__FILE__,__LINE__);
        return -1;
    }

    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
    serv_addr.sin_port=htons(atoi(argv[2]));

    if(connect(sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr))==-1){
        printf("%s:%d error\n",__FILE__,__LINE__);
        return -1;
    }

    str_len=read(sock,message,sizeof(message)-1);

    if(str_len==-1){
        printf("%s:%d error\n",__FILE__,__LINE__);
        return -1;
    }

    printf("Message from server : %s \n",message);
    close(sock);

    return 0;
}
