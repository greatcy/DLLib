/*************************************************************************
	> File Name: peer.c
	> Author: eli
	> Mail: 1525843015@qq.com
	> Created Time: 2017年12月20日 星期三 17时33分03秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "peer.h"
#include "bitfield.h"

extern Bitmap *bitmap;
Peer *peer_head=NULL;

int initialize_peer(Peer *peer){
    if(peer==NULL){
        return -1;
    }

    peer->socket=-1;
    memset(peer->ip,0,16);
    peer->port=0;
    memset(peer->id,0,21);
    peer->state=INITIAL;

    peer->in_buff=NULL;
    peer->out_msg=NULL;
    peer->out_msg_copy=NULL;

    peer->in_buff=(char *)malloc(MSG_SIZE);
    if(peer->in_buff==NULL){
        goto OUT;
    }
    memset(peer->in_buff,0,MSG_SIZE);
    peer->buf_len=0;

    peer->out_msg=(char *)malloc(MSG_SIZE);
    if(peer->out_msg==NULL){
        goto OUT;
    }
    memset(peer->out_msg,0,MSG_SIZE);
    peer->msg_len=0;

    peer->out_msg_copy=(char *)malloc(MSG_SIZE);
    if(peer->out_msg_copy==NULL){
        goto OUT;
    }
    memset(peer->out_msg_copy,0,MSG_SIZE);
    peer->msg_copy_len=0;
    peer->msg_copy_index=0;

    peer->am_choking=1;
    peer->am_interested=0;
    peer->peer_choking=1;
    peer->peer_interested=0;

    peer->bitmap.bitfield=NULL;
    peer->bitmap.bitfield_length=0;

    peer->Request_piece_head=NULL;
    peer->Requested_piece_head=NULL;

    peer->down_total=0;;
    peer->up_total=0;

    peer->start_timestamp=0;
    peer->recet_timestamp=0;

    peer->last_down_timestamp=0;
    peer->last_up_timestamp=0;
    peer->down_count=0;
    peer->up_count=0;
    peer->down_rate=0;
    peer->up_rate=0;

    peer->next=NULL;

    return 0;
OUT:
    if(peer->in_buff!=NULL)     free(peer->in_buff);
    if(peer->out_msg!=NULL)     free(peer->out_msg);
    if(peer->out_msg_copy!=NULL)    free(peer->out_msg_copy);
    return -1;
}
