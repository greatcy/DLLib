/*************************************************************************
	> File Name: message.h
	> Author: 
	> Mail: 
	> Created Time: 2017年12月24日 星期日 19时32分22秒
 ************************************************************************/

#ifndef _MESSAGE_H
#define _MESSAGE_H

#include "peer.h"
int int_to_char(int i,unsigned char c[4]);
int char_to_int(unsigned char c[4]);

int create_handshake_msg(char *info_hash,char *peer_id,Peer *peer);
int create_keep_alive_msg(Peer *peer);
int create_chock_interested_msg(int type,Peer *peer);
int create_have_msg(int index,Peer *peer);
int create_bitfield_msg(char bitfield,int bitfield_len,Peer *peer);
int create_request_msg(int index,int begin,int length,Peer *peer);
int create_piece_msg(int index,int begin,char *block,int b_len,Peer *peer);
int create_cancel_msg(int index,int begin,int length,Peer *peer);
int create_port_msg(int port,Peer *peer);

int is_complete_message(unsigned char *buff,unsigned int len,int *ok_len);
int parse_response(Peer *peer);
int parse_response_uncomplete_msg(Peer *p,int ok_len);
int create_response_message(Peer *peer);
int prepare_send_have_msg();
void discard_send_buffer(Peer *peer);

#endif
