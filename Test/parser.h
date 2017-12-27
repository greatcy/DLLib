/*************************************************************************
	> File Name: parser.h
	> Author: eli
	> Mail: 1525843015@qq.com
	> Created Time: 2017年12月18日 星期一 18时24分19秒
 ************************************************************************/

#ifndef _PARSER_H
#define _PARSER_H
typedef struct _Announce_list{
    char annouce[128];
    struct _Announce_list *next;
} Announce_list;

typedef struct _Files{
    char path[256];
    long length;
    struct _Files *next;
} Files;

//read torrent file,get file name ,filesize ,and content
//@param metafile_name 待读取的文件名
int read_metafile(char *metafile_name);
//find keyword in File
int find_keyword(char *keyword,long *position);
//get all tracker server address
//call this method can get a linker annouce_list urls
int read_announce_list();
//add url to tracker
int add_an_announce(char *url);

int get_piece_length();
int get_piece();

//is torrent has multifiles
int is_multi_files();
int get_file_name();
int get_file_length();
int get_files_length_path();//mutiFile call this for length and path 

//caculate info_hash
int get_info_hash();
//peer_id
int get_peer_id();

void release_memory_in_parse_metafile();
int parse_metafile(char *metafile);

#endif
