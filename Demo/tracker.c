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

#include "tracker.h"
#include "peer.h"

extern Announce_list *announce_list_head;

int http_encode(unsigned char *str,int strSize,char *result,int bufSize){ 
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
}

int get_tracker_name(Announce_list *node,char *name,int len){
    if(node!=NULL){
	int i,j,start;
	for(i=0;i<128;i++){
	    if(node->annouce[i]=='/'){
		i++;
		start=1;
	    }

	    if(node->annouce[i]==':'){
		return ++j;
	    }
	    else if(start==1){
		name[j]=announce[i];
		j++;
	    }
	}
    }
    return 0;
}

int get_tracker_port(Announce_list *node,unsigned short *port){
    if(node!=NULL){
	int i,j,start;
	for(i=0;i<128;i++){
	    if(node->annouce[i]==':'){
		i++;
		start=1;
	    }

	    if(start==1){
		if(isdigit(announce[i])){
		    name[j]=announce[i];
		    j++;
		}
		else{
		    return ++j;
		}
	    }
	}
    }
    return 0;
}

int create_request(char *request,int len,Announce_list *node,unsigned short port,long long down,long long up,long long left,int numwant){
    char encoded_info_hash[100];
    char encoded_peer_id[100];
    int key;
    char tracker_name[128];
    unsigned short tracker_port;

    http_encode(info_hash,20,encoded_info_hash,100);
    http_encode(peer_id,20,encoded_peer_id,100);

    srand(time(NULL));
    key=rand()/10000;

    get_tracker_name(node,tracker_name,128);
    get_tracker_port(node,&tracker_port);

    sprintf(request,"GET /announce?info_hash=%s&peer_id=%s&port=%u"
            "&uploaded=%lld&downloaded=%lld&left=%lld"
            "&event=started&key=%d&compat=l&numwant=%d HTTP/1.0\r\n"
            "Host:%s\r\nUser-Agent: Bittorrent\r\nAccept: */*\r\n"
            "Accept-Encoding:gzip\r\nConnection: closed\r\n\r\n",
           encoded_info_hash,encoded_peer_id,port,up,down,left,
           key,numwant,tracker_name);

    #ifdef DEBUG
        printf("request :%s\n",request);
    #endif

    return 0;
}

int prepare_connect_tracker(int *max_sockfd){
    int i,flags,ret,count=0;
    struct hostent *ht;
    Announce_list *p=announce_list_head;

    while(p!=NULL){
        count++;
        p=p->next;
    }

    tracker_count=count;
    sock=(int *)malloc(count * sizeof(int));

    if(sock==NULL){
        goto OUT;
    }

    p=announce_list_head;
    for(i=0;i<count;i++){
        char tracker_name[128];
        unsigned short tracker_port=0;

        sock[i]=socket(AF_INET,SOCK_STREAM,0);
        if(sock<0){
            printf("%s:%d socket create failed\n",__FILE__,__LINE__);
            valid[i]=0;
            p=p->next;
            continue;
        }

        get_tracker_name(p,tracker_name,128);
        get_tracker_port(p,&tracker_port);

        ht=gethostbyname(tracker_name);

        if(ht==NULL){
            printf("gethostbyname failed:%s\n",hstrerror(h_errno));
            valid[i]=0;
        }
        else{
            memset(&tracker[i],0,sizeof(struct sockaddr_in));
            memcpy(&tracker[i].sin_addr.s_addr,ht->h_addr_list[0],4);
            tracker[i].sin_port=htons(tracker_port);
            tracker[i].sin_family=AF_INET;
            valid[i]=-1;
        }
        p=p->next;
    }

    for(i=0;i<tracker_count;i++){
        if(valid[i]!=0){
            if(sock[i]>*max_sockfd){
                *max_sockfd=sock[i];
            }

            flags=fcntl(sock[i],F_GETFL,0);
            fcntl(sock[i],F_SETFL,flags|O_NONBLOCK);

            ret=connect(sock[i],(struct sockaddr*)&tracker[i],sizeof(struct sockaddr));
            if(ret<0&&errno!=EINPROGRESS){
                valid[i]=0;
            }
            if(ret==0){
                valid[i]=1;
            }
        }
    }

    return 0;

    OUT:
        if(sock!=NULL)free(sock);
        if(tracker!=NULL)free(tracker);
        if(valid!=NULL)free(valid);
        return -1;
}

int prepare_connect_peer(int *max_sockfd){
    int i,flags,ret,count;
    Peer_addr *p;
    p=peer_addr_head;
    while(p!=0){
        count++;
        p=p->next;
    }
    peer_count=count;
    peer_sock=(int *)malloc(count*sizeof(int));
    if(peer_sock==NULL){
        goto OUT;
    }

    peer_addr=(struct sockaddr_in*)malloc(count*sizeof(struct sockaddr_in));
    if(peer_addr==NULL){
        goto OUT;
    }
    peer_valid=(int *)malloc(count*sizeof(int));
    if(peer_valid==NULL){
        goto OUT;
    }

    p=peer_addr_head;
    for(i=0;i<count&&p!=NULL;i++){
        peer_sock[i]=socket(AF_INET,SOCK_STREAM,0);
        if(peer_sock<0){
            printf("%s:%d socket create failed\n",__FILE__,__LINE__);
            valid[i]=0;
            p=p->next;
            continue;
        }

        memset(&peer_addr[i],0,sizeof(struct sockaddr_in));
        peer_addr[i].sin_addr.s_addr=inet_addr(p->ip);
        peer_addr[i].sin_port=htons(p->port);
        peer_addr[i].sin_family=AF_INET;
        peer_valid[i]=-1;

        p=p->next;
    }
    count=i;

    for(i=0;i<count;i++){
        if(valid[i]!=0){
            if(peer_sock[i]>*max_sockfd){
                *max_sockfd=peer_sock[i];
            }

            flags=fcntl(sock[i],F_GETFL,0);
            fcntl(peer_sock[i],F_SETFL,flags|O_NONBLOCK);

            ret=connect(sock[i],(struct sockaddr*)&peer_addr[i],sizeof(struct sockaddr));
            if(ret<0&&errno!=EINPROGRESS){
                valid[i]=0;
            }
            if(ret==0){
                valid[i]=1;
            }
        }
    }

    return 0;

    OUT:
        if(sock!=NULL)free(sock);
        if(tracker!=NULL)free(tracker);
        if(valid!=NULL)free(valid);
        return -1;
}
    

int get_response_type(char *buffer,int len,int *total_length){
    int i,content_length=0;
    for(i=0;i<len-7;i++){
        if(memcmp(&buffer[i],"5:peers",7)){
            i=i+7;
            break;
        }
    }

    if(i==len-7){
        return -1;
    }

    if(buffer[i]!='1'){
        return 0;
    }

    *total_length=0;

    for(i=0;i<len-16;i++){
        if(memcmp(&buffer[i],"Content_Length: ",16)==0){
            i=i+16;
            break;
        }
    }

    if(i!=len-16){
        while(isdigit(buffer[i])){
            content_length=content_length*10+(buffer[i]-'0');
            i++;
        }
        for(i=0;i<len-4;i++){
            if(memcmp(&buffer[i],"\r\n\r\n",4)==0){
                i=i+4;
                break;
            }
        }

        if(i!=len-4){
            *total_length=content_length+i;
        }
    }

    if(*total_length==0){
        return -1;
    }
    else{
        return 1;
    }
}



