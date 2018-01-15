/*************************************************************************
	> File Name: connect_http.c
	> Author: eli
	> Mail: 1525843015@qq.com
	> Created Time: 2018年01月09日 星期二 18时17分02秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define HOST_NAME "www.sogou.com"
#define PORT 80
#define BUFF_SIZE 1024

int sock;

void do_connect(char **argv){
    struct sockaddr_in serv_addr;
    struct hostent *host;

    sock=socket(PF_INET,SOCK_STREAM,0); 
    if(sock==-1){
        printf("%s,%d error\n",__FILE__,__LINE__);
        exit(1);
    }

    host=gethostbyname(HOST_NAME);
    if(!host||!(host->h_addr_list[0])){
        printf("%s,%d error\n",__FILE__,__LINE__);
        exit(1);
    }

    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    //serv_addr.sin_addr.s_addr=((struct in_addr*)host->h_addr_list[0])->s_addr;
    serv_addr.sin_addr=*(struct in_addr*)host->h_addr_list[0];
    serv_addr.sin_port=htons(PORT);

    if(connect(sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr))==-1){
        printf("%s,%d error\n",__FILE__,__LINE__);
        exit(1);
    }
}

void get_http_response(){
    int strlen=-1;
    char msg[BUFF_SIZE];
    for(;;){
        strlen=read(sock,msg,BUFF_SIZE);
        if(strlen==0){
            return;
        }
        printf("%s\n",msg);
    }
}

void send_http_request(){
    //create http mesage
    char *request=malloc(BUFF_SIZE);
    memset(request,0,BUFF_SIZE);
    sprintf(request,
            "GET %s HTTP/1.1\r\n"
            "Host:%s\r\n"
            "User-Agent:you don't care,right?\r\n"
            "Accept-Encodeing:gzip\r\n"
            "Accept:*/*\r\n",
           HOST_NAME,HOST_NAME);

    write(sock,request,strlen(request));

    free(request);
}

int main(int argc,char **argv){
    do_connect(argv);
    send_http_request();
    get_http_response();
    close(sock);
    return 0;
}
