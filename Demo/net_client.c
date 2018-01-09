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
#include <netdb.h>

#include "parser.h"
#include "simple_tracker.h"

#define CONNECT_BUF_SIZE 128
#define SEND_HTTP_MSG_LENGTH 512

extern Announce_list *announce_list_head;
extern long long file_length;

int sock;

int connect_tracker(){/*{{{*/
    struct sockaddr_in serv_addr;
    int str_len;
    char *host_name;
    unsigned short *port;
    struct hostent *host;

    host_name=malloc(CONNECT_BUF_SIZE);

    get_tracker_name(announce_list_head,host_name,CONNECT_BUF_SIZE);
    if(host_name==NULL){
        printf("%s:%d error\n",__FILE__,__LINE__);
        return -1;
    }
    
    get_tracker_port(announce_list_head,port);
    if(*port==0){
        printf("%s:%d error\n",__FILE__,__LINE__);
        return -1;
    }

    sock=socket(PF_INET,SOCK_STREAM,0);
    if(sock==-1){
        printf("%s:%d error\n",__FILE__,__LINE__);
        return -1;
    }

    //get host by Name
    host=gethostbyname(host_name);
    if(!host||!(host->h_addr_list[0])){
        printf("%s:%d error\n",__FILE__,__LINE__);
        return -1;
    }

    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=inet_addr(host->h_addr_list[0]);
    serv_addr.sin_port=*port;

    if(connect(sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr))==-1){
        printf("%s:%d error\n",__FILE__,__LINE__);
        return -1;
    }

    free(host_name);
    return 0;
}/*}}}*/

int send_request(){/*{{{*/
    char *request;    

    create_request(request,SEND_HTTP_MSG_LENGTH,announce_list_head,
                  8888,0,0,file_length,50);

    if(request==NULL){
        printf("%s:%d error\n",__FILE__,__LINE__);
        return -1;
    }
    
    //write(sock,request,strlen(request));

    printf("%s\n",request);

    free(request);
}/*}}}*/

void close_socket(){/*{{{*/
    close(sock);
}/*}}}*/

int main(int argc,char *argv[]){

    read_metafile("BTFile.torrent");
    read_announce_list();
    if(announce_list_head==NULL){
        printf("%s:%d error\n",__FILE__,__LINE__);
        return -1;
    }
    get_file_length();
    
    if(connect_tracker()==0){
        send_request();
        close_socket();
    }

    return 0;
}
