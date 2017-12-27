/*************************************************************************
	> File Name: tracker.c
	> Author: eli
	> Mail: 1525843015@qq.com
	> Created Time: 2017年12月27日 星期三 11时32分07秒
 ************************************************************************/

#include "tracker.h"

int http_encode(unsigned char *in,int len1,char *out,int len2){ //TODO
    return 0;
}
int get_tracker_name(Announce_list *node,char *name,int len){
    //TODO
    return 0;
}
int get_tracker_port(Announce_list *node,unsigned short *port){
    //TODO
    return 0;
}

int create_request(char *request,int len,Announce_list *node,unsigned short port,long long down,long long up,long long left,int numwant){
    //TODO
    return 0;
}

int prepare_connect_tracker(int *max_sockfd){
    //TODO
    return 0;
}

int prepare_connect_peer(int *max_sockfd){
    //TODO
    return 0;
}
    

int get_response_type(char *buffer,int len,int *total_length){
    //TODO
    return 0;
}

int parse_tracker_responsel(char *buffer,int ret,char *redirection,int len){
    //TODO
    return 0;
}

int add_peer_node_to_peerlist(int *sock,struct sockaddr_in saptr){
    //TODO
    return 0;
}
void free_peer_addr_head(){
    //TODO   
}

