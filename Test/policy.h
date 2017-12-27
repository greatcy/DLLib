/*************************************************************************
	> File Name: policy.h
	> Author: eli
	> Mail: 1525843015@qq.com
	> Created Time: 2017年12月27日 星期三 10时16分42秒
 ************************************************************************/

#ifndef _POLICY_H
#define _POLICY_H

include "peer.h"

#define CMPUTE_RATE_TIME 10
#define UNCHOKE_COUNT 4
#define REQ_SLICE_NUM 4

typedef struct _Unchoke_peer{
    Peer* unchkpeer[UNCHOKE_COUNT];
    int count;
    Peer* optunchkpeer;
} Unchoke_peers;

void init_unchoke_peers();
int select_optunchoke_peer();
int select_optoptunchoke_peer();
int compute_rate();
int compute_total_rate();

int is_seed(Peer *node);
int create_req_slice_msg(Peer *node);

#endif
