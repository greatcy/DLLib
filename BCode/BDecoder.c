/*************************************************************************
	> File Name: BDecoder.c
	> Author: eli
	> Mail: 1525843015@qq.com
	> Created Time: 2018年01月19日 星期五 18时15分48秒
 ************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define TAB_SPACE "\t"
#define TAG_LIST "<LIST>\n"
#define TAG_LIST_D "</LIST>\n"
#define TAG_DICTOINARY "<DICTIONARY>\n"
#define TAG_DICTOINARY_D "</DICTIONARY>\n"
#define TAG_DICTOINARY_KEY "<KEY>\n"
#define TAG_DICTOINARY_KEY_D "</KEY>\n"
#define TAG_DICTOINARY_VALUE "<VALUE>\n"
#define TAG_DICTOINARY_VALUE_D "</VALUE>\n"

unsigned short tab_cursor;
unsigned long long file_cursor;
char *file_content;

int print_array();
int print_dictionary();

void print_tab(){/*{{{*/
    int j;
    for(j=0;j<tab_cursor;j++){
        printf(TAB_SPACE);
    }
}/*}}}*/

int print_int(){/*{{{*/
    if(file_content[file_cursor]!='i'){
        printf("%s:%d\n",__FILE__,__LINE__);
        return -1;
    }

    print_tab();

    file_cursor++;

    long value;
    while(file_content[file_cursor]!='e'){
        value=value*10+(file_content[file_cursor]-'0');
        file_cursor++;
    }

    file_cursor++;
    printf("%ld\n",value);
    
    return 0;
}/*}}}*/

int print_str(){/*{{{*/
    if(!isdigit(file_content[file_cursor])){
        printf("%s:%d\n",__FILE__,__LINE__);
        return -1;
    }

    print_tab();

    int len;
    len=0;
    while(file_content[file_cursor]!=':'){
        len=len*10+(file_content[file_cursor]-'0');
        file_cursor++;
    }

    char *buf=malloc(len);

    if(buf==NULL){
        printf("%s:%d\n",__FILE__,__LINE__);
        return -1;
    }

    //skip ':'
    file_cursor++;

    memcpy(buf,&file_content[file_cursor],len);
    file_cursor+=len;

    printf("%s\n",buf);

    free(buf);
    return 0;
}/*}}}*/

int print_array(){/*{{{*/
    if(file_content[file_cursor]!='l'){
        printf("%s:%d\n",__FILE__,__LINE__);
        return -1;
    }
    
    print_tab();
    printf(TAG_LIST);
    file_cursor++;
    tab_cursor++;
    while(file_content[file_cursor]!='e'){
        switch(file_content[file_cursor]){
            case 'l':
                print_array(&file_content[file_cursor]);
                break;
            case 'd':
                print_dictionary(&file_content[file_cursor]);
                break;
            case 'i':
                print_int(&file_content[file_cursor]);
                break;
        }
        if(isdigit(file_content[file_cursor])){
           print_str(&file_content[file_cursor]); 
        }
    }

    tab_cursor--;
    print_tab();
    printf(TAG_LIST_D);
    file_cursor++;
    return 0;
}/*}}}*/

int print_dictionary(){/*{{{*/
    if(file_content[file_cursor]!='d'){
        printf("%s:%d\n",__FILE__,__LINE__);
        return -1;
    }

    printf(TAG_DICTOINARY);
    tab_cursor++;
    file_cursor++;
    
    while(file_content[file_cursor]!='e'){
        //print KEY
        print_tab();
        printf(TAG_DICTOINARY_KEY);
        if(isdigit(file_content[file_cursor])){
            tab_cursor++;
            print_str(&file_content[file_cursor]); 
            tab_cursor--;
        }
        else{
            printf("%s:%d\n",__FILE__,__LINE__);
            return -1;
        }
        print_tab();
        printf(TAG_DICTOINARY_KEY_D);

        //print value
        print_tab();
        printf(TAG_DICTOINARY_VALUE);
        tab_cursor++;
        if(isdigit(file_content[file_cursor])){
           print_str(&file_content[file_cursor]); 
        }
        else{
            switch(file_content[file_cursor]){
                case 'l':
                    print_array(&file_content[file_cursor]);
                    break;
                case 'd':
                    print_dictionary(&file_content[file_cursor]);
                    break;
                case 'i':
                    print_int(&file_content[file_cursor]);
                    break;
            }
        }
        tab_cursor--;
        print_tab();
        printf(TAG_DICTOINARY_VALUE_D);
    }

    printf(TAG_DICTOINARY_D);
    tab_cursor--;
    file_cursor++;
    return 0;
}/*}}}*/

int main(int argc,char **argv){/*{{{*/
    if(argc!=2){
        printf("usage %s bt file path\n",argv[0]);
        return -1;
    }

    tab_cursor=0;

    FILE *file = fopen(argv[1],"r");

    if(file==NULL){
        printf("%s:%d\n",__FILE__,__LINE__);
        return -1;
    }

    fseek(file,0,SEEK_END);
    int file_size=ftell(file);
    fseek(file,0,SEEK_SET);
    if(file_size==0){
        printf("%s:%d\n",__FILE__,__LINE__);
        return -1;
    }

    file_content=malloc(file_size+1);
    int i;
    for(i=0;i<file_size;i++){
        file_content[i]=fgetc(file);
    }
    file_content[file_size]='\0';

    i=0; 
    for(;i<file_size;i=file_cursor){
        switch(file_content[file_cursor]){
            case 'l':
                print_array();
                break;
            case 'd':
                print_dictionary();
                break;
            case 'i':
                print_int();
                break;
        }
        if(isdigit(file_content[file_cursor])){
           print_str(); 
        }
    }

    free(file_content);
    return 0;
}/*}}}*/
