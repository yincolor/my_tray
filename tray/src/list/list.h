#ifndef __CUSTOMER_LIST_HEADER__
#define __CUSTOMER_LIST_HEADER__
#include<stdlib.h>
#include<stdio.h>

struct list_node { 
    struct list_node *prev;
    struct list_node *next;

    size_t dataBytes; // 字节长度
    void *data; // 字节数据

    // 对象的调用函数

    void (*OnListNodeDestroyCall)(void *data, size_t data_bytes); // 当节点销毁前的回调函数，可以用于处理数据的销毁
    void (*destroy)(struct list_node *self); // 销毁节点本身
};

static struct list_node *list_node_new(void *data, size_t data_bytes, void (*on_node_destroy_call)(void* data, size_t data_bytes));
static void list_node_destroy(struct list_node *node);

struct list{
    struct list_node *headNode;
    struct list_node *endNode;

    // 对象的调用函数
    size_t (*length)(struct list *self);
    int (*insert)(struct list *self, size_t index, void *data, size_t data_bytes, void (*on_item_remove_call)(void *data, size_t data_bytes));
    int (*add)(struct list *self, void *data, size_t data_bytes, void (*on_item_remove_call)(void *data, size_t data_bytes));
    int (*remove)(struct list *list, size_t index); 
    void *(*getItem)(struct list *self, size_t index, size_t *return_data_bytes); 
    void (*destroy)(struct list *self);

    void (*printInfo)(struct list *self);
    void (*sortBy)(struct list *list, int(*compare_function)(void *data1, size_t data1_bytes, void *data2, size_t data2_byte));
    // struct list_node *(*getNodeByIndex)(struct list *list, size_t index);
};

struct list *list_new();
// 列表获取列表长度，也就是元素数量
static size_t list_get_length(struct list *list);
// 从列表中插入一个元素
static int list_insert_item(struct list *list, size_t index, void *data, size_t data_bytes, void (*on_item_remove_call)(void *data, size_t data_bytes));
// 从列表末尾追加一个元素
static int list_add_item(struct list *list, void *data, size_t data_bytes, void (*on_item_remove_call)(void *data, size_t data_bytes));
// 从列表中查询索引，获取对应的数据
static void *list_get_item_by_index(struct list *list, size_t index, size_t *return_data_bytes);
// 从列表中获取对应索引的
static struct list_node *list_get_node_by_index(struct list *list, size_t index);
// 根据列表索引删除元素
static int list_remove_item_by_index(struct list *list, size_t index);
// 删除列表
static void list_destroy(struct list *list);
// 打印列表信息
static void list_print_info(struct list *list);

/**
 * 根据比较函数进行排序
 * 当比较函数返回负数，则data1 小于 data2，顺序不变
 * 当比较函数返回0，   则data1 等于 data2，顺序不变
 * 当比较函数返回正数，则data1 大于 data2，二者交换位置
 * 
 * @param compare_function 比较函数，当不传入比较函数时，不排序，直接跳过函数被执行
 */
static void list_sort_by(struct list *list, int(*compare_function)(void *data1, size_t data1_bytes, void *data2, size_t data2_bytes));

#endif
