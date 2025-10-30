#include "string_list.h"

int string_list_str_is_duplicate(char **str_list, char *str) {
    for (int i = 0; str_list[i] != NULL; i++) {
        if (strcmp(str_list[i], str) == 0) {
            return 0;
        }
    }
    return -1;
}

char **string_list_deduplicate(char **str_list, size_t *new_list_num){
    // printf("===================== string_list_deduplicate =======================\n"); 
    size_t str_num = 0;
    char **str_ptr = str_list; 
    // printf("[string_list_deduplicate] 待去重的字符串：\n"); 
    while (*str_ptr != NULL)
    {
        // printf("%s\n", *str_ptr);
        *str_ptr++; 
        str_num++;
    }
    // printf("[string_list_deduplicate] 待去重的字符串的数量：%ld\n", str_num); 
    if(str_num <=0){
        printf("[string_list_deduplicate] ERROR: 空的字符串数组，不需要去重\n"); 
        return NULL;
    }
    char **copy_str_list = malloc(sizeof(char *) * (str_num + 1)) ; // 一个存储复制的去重后的字符串列表
    for(int i=0;i<str_num+1;i++){
        copy_str_list[i] = NULL; // 设置初始值为NULL
    }
    copy_str_list[0] = string_copy(str_list[0]); // 第一个元素复制
    size_t copy_str_num = 1; // 复制去重字符串后的字符串数量
    for(int i=1;i<str_num;i++){
        int check = string_list_str_is_duplicate(copy_str_list, str_list[i]);
        if(check < 0){
            copy_str_list[i] = string_copy(str_list[i]);
            copy_str_num++; 
        }
    }

    // printf("[string_list_deduplicate] 去重后的字符串数组：\n");
    // for(int i=0;i<str_num+1;i++){
    //     if(copy_str_list[i] == NULL){
    //         printf("null\n");
    //         continue;
    //     }
    //     printf("%s\n", copy_str_list[i]);
    // }
    // printf("[string_list_deduplicate] 去重后，字符串有%ld个\n", copy_str_num); 
    *new_list_num = copy_str_num;

    // printf("=====================================================================\n"); 
    return copy_str_list; 
}

void string_list_print(char **str_list){
    char **str_ptr = str_list; 
    // printf("[string_list_deduplicate] 待去重的字符串：\n"); 
    while (*str_ptr != NULL)
    {
        printf("%s\n", *str_ptr);
        *str_ptr++; 
    }
}

char *string_copy(char *src_str){
    size_t str_len = strlen(src_str);
    if(str_len <= 0){
        return NULL;
    }
    char *dest_str = (char *)malloc(sizeof(char) * (str_len+1));
    strcpy(dest_str, src_str);
    return dest_str; 
}


