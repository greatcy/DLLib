/*************************************************************************
	> File Name: message.c
	> Author: 
	> Mail: 
	> Created Time: 2017年12月24日 星期日 20时01分56秒
 ************************************************************************/

#include<stdio.h>
#include "message.h"

#define HANDSHAKE       -2
#define KEEP_ALIVE      -1
#define CHOKE           0
#define UNCHOKE         1
#define INTERESTED      2
#define UNINTERESTED    3 
#define HAVE            4 
#define BITFIELD        5 
#define REQUEST         6 
#define PIECE           7 
#define CANCEL          8 
#define PORT            9 

#define KEEP_ALIVE_TIME 45

extern Bitmap *bitmap;
extern char info_hash[20];
extern char peer_id[20];
extern int have_piece_index[64];
extern Peer *peer_head;

int int_to_char(int i,unsigned char c[4]){
    c[3]=i&0xFF;
    c[2]=(i>>8)&0xFF;
    c[1]=(i>>16)&0xFF;
    c[0]=(i>>24)&0xFF;

    return 0;
}

int char_to_int(unsigned char c[4]){
    int i;
    i=c[3]+(c[2]>>8)+
            (c[1]>>16)+
            (c[0]>>24)+
            (c[1]>>16);
    return i;
}

//int create_handshake_msg(){
//    
//}
