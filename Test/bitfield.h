/*************************************************************************
	> File Name: bitfield.c
	> Author: eli
	> Mail: 1525843015@qq.com
	> Created Time: 2017年12月20日 星期三 10时14分55秒
 ************************************************************************/

#include <stdio.h>
#include "bitfield.h"
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

extern int pieces_length;
extern char *file_name;

Bitmap *bitmap=NULL;
int download_piece_num=0;

int create_bitfield(){
    bitmap=(Bitmap *)malloc(sizeof(Bitmap));
    if(bitmap==NULL){
        printf("allocate memory for bitmap fialed!\n");
        return -1;
    }

    bitmap->valid_length=pieces_length/20;//because a hash code 20byte
    bitmap->bitfield_length=pieces_length/20/8;//one bit present a piece status
    if((pieces_length/20)%8!=0){
        bitmap->bitfield_length++;//the last byte less than 8 piece,so valid_length used.
    }

    bitmap->bitfield=(unsigned char *)malloc(bitmap->bitfield_length);

    if(bitmap->bitfield==NULL){
        printf("allocate memory for bitmap->bitfield failed\n");
        if(bitmap!=NULL){
            free(bitmap);
        }
        return -1;
    }

    char bitmapfile[64];
    sprintf(bitmapfile,"%dbitmap",pieces_length);
    int i;
    FILE *fp=fopen(bitmapfile,"r");
    if(fp==NULL){
        memset(bitmap->bitfield,0,bitmap->bitfield_length);
    }
    else{
        fseek(fp,0,SEEK_SET); 
        for(int i=0;i<bitmap->bitfield_length;i++){
            (bitmap->bitfield)[i]=fgetc(fp);
        }
        fclose(fp);
        download_piece_num=get_download_piece_num();
    }
    return 0;
}

int get_bit_value(Bitmap *bitmap,int index){
    int ret;
    int byte_index;
    unsigned char byte_value;
    unsigned char inner_byte_index;

    if(bitmap==NULL||index>=bitmap->valid_length){
        return -1;
    }

    byte_index=index/8;
    byte_value=bitmap->bitfield[byte_index];//get the byte contain status bit
    inner_byte_index=index%8;
    byte_value=byte_value>>(7-inner_byte_index);
    if(byte_value%2==0){//1%2 or 0%2
        ret=0; 
    }
    else{
        ret=1;
    }
    return ret;
}

int set_bit_value(Bitmap *bitmap,int index,unsigned char value){
    int ret;
    int byte_index;
    unsigned char byte_value;
    unsigned char inner_byte_index;

    if(bitmap==NULL||index>=bitmap->valid_length){
        return -1;
    }

    byte_index=index/8;
    byte_value=bitmap->bitfield[byte_index];//get the byte contain status bit
    unsigned char tmp=value<<(7-inner_byte_index);
    bitmap->bitfield[byte_index]=tmp&byte_value;
    return 1;
}

int all_zero(Bitmap *bitmap){
    if(bitmap!=NULL){
        memset(bitmap->bitfield,0,bitmap->bitfield_length);   
    }
    return 0;
}

int all_set(Bitmap *bitmap){
    if(bitmap!=NULL){
        memset(bitmap->bitfield,1,bitmap->bitfield_length);   
    }
    return 0;
}

void release_memory_in_bitfield(){
    if(bitmap!=NULL){
        if(bitmap->bitfield!=NULL){
            free(bitmap->bitfield);
        }
        free(bitmap);
    }
}

int is_interested(Bitmap *dst,Bitmap *src){

    unsigned char const_char[8]={0x80,0x40,0x20,0x10,
                                 0x08,0x04,0x02,0x01};
    unsigned char c1,c2;
    int i,j;
    if(dst==NULL||src==NULL) return -1;

    if(dst->bitfield_length!=src->bitfield_length||
       dst->valid_length!=src->valid_length){
           return -1;
       }

    for(i=0;i<dst->bitfield_length-1;i++){
        for(j=0;j<8;j++){
            c1=(dst->bitfield)[i] & const_char[j];
            c2=(src->bitfield)[i] & const_char[j];
            if(c1>0&&c2==0){
                return 1;
            }
        }
    }

    //compare the last byte
    j=dst->valid_length%8;
    c1=dst->bitfield[dst->bitfield_length-1];
    c2=src->bitfield[src->bitfield_length-1];

    for(i=0;i<j;i++){
        if((c1&const_char[i])>0&&(c2&const_char[i]==0)){
            return 1;
        }
    }
    return 0;
}

int restore_bitmap(){
    int fd;
    char bitmapfile[64];

    if((bitmap!=NULL)||(file_name==NULL)){
        return -1; 
    }

    sprintf(bitmapfile,"%dbitmap",pieces_length);
    fd=open(bitmapfile,O_RDWR|O_CREAT|O_TRUNC,0666);
    
    if(fd<0){
        return -1;
    }

    write(fd,bitmap->bitfield,bitmap->bitfield_length);
    close(fd);
    return 0;
}

int get_download_piece_num(){
    if(bitmap==NULL){
        return 0;
    }    
    
    unsigned char const_char[8]={0x80,0x40,0x20,0x10,
                                 0x08,0x04,0x02,0x01};
    int i=0,j=0;
    for(i=0;i<bitmap->bitfield_length-1;i++){
        for(j=0;j<8;j++){
            unsigned char c1=(bitmap->bitfield)[i] & const_char[j];
            if(c1>0){
                download_piece_num++;
            }
        }
    }

    return download_piece_num;
}

