/*************************************************************************
	> File Name: bitfield.h
	> Author: eli
	> Mail: 1525843015@qq.com
	> Created Time: 2017年12月20日 星期三 10时05分54秒
 ************************************************************************/

#ifndef _BITFIELD_H
#define _BITFIELD_H
typedef struct _Bitmap{
    unsigned char *bitfield;
    int bitfield_length;
    int valid_length;
} Bitmap;

//malloc memory and init
int create_bitfield();

int get_bit_value(Bitmap *bitmap,int index);
int set_bit_value(Bitmap *bitmap,int index,unsigned char value);
int all_zero(Bitmap *bitmap);

int all_set(Bitmap *bitmap);
void release_memory_in_bitfield();
int restore_bitmap();//save bitmap to disk
int is_interested(Bitmap *dst,Bitmap *src);
//get has downloaded piece
int get_download_piece_num();
#endif
