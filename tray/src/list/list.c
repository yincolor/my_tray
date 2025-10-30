
#include "list.h"

static struct list_node *list_node_new(void *data, size_t data_bytes, void (*on_node_destroy_call)(void *data, size_t data_bytes))
{
    if (data == NULL || data_bytes <= 0)
    {
        return NULL;
    }
    struct list_node *node = (struct list_node *)malloc(sizeof(struct list_node));
    if (node == NULL)
    {
        return NULL;
    }
    node->data = data;
    node->dataBytes = data_bytes;
    node->OnListNodeDestroyCall = on_node_destroy_call;
    node->next = NULL;
    node->prev = NULL;
    node->destroy = list_node_destroy;
    return node;
}
static void list_node_destroy(struct list_node *node)
{
    if (node->OnListNodeDestroyCall != NULL)
    {
        node->OnListNodeDestroyCall(node->data, node->dataBytes);
    }
    node->next = NULL;
    node->prev = NULL;
    free(node);
    node = NULL;
}

struct list *list_new()
{
    struct list *list = (struct list *)malloc(sizeof(struct list));
    if (list == NULL)
    {
        return NULL;
    }
    list->headNode = NULL;
    list->endNode = NULL;

    list->length = list_get_length;
    list->insert = list_insert_item;
    list->add = list_add_item;
    list->remove = list_remove_item_by_index;
    list->getItem = list_get_item_by_index;
    list->destroy = list_destroy;
    list->printInfo = list_print_info;
    list->sortBy = list_sort_by;
    return list;
}

static size_t list_get_length(struct list *list)
{
    if (list->endNode == NULL || list->headNode == NULL)
    {
        list->endNode = NULL;
        list->headNode = NULL;
        return 0;
    }
    struct list_node *cur_node = list->headNode;
    size_t node_num = 1;
    while (1)
    {
        if (cur_node == list->endNode)
        {
            break;
        }
        cur_node = cur_node->next;
        node_num++;
    }
    return node_num;
}

static int list_insert_item(struct list *list, size_t index, void *data, size_t data_bytes, void (*on_item_remove_call)(void *data, size_t data_bytes))
{
    if (data == NULL)
    {
        printf("无法追加空数据为元素，data IS NULL\n");
        return -1;
    }
    size_t len = list_get_length(list);
    // printf("list length = %ld\n", len);
    if (index > len)
    {
        printf("元素插入索引%ld超出列表长度%ld\n", index, len);
        return -2;
    }
    struct list_node *new_node = list_node_new(data, data_bytes, on_item_remove_call);
    if (new_node == NULL)
    {
        printf("列表创建新元素失败\n");
        return -3;
    }

    if (len == 0)
    {
        // printf("插入的元素是列表第一个元素\n");
        list->headNode = new_node;
        list->endNode = new_node;
        return 0;
    }
    if (index == 0)
    {
        // printf("向开头插入元素\n");
        list->headNode->prev = new_node;
        new_node->next = list->headNode;
        new_node->prev = NULL;
        list->headNode = new_node;
    }
    else if (index == len)
    {
        // printf("向末尾插入元素\n");
        list->endNode->next = new_node;
        new_node->prev = list->endNode;
        new_node->next = NULL;
        list->endNode = new_node;
    }
    else
    {
        // printf("向中间插入元素\n");
        struct list_node *old_index_node = list_get_node_by_index(list, index);
        struct list_node *prev_node = old_index_node->prev;
        prev_node->next = new_node;

        new_node->prev = prev_node;
        new_node->next = old_index_node;

        old_index_node->prev = new_node;
    }
    return 0;
}

// 从列表末尾追加一个元素
static int list_add_item(struct list *list, void *data, size_t data_bytes, void (*on_item_remove_call)(void *data, size_t data_bytes))
{
    size_t len = list_get_length(list);
    return list_insert_item(list, len, data, data_bytes, on_item_remove_call);
}

// 从列表中查询索引，获取对应的数据
static void *list_get_item_by_index(struct list *list, size_t index, size_t *return_data_bytes)
{
    struct list_node *node = list_get_node_by_index(list, index);
    *return_data_bytes = node->dataBytes;
    return node->data;
}

static struct list_node *list_get_node_by_index(struct list *list, size_t index)
{
    struct list_node *find_node = list->headNode;
    size_t cur_index = 0;
    while (1)
    {
        if (cur_index == index)
        {
            return find_node;
        }
        find_node = find_node->next;
        cur_index++;
        if (find_node == NULL)
        {
            // printf("查到尾部了\n");
            break;
        }
    }
    return NULL;
}
// 根据列表索引删除元素
static int list_remove_item_by_index(struct list *list, size_t index)
{
    size_t len = list_get_length(list);
    if (index >= len)
    {
        printf("元素删除索引%ld超出列表长度%ld\n", index, len);
        return -2;
    }
    struct list_node *rm_node = list_get_node_by_index(list, index);
    if (len == 1 && index == 0)
    {
        // 要删除的是列表最后一个节点元素
        list->headNode = NULL;
        list->endNode = NULL;
        rm_node->destroy(rm_node);
        return 0;
    }
    struct list_node *prev_node = rm_node->prev;
    struct list_node *next_node = rm_node->next;
    if (index == 0)
    {
        // 删除最开头的节点
        list->headNode = next_node;
        next_node->prev = NULL;
        rm_node->next = NULL;
        rm_node->destroy(rm_node);
    }
    else if (index == len - 1)
    {
        // 删除最末尾的节点
        list->endNode = prev_node;
        prev_node->next = NULL;
        rm_node->prev = NULL;
        rm_node->destroy(rm_node);
    }
    else
    {
        // 删除中间的节点
        prev_node->next = next_node;
        next_node->prev = prev_node;
        rm_node->prev = NULL;
        rm_node->next = NULL;
        rm_node->destroy(rm_node);
    }
    return 0;
}
// 删除列表
static void list_destroy(struct list *list)
{
    size_t len = list_get_length(list);
    size_t cur_index = 0;
    struct list_node *cur_node = list->headNode;
    while (1)
    {
        if (cur_index == len)
        {
            break;
        }
        struct list_node *next_node = cur_node->next;
        cur_node->destroy(cur_node);
        cur_node = next_node;
        cur_index++;
    }
    free(list);
    list = NULL;
}

// 打印列表信息
static void list_print_info(struct list *list)
{
    struct list_node *cur_node = list->headNode;
    printf("==============打印列表信息===============\n");
    printf("list ptr: %p list node: \n", list);
    while (1)
    {
        struct list_node *prev_node = cur_node->prev;
        struct list_node *next_node = cur_node->next;
        if(prev_node == NULL){
            printf("* node ptr: %p\tprev ptr: %p\t\t\tnext ptr: %p \n", cur_node, cur_node->prev, cur_node->next);
        }else{
            printf("* node ptr: %p\tprev ptr: %p\tnext ptr: %p \n", cur_node, cur_node->prev, cur_node->next);
        }
        cur_node = cur_node->next;
        if (cur_node == NULL)
        {
            break;
        }
    }
    printf("list node num: %ld\n", list->length(list));
    printf("=======================================\n");
}

static void list_sort_by(struct list *list, int (*compare_function)(void *data1, size_t data1_bytes, void *data2, size_t data2_bytes))
{
    size_t list_length = list->length(list);
    if (list_length < 2)
    {
        printf("列表元素数量小于2，不需要排序\n");
        return;
    }
    // printf("当前列表长度：%ld\n", list_length);
    for (size_t i = 0; i < list_length - 1; i++)
    {
        for (size_t j = 0; j < list_length - i - 1; j++)
        {
            // printf("index = %ld\n", j);
            struct list_node *left_node = list_get_node_by_index(list, j);
            struct list_node *right_node = left_node->next; //list_get_node_by_index(list, j + 1);
            // printf("比较节点：%p <> %p ", left_node, right_node);
            int compare_state = compare_function(left_node->data, left_node->dataBytes, right_node->data, right_node->dataBytes);
            // printf("比较状态：%d\n", compare_state);
            if (compare_state > 0)
            {
                // 需要将left_node 和 right_node 交换位置

                struct list_node *prev_node = left_node->prev;
                struct list_node *next_node = right_node->next;
                // printf("左边的prev_node: %p 右边的next_node: %p\n", prev_node, next_node); 

                if(list->headNode == left_node){
                    // 左边的节点是列表的第一个节点
                    left_node->next = right_node->next;
                    left_node->prev = right_node;
                    right_node->next = left_node;
                    right_node->prev = NULL;
                    next_node->prev = left_node;
                    list->headNode = right_node; 
                    
                }else if(list->endNode == right_node){
                    // 右边的节点是列表的最后一个节点
                    right_node->prev = left_node->prev;
                    right_node->next = left_node;
                    left_node->prev = right_node;
                    left_node->next = NULL;
                    prev_node->next = right_node; 
                    list->endNode = left_node; 
                }else{
                    // 中间的节点交换
                    right_node->prev = left_node->prev;
                    left_node->next = right_node->next;
                    right_node->next = left_node;
                    left_node->prev = right_node; 
                    prev_node->next = right_node;
                    next_node->prev = left_node; 
                }
                
            }
            list->printInfo(list); 
        }
    }
}