#ifndef __NET_UTILS__

#define __NET_UTILS__

#include "parser.h"

int http_encode(unsigned char *str,int strSize,char *result,int bufSize);
int get_tracker_name(Announce_list *node,char *name,int len);
int get_tracker_port(Announce_list *node,unsigned short *port,int position);
int create_request(char *request,int len,Announce_list *node,unsigned short port,long long down,long long up,long long left,int numwant);

#endif

