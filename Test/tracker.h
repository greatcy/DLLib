/*************************************************************************
	> File Name: tracker.h
	> Author: eli
	> Mail: 1525843015@qq.com
	> Created Time: 2017年12月27日 星期三 11时32分07秒
 ************************************************************************/

#ifndef _TRACKER_H
#define _TRACKER_H

#include <netinet/in.h>
#include "parser.h"

typedef struct _Peer_addr{
    char ip[16];
    unsigned short port;
    struct _Peer_addr *next;
} Peer_addr;

int http_encode(unsigned char *in,int len1,char *out,int len2);
int get_tracker_name(Announce_list *node,char *name,int len);
int get_tracker_port(Announce_list *node,unsigned short *port);

int create_request(char *request,int len,Announce_list *node,unsigned short port,
                  long long down,long long up,long long left,int numwant);

int prepare_connect_tracker(int *max_sockfd);
int prepare_connect_peer(int *max_sockfd);

int get_response_type(char *buffer,int len,int *total_length);
int parse_tracker_responsel(char *buffer,int ret,char *redirection,int len);

int add_peer_node_to_peerlist(int *sock,struct sockaddr_in saptr);
void free_peer_addr_head();

#endif
