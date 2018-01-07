/*************************************************************************
	> File Name: torrent.c
	> Author: eli
	> Mail: 1525843015@qq.com
	> Created Time: 2018年01月03日 星期三 10时56分09秒
 ************************************************************************/

#include <stdio.h>
#include "torrent.h"
#include "message.h"
#include "tracker.h"
#include "peer.h"
#include "policy.h"
#include "data.h"
#include "bitfield.h"
#include "parser.h"

#define treshold (18*1024-1500)

extern Announce_list *announce_list_head;
extern char *file_name;
extern long long file_length;
extern int piece_length;
extern char *pieces;
extern int pieces_length;
extern Peer *peer_head;

extern long long total_down,total_up;
extern float total_down_rate,total_up_rate;
extern int total_peers;
extern int download_piece_num;
extern Peer_addr *peer_addr_head;

int *sock=NULL;
struct sockaddr_in *tracke=NULL;
int *valid=NULL;
int tracke_count=0;

int *peer_sock=NULL;
struct sockaddr_in *peer_addr=NULL;
int peer_valid=NULL;
int peer_count=0;
int response_len=0;
int response_index=0;
char *tracker_response=NULL;

int download_upload_with_peers(){
    Peer *p;
    int ret,max_sockfd,i;

    int connect_tracker,connecting_tracker;
    int connect_peer,connecting_peer;

    time_t last_time[3],now_time;

    time_t start_connect_tracker;
    time_t start_connect_peer;
    fd_set rset,wset;
    struct timeval rmval;

    now_time=time(NULL);

    last_time[0]=now_time;
    last_time[1]=now_time;
    last_time[2]=now_time;

    connect_tracker=1;
    connecting_tracker=0;
    connect_peer=0;
    connecting_peer=0;

    for(;;){
        max_sockfd=0;
        now_time=time(NULL);

        //每隔一个gap值,重新计算peer
        if(now_time-last_time[0]>=10){
            if(download_piece_num>0&&peer_head!=NULL){
                //compute_rate();
                //select_unchoke_peer();
                last_time[0]=now_time;
            }
        }

        if(now_time-last_time[0]>=30){
            if(download_piece_num>0&&peer_head!=NULL){
                //compute_rate();
                //select_optunchoke_peer();
                last_time[1]=now_time;
            }
        }

        if((now_time-last_time[2]>=300||connect_tracker==1)&&
            connecting_peer!=1){
            ret=prepare_connect_tracker(&max_sockfd);
            if(ret<0){
                printf("%s:%d",__FILE__,__LINE__);
                return -1;
            }
            connect_tracker=0;
            connecting_tracker=1;
            start_connect_tracker=now_time;
        }

        if(connect_peer==1){
            ret=prepare_connect_peer(&max_sockfd);
            if(ret<0){
                printf("%s:%d",__FILE__,__LINE__);
                return -1;
            }
            connect_peer=0;
            connecting_peer=1;
            start_connect_peer=now_time;
        }

        FD_ZERO(&rset);
        FD_ZERO(&wset);

        //检测tracker超时等
        if(connecting_tracker==1){
            int flag=1;
            if(now_time-start_connect_tracker>10){
                for(i=0;i<tracke_count;i++){
                    if(valid[i]!=0){
                        close(sock[i]);
                    }
                }
            }
            else{
                for(i=0;i<tracke_count;i++){
                    if(valid[i]!=0&&sock[i]>max_sockfd){
                        max_sockfd=sock[i];
                    }

                    if(valid[i]==-1){
                        FD_SET(sock[i],&rset);
                        FD_SET(sock[i],&wset);
                        if(flag==1)flag=0;
                    }
                    else if(valid[i]==1){
                        FD_SET(sock[i],&wset);
                        if(flag==1)flag=0;
                    }
                    else if(valid[i]==2){
                        FD_SET(sock[i],&rset);
                        if(flag==1)flag=0;
                    }
                }
            }

            if(flag==1){
                connecting_tracker=0;
                last_time[2]=now_time;
                clear_connect_trcker();
                clear_tracker_response();
                if(peer_addr_head!=NULL){
                    connect_tracker=0;
                    connect_peer=1;
                }
                else{
                    connect_tracker=1;
                }
                continue;
            }
        }

        if(connecting_peer==1){
            int flag=1;
            if(now_time-start_connect_peer>10){
                for(i=0;i<peer_count;i++){
                    if(peer_valid[i]!=1){
                        close(peer_sock[i]);
                    }
                }
            }
            else{
                for(i=0;i<peer_count;i++){
                    if(peer_valid[i]==-1){
                        if(peer_sock[i]>max_sockfd){
                            max_sockfd=peer_sock[i];
                        }
                        FD_SET(peer_sock[i],&rset);
                        FD_SET(peer_sock[i],&wset);
                        if(flag==1){
                            flag=0;
                        }
                    }
                }
            }
            //TODO 错误的代码,需要考虑FD_SET等IO函数
            if(flag==1){
                connect_peer=0;
                clear_connect_peer();
                if(peer_head==NULL){
                    connect_tracker=1;
                }
                continue;
            }
        }
    }
}

int print_peer_list(){
    
}

void clear_connect_trcker(){

}

void clear_connect_peer(){
    
}

void clear_tracker_response(){

}

void release_memory_in_torrent(){
    
}
