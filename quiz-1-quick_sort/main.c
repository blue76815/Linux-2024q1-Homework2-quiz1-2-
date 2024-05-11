#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "cpucycles.h"
#include <time.h>

typedef struct __node {
    //struct __node *left, *right;
    //struct __node *next;
    struct list_head list;
    long value;
} node_t;

struct list_head *list_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));

    if (!head)
        return NULL;

    INIT_LIST_HEAD(head);
    return head;
}


int list_length(struct list_head *head)
{
    int n = 0;
    struct list_head *p = NULL;
    list_for_each(p, head)
        n++;

    return n;
}
struct list_head *list_construct(struct list_head *head, int n)
{
    node_t *node = malloc(sizeof(node_t));
    node->value = n;
    list_add(&node->list, head);
    return head;
}


void list_print(struct list_head *head)
{
    if (head== NULL)
        return;
    node_t *node = NULL; 
    list_for_each_entry(node, head,list){
        printf("%ld ",node->value);
    }
    printf("\r\n");
    return;
}

void list_free(struct list_head *l)
{
	if (l== NULL)
        return;

    node_t *node = NULL, *tmp_node = NULL;
    list_for_each_entry_safe (node, tmp_node, l, list) {
        list_del(&node->list);
        free(node);
    }
    free(l);
    return;
}

static bool list_is_ordered(struct list_head *head)
{       
    node_t *node = NULL, *tmp_node = NULL;
    list_for_each_entry_safe (node, tmp_node, head, list) {
        if (&tmp_node->list == head)
            break;
        if (node->value > tmp_node->value)
            return false;
    }
    return true;
}


struct list_head *list_tail(struct list_head *head) {
    if (head->prev == head)
        return NULL;
    return head->prev;
}

node_t *list_remove_head(struct list_head *head)
{
    if (!head || list_empty(head))
        return NULL;

    node_t *f = list_first_entry(head, node_t, list);
    list_del(&f->list);
    return f;

}

void quick_sort(struct list_head **head)
{
    int n = list_length(*head);
    int pivot_value;
    int i = 0;
    int max_level = 2 * n;
    struct list_head  *begin[max_level];
    begin[0] = *head;
    for (int i = 1; i < max_level; i++)
        begin[i] = list_new();   

    struct list_head  *result = list_new(), *left = list_new(), *right = list_new();//OK
        
    while (i >= 0) {
        struct list_head *L = begin[i]->next, *R=begin[i]->prev;
        if (L!=R) {
			node_t *pivot = list_remove_head(begin[i]);
			pivot_value = pivot->value;

            node_t *entry, *safe;
            list_for_each_entry_safe(entry, safe, begin[i], list){
                list_del(&entry->list);
                list_add(&entry->list,entry->value > pivot_value? right:left);
            }                             	
            list_splice_init(left, begin[i]);
            list_add(&pivot->list, begin[i + 1]);
            list_splice_init(right, begin[i + 2]);

            i += 2;
        } else {
            if (list_is_singular(begin[i]))
                list_splice_init(begin[i], result);

            i--;
        }
    }

    for (int i = 0; i < max_level; i++)
        list_free(begin[i]);
    
    list_free(left);
    list_free(right);

	*head = result;
}

/* shuffle array, only work if n < RAND_MAX */
void shuffle(int *array, size_t n)
{
    if (n <= 0)
        return;

    for (size_t i = 0; i < n - 1; i++) {
        size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
        int t = array[j];
        array[j] = array[i];
        array[i] = t;
    }
}

int main(int argc, char **argv)
{
    //for (int i = 10; i <= 100000; i++) {
    for (int i = 10; i <= 1000; i++) {
    unsigned long long time_non_sort = 0, time_sort= 0;
    struct timespec t1, t2;

    struct list_head *list = list_new();
    size_t count = i, data_number=i;

    int *test_arr = malloc(sizeof(int) * count);

    for (int i = 0; i < count; ++i)
        test_arr[i] = i;
    shuffle(test_arr, count);

    while (count--)
        list = list_construct(list, test_arr[count]);

    
    clock_gettime(CLOCK_MONOTONIC, &t1); //撮記時間t1
    quick_sort(&list);//真正跑分析的函式

    clock_gettime(CLOCK_MONOTONIC, &t2); //撮記時間t2
    time_non_sort += (unsigned long long) (t2.tv_sec * 1000000000 + t2.tv_nsec) -
                     (t1.tv_sec * 1000000000 + t1.tv_nsec);//轉成 nanosecond    
    assert(list_is_ordered(list));
    clock_gettime(CLOCK_MONOTONIC, &t1); //撮記時間t1
    quick_sort(&list);//真正跑分析的函式
    clock_gettime(CLOCK_MONOTONIC, &t2); //撮記時間t2
    time_sort += (unsigned long long) (t2.tv_sec * 1000000000 + t2.tv_nsec) -
                     (t1.tv_sec * 1000000000 + t1.tv_nsec);//轉成 nanosecond    
    assert(list_is_ordered(list));
    printf("%ld,%llu,%llu\n", data_number, time_non_sort, time_sort);   

    list_free(list);

    free(test_arr);

    }
    return 0;
}