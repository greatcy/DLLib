/*************************************************************************
	> File Name: peer.h
	> Author: eli
	> Mail: 1525843015@qq.com
	> Created Time: 2017年12月20日 星期三 14时32分41秒
 ************************************************************************/

#ifndef _PEER_H
#define _PEER_H

#include <string.h>
#include <time.h>
#include "bitfield.h"

#define INITIAL -1      //initial status
#define HALFSHAKED 0    //half shake status
#define HANDSHAKED 1    //hand shake status
#define SENDBITFIELD 2  //send bitfield status
#define RECVBITFIELD 3  //receive bitfield status
#define DATA 4          //traslate DATA status
#define CLOSING 5       //close peer connection status

#define MSG_SIZE (2*1024+16*1024)

typedef struct _Request_piece{
    int index;
    int begin;
    int length;
    struct _Request_piece *next;
} Request_piece;//request list item

typedef struct _Peer{
    int socket;                 //the collection socket
    char ip[16];
    unsigned short port;
    char id[21];                //the peer id

    int state;                  //current status
    int am_choking;             //if block peer 
    int am_interested;          //if peer is interested
    int peer_choking;           //if blocked by peer
    int peer_interested;        //if we are interested for peer

    Bitmap bitmap;              //the peer bitmap
    
    char *in_buff;              //save peer msg
    int buf_len;                //buff length
    char *out_msg;              //save msg will send to peer
    int msg_len;                //out_msg length
    char *out_msg_copy;         //the copy of out_msg
    int msg_copy_len;           //out_msg_copy length
    int msg_copy_index;         //next msg offset

    Request_piece *Request_piece_head; //request list
    Request_piece *Requested_piece_head; //requested list
    
    unsigned int down_total;    //whole size download from peer
    unsigned int up_total;      //whole size downloaded from peer

    time_t start_timestamp;     //last time receive 
    time_t recet_timestamp;     //last time send msg to peer
    time_t last_down_timestamp;
    time_t last_up_timestamp;
    
    long long down_count;       //the cycle download size from peer
    long long up_count;         //the cycle up size from peer
    
    float down_rate;            //the download speed in this cycle
    float up_rate;              //the up speed 

    struct _Peer *next;
} Peer;

int initialize_peer(Peer *peer);
Peer* add_peer_node();
int del_peer_node(Peer *peer);
void free_peer_node(Peer *node);
int cancel_request_list(Peer *node);
void release_memory_in_peer();
void print_peer_data();

#endif
