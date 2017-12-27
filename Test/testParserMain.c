/*************************************************************************
	> File Name: main.c
	> Author: eli
	> Mail: 1525843015@qq.com
	> Created Time: 2017年12月19日 星期二 10时03分23秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "parser.h"

#define OUTPUT "output.txt"

extern char *metafile_content;
extern int filesize;

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

    printf("test done! filesize %d \n",filesize);
    return 0;
}


