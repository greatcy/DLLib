/*************************************************************************
	> File Name: parser.c
	> Author: eli
	> Mail: 1525843015@qq.com
	> Created Time: 2017年12月18日 星期一 18时36分05秒
************************************************************************/
#include <stdio.h>
#include <ctype.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include "parser.h"
#include "sha1.h"

unsigned char *metafile_content=NULL;
long filesize;
int piece_length=0;
char *piece=NULL;
int pieces_length=0;
int muti_file=0;//mutifile count?
char *file_name=NULL;
long long file_length=0;
Files *files_head=NULL;//mutifile will be a link.
unsigned char info_hash[20];
unsigned char peer_id[20];
Announce_list *announce_list_head=NULL;
int read_metafile(char *metafile_name){
    if(metafile_name==NULL){
        file_name=metafile_name;
    }
    FILE *file=fopen(metafile_name,"r");
    if(file==NULL){
        return -1;
    }
    //should read char from file while it will eof with '/0'
    fseek(file,0,SEEK_END);
    int length=ftell(file);
    if(length==0){
        return -1;
    }
    fseek(file,0,SEEK_SET);
    metafile_content=malloc(length+1);
    int i;
    for(i=0;i<length;i++){
        metafile_content[i]=fgetc(file);
    }

    metafile_content[length]='\0';
    filesize=length+1;
    return 0;
}

int find_keyword(char *keyword,long *position){
    if(keyword==NULL){
        return -1; 
    } 
    int length=0;
    while(keyword[length]!='\0'){
       length++; 
    }
    if(length>filesize){
        return -1;
    }
    long cur_pos=0;
    while(cur_pos+length<=filesize){
        if(memcmp(keyword,&metafile_content[cur_pos],length)==0){
            *position=cur_pos;
            return 1;
        } 
        cur_pos++;
    }
    return 0;
}

int read_announce_list(){
    Announce_list *node=NULL;
    Announce_list *p=NULL;
    long position;
    int len=0;
    int find_keyword_result=find_keyword("13:announce_list",&position);
    if(find_keyword_result==0){
        if(find_keyword("8:announce",&position)==1){
            position=position+strlen("8:announce"); 
            //get the url length
            while(isdigit(metafile_content[position])){
                len=len*10+(metafile_content[position]-'0'); 
                position++;
            }
            position++;//skip :
            node = (Announce_list *)malloc(sizeof(Announce_list));
            strncpy(node->annouce,&metafile_content[position],len);
            node->annouce[len]='\0';
            node->next=NULL;
            announce_list_head=node;
        } 
    }  
    else if(find_keyword_result==1){
        position=position+strlen("13:announce_list"); 
        position++;//skip l
        while(metafile_content[position]!='e'){
            //loop to get urls list 
            position++;//for announce_list it is a list ,so skip 'l'
            while(isdigit(metafile_content[position])){
                len=len*10+(metafile_content[position]-'0');
                position++;
            }
            if(metafile_content[position]==':'){
                position++;
            }
            else{
                return -1;
            }

            //TODO just handle http/https protocol
            if(memcmp(&metafile_content[position],"http",4)||
              memcmp(&metafile_content[position],"https",5)){
                node=(Announce_list *)malloc(sizeof(Announce_list)); 
                strncpy(node->annouce,&metafile_content[position],len);
                node->annouce[len]='\0';
                node->next=NULL;

                if(announce_list_head==NULL){
                    announce_list_head=node;
                }
                else{
                    p=announce_list_head;
                    while(p->next!=NULL)
                        p=p->next;
                    p->next=node;
                }
            }
 
            //goto next announce_list
            position=position+len;
            len=0;
            position++;//skip 'e'
            if(position>filesize){
                return -1;
            }
        }
    }
    else{
        //can't find announce_list
        return -1;
    }
}

int add_an_announce(char *url){
    if(url==NULL){
        return -1;
    }
    
    Announce_list *p=announce_list_head,*q;

    while(p!=NULL){
        if(strcmp(p->annouce,url)==0){
            break;
        } 
        p=p->next;
    }

    if(p!=NULL){
       return -1; 
    }

    q=(Announce_list *)malloc(sizeof(Announce_list));
    strcpy(q->annouce,url);
    q->next=NULL;

    p=announce_list_head;
    if(p==NULL){
        announce_list_head=q;
        return 0;
    }
    while(p->next!=NULL){
       p=p->next; 
    }
    p->next=q;
    return 1;
}

int is_muti_files(){
    long i;
    if(find_keyword("5:files",&i)==1){
        muti_file=1;
        return 1;
    }

    #ifdef DEBUG
        printf("is multi_files:%d\n",multi_files);
    #endif

    return 0;
}

int get_piece_length(){
    long i;
    if(find_keyword("12:piece_length",&i)==1){
        i=i+strlen("12:piece_length");
        i++;//skip i
        while(metafile_content[i]!='e')
        {
            piece_length=piece_length*10+(metafile_content[i]-'0');
            i++;
        }
    }
    else{
        return -1;
    }

    #ifdef DEBUG
        printf("piece_length:%d\n",piece_length);
    #endif
}

int get_pieces(){
    long i;
    
    if(find_keyword("6:pieces",&i)==1){
        i=i+8;
        while(metafile_content[i]!=':'){
            piece_length=pieces_length*10+(metafile_content[i]-'0');
            i++;
        }
        i++;//skip ':'
        piece=(char *)malloc(pieces_length+1);
        memcpy(piece,&metafile_content[i],pieces_length);
        piece[pieces_length]='\0';
    }
    else{
        return -1;
    } 
    #ifdef DEBUG
        printf("get piece done!\n");
    #endif
}

int get_file_name(){
    long i;
    int count=0;

    if(find_keyword("4:name",&i)==1){
        i=i+6;//skip "4:name"
        while(metafile_content[i]!=':'){
            count=piece_length*10+(metafile_content[i]-'0');
            i++;
        }
        i++;//skip ':'
        file_name=(char *)malloc(count+1);
        memcpy(file_name,&metafile_content[i],count);
        file_name[count]='\0';
    }
    else{
        return -1;
    } 
    #ifdef DEBUG
        printf("file_name %s!\n",file_name);
    #endif
    return 0;
}

int get_files_length_path(){
    long position=0;
    int len;
    Files *file;
    if(find_keyword("5:files",&position)==1){
        position++;//skip 'l' 
        while(metafile_content[position]!='e'){
        //skip 'd'
        position++;
         //get a Files
        file=(Files *)malloc(sizeof(Files));
        if(memcmp("6:length",&metafile_content[position],8)){
            //skip 6:length
            position=position+8;
            //skip 'i'
            position++;
            while(metafile_content[position]!='e'){
               len=len*10+(metafile_content[position]-'0');
            }
            file->length=len;
            //skip 'e'
            position++;
         }
         else if(memcmp("4:path",&metafile_content[position],4)){//path
            //skip l 
            //TODO may be more than one path 
            position++;
            while(metafile_content[position]!='e')
            {
               len=len*10+(metafile_content[position]-'0');
            }
            strncpy(file->path,&metafile_content[position],len);
            //skip e
            position++;
         }
         else{
            //run error!
            return -1;
         }
        if(files_head==NULL){
            files_head=file;
        }
        else{
            Files *p=files_head->next;
            while(p!=NULL){
                if(p->next==NULL){
                    p->next=file;
                }
            }   
        }
        //goto another map
        }
    }
}

int get_file_length(){
    long i;
    if(is_muti_files()){
        if(files_head==NULL){
           get_files_length_path; 
        }
        Files *p=files_head;
        while(p!=NULL){
            file_length+=p->length;p=p->next;
        }
    }
    else{
        if(find_keyword("6:length",&i)){
            i=i+8;//skip 6:length
            i++;//skip i
            while(metafile_content[i]!='e'){
                file_length=file_length*10+(metafile_content[i]='e');
                i++;
            }
        }
    }

    #ifdef DEBUG
        printf("file_length %s!\n",file_length);
    #endif
}

int get_info_hash(){
    int push_pop;
    long i,begin,end;
    if(metafile_content!=NULL){
        if(find_keyword("4:info",&i)==1){
            begin=i+6;
        }
        else{
            return -1;
        }

        i=i+6;//skip '4:info'
        for(;i<filesize;){
            if(metafile_content[i]=='d'){
                push_pop++;
                i++;
            } 
            else if(metafile_content[i]=='l'){
                push_pop++;
                i++;
            }
            else if(metafile_content[i]=='i'){
                i++;
                if(i==filesize){
                    return -1;
                }
                while(metafile_content[i]!='e'){
                    if((i+1)==filesize){
                        return -1;
                    }
                    else{
                        i++;
                    }
                }
                i++;//skip 'e'
            }
            else if(metafile_content[i]>='0'&&metafile_content[i]<'9'){
                int number=0;
                    while(metafile_content[i]>='0'&&metafile_content[i]<='9'){
                    number=number*10+metafile_content[i]-'0';
                    i++;
                    }
                i++;//skip ":"
                i=i+number;//skip string
            }
            else if(metafile_content[i]=='e'){
                push_pop--;
                if(push_pop==0){
                    end=i;
                    break;
                }
                else{
                    i++;
                }
            }
            else{
                return -1;
            }
        }
        if(i==filesize){
            return -1;
        }

        SHA1_CTX context;
        SHA1_Init(&context);
        SHA1_Update(&context,&metafile_content[begin],end-begin+1);
        SHA1_Final(&context,info_hash);
    }
    else{
        return -1;
        }
}

int get_peer_id(){
    srand(time(NULL));
    sprintf(peer_id,"-TT1000-%12d",rand());
    #ifdef DEBUG
        printf("peer_id%s!\n",peer_id);
    #endif
    return 0;
}

void release_memory_in_parse_metafile(){
    Announce_list *p;
    Files *q;

    if(metafile_content!=NULL){
        free(metafile_content);
    }

    if(file_name!=NULL){
        free(file_name);
    }

    if(piece!=NULL){
        free(piece);
    }

    while(announce_list_head!=NULL){
        p=announce_list_head;
        announce_list_head=announce_list_head->next;
        free(p);
    }

    while(files_head!=NULL){
        q=files_head;
        files_head=files_head->next;
        free(q);
    }
}
