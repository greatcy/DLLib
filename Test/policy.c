/*************************************************************************
	> File Name: policy.c
	> Author: eli
	> Mail: 1525843015@qq.com
	> Created Time: 2017年12月27日 星期三 10时20分53秒
 ************************************************************************/

#include <stdio.h>
#include "policy.h"
#include "peer.h"
#include "data.h"

long long total_down=0L,total_up=0L;

float total_down_rate=0.0F,total_up_rate=0.0F;

int total_peer=0;//已经连接的peer 数

//TODO why defination error?
Unchoke_peers unchoke_peers;//存放非阻塞Peer和优化非阻塞peer 指针

extern int end_mode;
extern Bitmap *bitmap;
extern Peer *peer_head;
extern int pieces_length;   //所有piece哈希值的长度
extern int piece_length;    //每个piece的长度

extern Btcache *btcache_head;
extern int last_piece_index;
extern int last_piece_count; //最后piece的slice数
extern int last_slice_len;
extern int download_piece_num;

void init_unchoke_peers(){
    //TODO
}

int is_in_unchoke_peers(Peer *peer){
    //TODO
    return 0;
}

//在非阻塞队列中获取速度最慢的peer
int get_last_index(Peer **array,int len){
    //TODO
    return 0;
}

int select_unchoke_peer(){
    //TODO
    return 0;
}

//打乱piece的顺序,实现片段的初始化
int get_rand_numbers(int lenght){
    //TODO
    return 0;
}

int select_optunchoke_peer(){
    //TODO
    return 0;
}

int comput_rate(){
    //TODO
    return 0;
}

int comput_total_rate(){
    //TODO
    return 0;
}

int is_seed(Peer *peer){
    //TODO
    return 0;
}

int create_req_slice_msg(Peer *node){
    //TODO
    return 0;
}

