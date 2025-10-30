#include "../tray/src/cJSON/cJSON.h"
#include<stdio.h>
#include<stdlib.h>

char *_str_read_from_file(char *file_path){
    FILE *f = fopen(file_path, "r");
    if(f == NULL){
        printf("无法打开文件\n");
        return NULL;
    }    
    fseek(f, 0, SEEK_END);
    long file_size = ftell(f);
    fseek(f, 0, SEEK_SET); 
    char *str = (char *)malloc(file_size+1);
    fread(str, file_size, 1, f);
    fclose(f);
    f = NULL; 
    str[file_size] = '\0';
    return str; 
}
void _str_delete(char *str){
    free(str); 
    str = NULL; 
}

int main(int argc, char const *argv[])
{
    FILE *f = fopen("./tray_menu_data.json", "r");
    if(f == NULL){
        printf("无法打开文件\n");
        return -1;
    }    
    fseek(f, 0, SEEK_END);
    long file_size = ftell(f);
    fseek(f, 0, SEEK_SET); 
    char *str = (char *)malloc(file_size+1);
    fread(str, file_size, 1, f);
    fclose(f);
    str[file_size] = '\0';
    printf("%s\n结束", str);
    free(str);
    return 0;
}
