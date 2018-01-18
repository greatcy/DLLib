/*************************************************************************
	> File Name: tracker.c
	> Author: eli
	> Mail: 1525843015@qq.com
	> Created Time: 2017年12月27日 星期三 11时32分07秒
 ************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <time.h>

#include "simple_tracker.h"
#include "parser.h"

extern unsigned char info_hash[20];
extern Announce_list *announce_list_head;
extern unsigned char peer_id[20];

int http_encode(unsigned char *str,int strSize,char *result,int bufSize){ /*{{{*/
    int i;
    int j = 0;//for result index
    char ch;

    if ((str==NULL) || (result==NULL) || (strSize<=0) || (bufSize<=0)) {
        return 0;
    }

    for ( i=0; (i<strSize)&&(j<bufSize); ++i) {
        ch = str[i];
        if (((ch>='A') && (ch<'Z')) ||
            ((ch>='a') && (ch<'z')) ||
            ((ch>='0') && (ch<'9'))) {
            result[j++] = ch;
        } else if (ch == ' ') {
            result[j++] = '+';
        } else if (ch == '.' || ch == '-' || ch == '_' || ch == '*') {
            result[j++] = ch;
        } else {
            if (j+3 < bufSize) {
                sprintf(result+j, "%%%02X", (unsigned char)ch);
                j += 3;
            } else {
                return 0;
            }
        }
    }

    result[j] = '\0';
    return j;
}/*}}}*/

int get_tracker_name(Announce_list *node,char *name,int len){/*{{{*/
    if(node!=NULL){
	int i=0,j=0,start=0;
	for(i=0;i<128;i++){
	    if(node->annouce[i]=='/'){
		    i+=2;//skip //
		    start=1;
	    }

	    if(start==1&&node->annouce[i]==':'){
		    return ++j;
	    }
	    else if(start==1){
		    name[j]=node->annouce[i];
		    j++;
	    }
	}
    }
    return 0;
}/*}}}*/

int get_tracker_port(Announce_list *node,unsigned short *port,int position){/*{{{*/
    if(node!=NULL){
	    int i=0;
        unsigned short tmp;
	    for(i=position;i<128;i++){
	        if(node->annouce[i]==':'){
                i++;//skip :
	    	    while(isdigit(node->annouce[i])){
	    	        tmp=tmp*10+(node->annouce[i]-'0');
	    	        i++;
	    	    }
                *port=tmp;
	    	    return i;
	        }
	    }
    }
    return 0;
}/*}}}*/

int create_request(char *request,int len,Announce_list *node,unsigned short port,long long down,long long up,long long left,int numwant){/*{{{*/
    char encoded_info_hash[100];
    char encoded_peer_id[100];
    int key;
    char tracker_name[128];
    unsigned short tracker_port;

    http_encode(info_hash,20,encoded_info_hash,100);
    http_encode(peer_id,20,encoded_peer_id,100);

    srand(time(NULL));
    key=rand()/10000;

    int offset=get_tracker_name(node,tracker_name,128);
    get_tracker_port(node,&tracker_port,offset);

    sprintf(request,"GET /announce?info_hash=%s&peer_id=%s&port=%u"
            "&uploaded=%lld&downloaded=%lld&left=%lld"
            "&event=started&key=%d&compat=l&numwant=%d HTTP/1.0\r\n"
            "Host:%s\r\nUser-Agent: Bittorrent\r\nAccept: */*\r\n"
            "Accept-Encoding:gzip\r\nConnection: closed\r\n\r\n",
           encoded_info_hash,encoded_peer_id,port,up,down,left,
           key,numwant,tracker_name);

    //#ifdef DEBUG
    //    printf("request :%s\n",request);
    //#endif

    return 0;
}/*}}}*/
