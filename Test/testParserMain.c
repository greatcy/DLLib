/*************************************************************************
	> File Name: main.c
	> Author: eli
	> Mail: 1525843015@qq.com
	> Created Time: 2017年12月19日 星期二 10时03分23秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"

#define OUTPUT "output.txt"

extern char *metafile_content;
extern int filesize;
extern Announce_list *announce_list_head;
extern int piece_length;


//TODO finish all test case.

void test_get_peer_id(){

}

void test_get_info_hash(){

}

//多文件,获取所有文件信息
void test_get_files_length_path(){

}

void test_get_file_length(){

}

void test_get_file_name(){

}

void test_is_multi_files(){

}

void test_get_piece_length(){
    get_piece_length();
    if(piece_length==16384){
        printf("%s:%d error\n",__FILE__,__LINE__);
        return;
    }
}

void test_read_announce_list(){
    read_announce_list();

    if(announce_list_head==NULL){
        printf("%s:%d error\n",__FILE__,__LINE__);
        return;
    }

    if(!memcmp(announce_list_head->annouce,"https://open.kickasstracker.com:443/announce",44)){
        printf("%s:%d error\n",__FILE__,__LINE__);
        return;
    }

    if(announce_list_head->next!=NULL){
        printf("%s:%d error\n",__FILE__,__LINE__);
        return;
    }
}

void testFindKeyword(){
    long position;
    if(find_keyword("d8:announce",&position)!=1){
        printf("%s:%d error\n",__FILE__,__LINE__);
        return;
    }

    if(position!=0){
        printf("%s:%d error\n",__FILE__,__LINE__);
        return;
    }

    position=-1;
    if(find_keyword("d8:1231231announce",&position)==1){
        printf("%s:%d error\n",__FILE__,__LINE__);
        return;
    }

    if(position!=-1){
        printf("%s:%d error\n",__FILE__,__LINE__);
        return;
    }
}

void testReadMetafile(){
    FILE *outputFile=fopen(OUTPUT,"w");
    if(outputFile==NULL){
        printf("Open %s fail!\n",OUTPUT);
        return;
    }

    //fprintf(outputFile,"%s\n","12345 new text");
    read_metafile("testBTFile.torrent");
    if(metafile_content==NULL){
        printf("%s:%d error\n",__FILE__,__LINE__);
        return;
    }

    //error! the 0x00(hash value) may in middle of torrent
    //fprintf(outputFile,"%s\n",metafile_content);

    fseek(outputFile,0,SEEK_SET);
    int i;
    for(i=0;i<filesize-1;i++){
        fputc(metafile_content[i],outputFile);
    }
}

int main(){
    //don't change the execute sequence;
    testReadMetafile();
    printf("testReadMetafile done\n");
    testFindKeyword();
    printf("testFindKeyword done\n");
    test_read_announce_list();
    printf("test_read_announce_list done\n");
    test_get_piece_length();
    printf("test_get_piece_length done\n");

    printf("test done! filesize %d \n",filesize);
    return 0;
}


