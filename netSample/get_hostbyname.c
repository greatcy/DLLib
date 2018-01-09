/*************************************************************************
	> File Name: get_hostbyname.c
	> Author: eli
	> Mail: 1525843015@qq.com
	> Created Time: 2018年01月09日 星期二 11时07分10秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

#define OUTPUT_FILE "gethostbyname.output"


int main(int argc,char **argv){
    int i;
    struct hostent *host;
    remove(OUTPUT_FILE);

    //FILE *output_file=fopen(OUTPUT_FILE,"a+");
    
    if(argc!=2){
        printf("Usage : %s <addr>\n",argv[0]);
        exit(0);
    }

    host=gethostbyname(argv[1]);
    if(!host){
        printf("%s:%d error\n",__FILE__,__LINE__);
        return -1;
    }

    //fprintf(output_file,"official name : %s\n",host->h_name);
    printf("official name : %s\n",host->h_name);
    for(i=0;host->h_aliases[i];i++){
        //fprintf(output_file,"Aliases %d:%s\n",i+1,host->h_aliases[i]);
        printf("Aliases %d:%s\n",i+1,host->h_aliases[i]);
    }

    //fprintf(output_file,"Address type : %s\n",(host->h_addrtype==AF_INET)?"AF_INET":"AF_INET6");
    printf("Address type : %s\n",(host->h_addrtype==AF_INET)?"AF_INET":"AF_INET6");

    for(i=0;host->h_addr_list[i];i++){
        //fprintf(output_file,"ip addr %d:%s\n",i+1
        //        ,inet_ntoa(*(struct in_addr*)host->h_addr_list[i]));
        printf("ip addr %d:%s\n",i+1
                ,inet_ntoa(*(struct in_addr*)host->h_addr_list[i]));
    }

    return 0;
}
