#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "cpucycles.h"
#include <time.h>
#include <string.h>

typedef struct __node {
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


node_t *find_median_of_three(node_t *a, node_t *b, node_t *c) {
    if ((a->value > b->value && a->value < c->value) || (a->value > c->value && a->value < b->value)) {
        return a;
    } else if ((b->value > a->value && b->value < c->value) || (b->value > c->value && b->value < a->value)) {
        return b;
    } else {
        return c;
    }
}

/**
 * https://github.com/torvalds/linux/blob/master/include/linux/list.h
 * list_replace - replace old entry by new one
 * @old : the element to be replaced
 * @new : the new element to insert
 *
 * If @old was empty, it will be overwritten.
 */
static inline void list_replace(struct list_head *old, struct list_head *new)
{
    new->next = old->next;
    new->next->prev = new;
    new->prev = old->prev;
    new->prev->next = new;
}

/**
 * https://github.com/torvalds/linux/blob/master/include/linux/list.h
 * list_swap - replace entry1 with entry2 and re-add entry1 at entry2's position
 * @entry1: the location to place entry2
 * @entry2: the location to place entry1
 */
static inline void list_swap(struct list_head *entry1, struct list_head *entry2)
{
    struct list_head *pos = entry2->prev;

    list_del(entry2);
    list_replace(entry1, entry2);
    if (pos == entry1)
        pos = entry2;
    list_add(entry1, pos);
}


struct list_head* swap_nodes(node_t *node1, node_t *node2, struct list_head *head) {
    if (node1 == node2) return head;
    
    list_swap(&node1->list,&node2->list);
    // If node1 or node2 was the head, update head
    if (&node1->list == head) {
        head = &node2->list;
    } else if (&node2->list == head) {
        head = &node1->list;
    }

    return head;
}

//參照 快慢指標作法
// https://hackmd.io/@sysprog/ry8NwAMvT
node_t *get_middle_node(struct list_head *head) {
    struct list_head *slow = head->next;
    struct list_head *fast = head->next;

    while (fast != head && fast->next != head) {
        slow = slow->next;
        fast = fast->next->next;
    }

    return list_entry(slow, node_t, list);
}

struct list_head *median_of_three(struct list_head *head)
{
    //type36  若輸入的link list節點小於3個，則直接回傳原節點，不處裡
    if(list_length(head)<3)
        return head;
    
    node_t *low_node = list_first_entry(head, node_t, list);   
    node_t *middle_node =get_middle_node(head);
    node_t *high_node = list_last_entry(head, node_t, list);
	//(2) 比較 A[low_node]、A[middle_node] 與 A[high_node] 這三筆資料，排出中間值。
	//c. 將此中間值再與 A[left] 做交換
	//(3) 讓現在新的 A[left] 作為 pivot

    node_t *middle_value_node=find_median_of_three(low_node,middle_node,high_node);

    // 交換中間值節點與頭節點
    head=swap_nodes(low_node, middle_value_node,head);

    return head;		
}


node_t *random_pivot(struct list_head *head) {
    /*  step1. 用亂數任選三個，
        step2. 再用其中的中間值作為 pivot。
    */
    int random_n[3],list_len=list_length(head);
    memset(random_n,0x00,sizeof(random_n));
    if(list_len<=2)
    {   // 若 link list 長度不足3個時，則從2個節點隨機取樣
        node_t *nodes;
        struct list_head *pos_tmp;
        int random_n=rand()%list_len;
        list_for_each(pos_tmp, head) {
            if (random_n == 0)
            {
                nodes= list_entry(pos_tmp, node_t, list);
                return nodes;
            } 
            random_n--;
        }
    }

    for(int i=0;i<3;i++){
        random_n[i]=rand()%list_len;
        if(i==1 && random_n[0]==random_n[1]){
            i--;
            continue;
        }
            
        if(i==2 && (random_n[0]==random_n[2] || random_n[1]==random_n[2])){
            i--;
            continue;
        }
  
    }

    node_t *nodes[3] = {NULL, NULL, NULL};
    struct list_head *pos;
    int count = 0;
    list_for_each(pos, head) {
        if (count == random_n[0]) 
            nodes[0] = list_entry(pos, node_t, list);
        if (count == random_n[1]) 
            nodes[1] = list_entry(pos, node_t, list);
        if (count == random_n[2]) 
            nodes[2] = list_entry(pos, node_t, list);
        count++;
    }

    // Find the median of the three nodes
    return find_median_of_three(nodes[0], nodes[1], nodes[2]);
}



void quick_sort_median_of_three(struct list_head **head)
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
            
            begin[i]=median_of_three(begin[i]); // 在這行加入 median_of_three（begin[i]）預先處裡 link list
            
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


void quick_sort_random_pivot(struct list_head **head)
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
            
            // 這三行是 做 random pivot
            /*
              step0. 先準備好 begin[i]的 head 節點，準備之後做交換用==> *head_node 
              step1. 用亂數任選三個，再用其中的中間值作為 pivot。
              step2. 再用其中的中間值作為 pivot。
              step1~step2 就是 random_pivot_node=random_pivot(begin[i]); 在處裡
              最後產生 random_pivot_node
              step4. swap_nodes(begin[i], head_node, random_pivot_node);
              代表 begin[i] link list 的 pivot 要改成 random_pivot_node做排序
            */

            node_t *head_node = list_first_entry(begin[i], node_t, list);
            node_t *random_pivot_node=random_pivot(begin[i]);
            begin[i]=swap_nodes(head_node, random_pivot_node,begin[i]);

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

//參考  vax-r  同學寫法
// https://hackmd.io/@vax-r/linux2024-homework2#%E6%B8%AC%E9%A9%97%E4%B8%80
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
    for (int i = 10; i <= 2000; i++) {
        unsigned long long time_non_sort = 0, time_sort= 0;
        unsigned long long time_median_of_three_sort = 0, time_random_pivot_sort= 0;
        struct timespec t1, t2;

        struct list_head *q_sort_list = list_new();
        struct list_head *q_sort_median_of_three_list = list_new();
        struct list_head *q_sort_random_pivot_list = list_new();

        size_t count = i, data_number=i;

        int *test_arr = malloc(sizeof(int) * count);

        for (int i = 0; i < count; ++i)
            test_arr[i] = i;
        shuffle(test_arr, count);

        while (count--){
            q_sort_list = list_construct(q_sort_list, test_arr[count]);
            q_sort_median_of_three_list = list_construct(q_sort_median_of_three_list, test_arr[count]);
            q_sort_random_pivot_list = list_construct(q_sort_random_pivot_list, test_arr[count]);
        }
            

        
        clock_gettime(CLOCK_MONOTONIC, &t1); //撮記時間t1
        quick_sort(&q_sort_list);//真正跑分析的函式
        clock_gettime(CLOCK_MONOTONIC, &t2); //撮記時間t2
        time_non_sort += (unsigned long long) (t2.tv_sec * 1000000000 + t2.tv_nsec) -
                        (t1.tv_sec * 1000000000 + t1.tv_nsec);//轉成 nanosecond    
          
        assert(list_is_ordered(q_sort_list));
 
        

        clock_gettime(CLOCK_MONOTONIC, &t1); //撮記時間t1
        quick_sort_median_of_three(&q_sort_median_of_three_list);//真正跑分析的函式
        clock_gettime(CLOCK_MONOTONIC, &t2); //撮記時間t2
        time_median_of_three_sort += (unsigned long long) (t2.tv_sec * 1000000000 + t2.tv_nsec) -
                        (t1.tv_sec * 1000000000 + t1.tv_nsec);//轉成 nanosecond   
        
        assert(list_is_ordered(q_sort_median_of_three_list));

        clock_gettime(CLOCK_MONOTONIC, &t1); //撮記時間t1
        quick_sort_random_pivot(&q_sort_random_pivot_list);//真正跑分析的函式
        clock_gettime(CLOCK_MONOTONIC, &t2); //撮記時間t2
        time_random_pivot_sort += (unsigned long long) (t2.tv_sec * 1000000000 + t2.tv_nsec) -
                        (t1.tv_sec * 1000000000 + t1.tv_nsec);//轉成 nanosecond                    
        
        assert(list_is_ordered(q_sort_random_pivot_list));
        
        printf("%ld,%llu,%llu,%llu\n", data_number, time_non_sort,time_median_of_three_sort,time_random_pivot_sort); 
        
        list_free(q_sort_list);
        list_free(q_sort_median_of_three_list);
        list_free(q_sort_random_pivot_list);

        free(test_arr);

    }//end for (int i = 10; i <= 1000; i++)
    return 0;
}
