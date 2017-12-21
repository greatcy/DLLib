/*************************************************************************
	> File Name: data.c
	> Author: eli
	> Mail: 1525843015@qq.com
	> Created Time: 2017年12月21日 星期四 14时11分29秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "data.h"
#include "bitfield.h"
#include "peer.h"
#include "parser.h"

extern char *file_name;
extern Files *files_head;//mutifile mode
extern int file_length;
extern int piece_length;
extern char *pieces;
extern int pieces_lenght;

extern Bitmap *bitmap;
extern int download_piece_num;
extern Peer *peer_head;

#define btcache_len 1024 
Btcache *btcache_head=NULL;
Btcache *last_piece=NULL;
int last_piece_index=0;
int last_piece_count=0;
int last_slice_len=0;

int *fds=NULL;
int fds_len=0;
int have_piece_index[64];
int end_mode=0;

Btcache* initialize_btcache_node(){
    Btcache *node;
    node=(Btcache *)malloc(sizeof(Btcache));

    if(node==NULL){
        return NULL;
    }

    node->buff=(unsigned char *)malloc(16*1024);
    if(node->buff==NULL){
        if(node!=NULL){
            free(node);
            return NULL;
        } 
        node->index=-1;
    }
    
    node->begin=-1;
    node->length=-1;

    node->in_use=0;
    node->read_write=-1;

    node->is_full=0;
    node->is_writed=-1;
    node->access_count=0;
    node->next=NULL;

    return node;
}

//create 1024 + (the count of slice which in last piece) nodes
int create_btcache(){
    int i;
    Btcache *node,*last;
    for(i=0;i<btcache_len;i++){
        node=initialize_btcache_node();
        if(node==NULL){
            printf("%s:%d error\n",__FILE__,__LINE__);
            release_memory_in_btcache();
            return -1;
        }

        if(btcache_head==NULL){
            btcache_head=node;
            last=node;
        }
        else{
            last->next=node;last=node;
        }
    }

    //TODO the caculation is redundant
    //for last piece
    int count=file_length%piece_length/(16*1024);
    if(file_length%piece_length%(16*1024)!=0){
        count++;
    }
    //the slice count of last piece
    last_piece_count=count;
    last_slice_len=file_length%piece_length%(16*1024);
    last_piece_index=pieces_lenght/20-1;
    while(count>0){
        node=initialize_btcache_node();
        if(node==NULL){
            printf("%s:%d error\n",__FILE__,__LINE__);
            release_memory_in_btcache();
            return -1;
        }
        if(last_piece==NULL){
            last_piece=node;last=node;
        }
        else{
            last->next=node;last=node;
        }
        count--;
    }
}

void release_memory_in_btcache(){
    Btcache *p=btcache_head;
    while(p!=NULL){
        btcache_head=p->next;
        if(p->buff!=NULL){
            free(p->buff);
        }
        free(p);
        p=btcache_head;
    }
    release_last_piece();
    if(fds!=NULL){
        free(fds);
    }
}

void release_last_piece(){
    Btcache *p=last_piece;
    while(p!=NULL){
        last_piece=p->next;
        if(p->buff!=NULL){
            free(p->buff);
        }
        free(p);
        p=last_piece;
    }
}

int get_files_count(){
    int count=0;

    if(is_multi_files()==0){
        return 1;
    }
    Files *p=files_head;
    while(p!=NULL){
        count++;
        p=p->next;
    }

    return count;
}

int create_files(){
    int ret,i;
    char buff[1]={0x0};

    fds_len=get_files_count();
    if(fds_len<0){
        return -1;
    }
    fds=(int *)malloc(fds_len*sizeof(int));// why ?
    if(fds==NULL){
        return -1;
    }

    if(is_multi_files()==0){
        *fds=open(file_name,O_RDWR|O_CREAT,0777);
        if(*fds<0){
            printf("%s:%d error\n",__FILE__,__LINE__);
            return -1;
        }
        ret=lseek(*fds,file_length-1,SEEK_SET);
        if(ret<0){
            printf("%s:%d error\n",__FILE__,__LINE__);
            return -1;
        }
        ret=write(*fds,buff,1);
        if(ret==-1){
            printf("%s:%d error\n",__FILE__,__LINE__);
            return -1;
        }
    }
    else{//multifile mode
        ret=chdir(file_name);
        if(ret<0){//should create a director
            ret=mkdir(file_name,0777);
            if(ret<0){
                printf("%s:%d error\n",__FILE__,__LINE__);
                return -1;
            }
            ret=chdir(file_name); 
            if(ret<0){
                printf("%s:%d error\n",__FILE__,__LINE__);
                return -1;
            }
        }
        Files *p=files_head;
        i=0;
        while(p!=NULL){
            fds[i]=open(p->path,O_RDWR|O_CREAT,0777); 
            if(fds[i]<0){
                printf("%s:%d error\n",__FILE__,__LINE__);
                return -1;
            }

            ret=lseek(fds[i],file_length-1,SEEK_SET);
            if(ret<0){
                printf("%s:%d error\n",__FILE__,__LINE__);
                return -1;
            }

            ret=write(fds[i],buff,1);
            if(ret==-1){
                printf("%s:%d error\n",__FILE__,__LINE__);
                return -1;
            }

            p=p->next;
            i++;
        }
    }
    return 0;
}

int write_btcache_node_to_harddisk(Btcache *node){
    long long line_position;
    Files *p;
    int i;

    if((node==NULL)||(fds==NULL)){
       return -1; 
    }

    //whatever single file or mutifile. the position is linnable.
    line_position=node->index*piece_length+node->begin;
    if(is_multi_files()==0){
        lseek(*fds,line_position,SEEK_SET);
        write(*fds,node->buff,node->length);
        return 0;
    }

    //mutifile mode
    if(files_head==NULL){
        printf("%s:%d error\n",__FILE__,__LINE__);
        return -1;
    }

    p=files_head;
    i=0;
    while(p!=NULL){
        if((line_position<p->length)&&(line_position+node->length)<p->length){
            //slice in this file
            lseek(fds[i],line_position,SEEK_SET);
            write(fds[i],node->buff,node->length);
            break;
        } 
        else if((line_position<p->length)&&(line_position+node->length>=p->length)){
            //the slice cross more than one file. 
            int offset=0;
            int left=node->length;
            lseek(fds[i],line_position,SEEK_SET);
            write(fds[i],node->buff,p->length-line_position);
            offset=p->length-line_position;
            left=left-(p->length-line_position);
            p=p->next;
            i++;

            while(left>0){
                if(p->length>=left){
                    lseek(fds[i],0,SEEK_SET);
                    write(fds[i],node->buff+offset,left);
                    left=0;
                } 
                else{
                    lseek(fds[i],0,SEEK_SET);
                    write(fds[i],node->buff+offset,p->length);
                    offset=offset+p->length;
                    left=left-p->length;
                    i++;
                    p=p->next;
                }
            }
            break;
        }
        else{
            //the slice is not belong to this file.
            line_position=line_position-p->length;
            i++;
            p=p->next;
        }
    }
    return 0;
}

int read_slice_from_hardisk(Btcache *node){
    long long line_position;
    Files *p;
    int i;
    if((node==NULL)||(fds==NULL)){
        return -1;
    }

    line_position=node->index*piece_length+node->begin;
    if(is_multi_files){
        lseek(*fds,line_position,SEEK_SET);
        ssize_t ret=read(*fds,node->buff,node->length);
        if(ret<0||ret!=node->length){
            printf("%s:%d error\n",__FILE__,__LINE__);
            return -1;
        }
        return 0;
    }

    if(files_head==NULL){
        printf("%s:%d error\n",__FILE__,__LINE__);
    }

    p=files_head;
    i=0;
    while(p!=NULL){
        if((line_position>p->length)&&(line_position+node->length)<p->length){
            //slice int this file
            lseek(fds[i],line_position,SEEK_SET);
            read(fds[i],node->buff,node->length);
            break;
        }
        else if((line_position<p->length)&&(line_position+node->length>=p->length)){
            //slic cross more than one file 
            int offset=0;
            int left=node->length;
            lseek(fds[i],line_position,SEEK_SET);
            read(fds[i],node->buff,p->length);
            offset=p->length-line_position;
            left=left-(p->length-line_position);
            p=p->next;
            i++;
            while(left>0){
                if(p->length>left){
                    lseek(fds[i],0,SEEK_SET);
                    read(fds[i],node->buff+offset,left);
                    left=0;
                }
                else{
                    lseek(fds[i],0,SEEK_SET);
                    read(fds[i],node->buff+offset,p->length);
                    offset=offset+p->length;
                    left=left-p->length;
                    i++;
                    p=p->next;
                }
            }
            break;
        }
        else{
            line_position=line_position-p->length;
            i++;
            p=p->next;
        }
    }

    return 0;
}

int write_piece_to_harddisk(int sequence,Peer *peer){
    Btcache *node_ptr=btcache_head; 
}