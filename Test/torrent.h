/*************************************************************************
	> File Name: torrent.h
	> Author: eli
	> Mail: 1525843015@qq.com
	> Created Time: 2018年01月03日 星期三 10时53分45秒
 ************************************************************************/

#ifndef _TORRENT_H
#define _TORRENT_H

#include "tracker.h"

int download_upload_with_peers();
int print_peer_list();
void print_process_info();
void clear_connect_trcker();
void clear_connect_peer();
void clear_tracker_response();
void release_memory_in_torrent();

#endif
