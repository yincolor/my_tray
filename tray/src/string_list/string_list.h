#ifndef __STRING_LIST_HEADER_
#define __STRING_LIST_HEADER_
#include<stdlib.h>
#include<stdio.h>
#include<string.h>

/** 检查字符串是否已存在于目标数组中 */
int string_list_str_is_duplicate(char **str_list, char *str) ; 

/**
 * 字符串数组去重
 * @param str_list 期待获得一个结尾char *为一个NULL的的char *数组，比如：char *str_list[] = {"apple", "banana", "apple", "orange", "banana", NULL};
 */
char **string_list_deduplicate(char **str_list, size_t *new_list_num);

/** 打印字符串 */
void string_list_print(char **str_list);

/** 字符串复制，会创建一个和源字符串相同字节的空间，然后复制内容返回，可以free */
char *string_copy(char *src_str);

#endif