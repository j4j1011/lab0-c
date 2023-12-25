#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = test_malloc(sizeof(struct list_head));

    if (!head)
        return NULL;

    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;
    element_t *entry, *safe;
    list_for_each_entry_safe (entry, safe, l, list) {
        q_release_element(entry);
    }

    test_free(l);
    return;
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;

    element_t *node = test_malloc(sizeof(element_t));

    if (!node)
        return false;

    char *val = test_malloc((strlen(s) + 1) * sizeof(char));

    if (!val) {
        test_free(node);
        return false;
    }

    strncpy(val, s, strlen(s) + 1);
    node->value = val;
    list_add(&node->list, head);

    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;

    element_t *node = malloc(sizeof(element_t));

    if (!node)
        return false;

    node->value = malloc(sizeof(char) * (strlen(s) + 1));

    if (!node->value) {
        test_free(node);
        return false;
    }

    strncpy(node->value, s, strlen(s) + 1);
    list_add_tail(&node->list, head);

    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (list_empty(head))
        return NULL;

    element_t *ele = list_first_entry(head, element_t, list);
    list_del_init(&ele->list);

    strncpy(sp, ele->value, bufsize - 1);
    sp[bufsize - 1] = '\0';
    return ele;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (list_empty(head))
        return NULL;

    element_t *ele = list_last_entry(head, element_t, list);
    list_del_init(&ele->list);

    strncpy(sp, ele->value, bufsize - 1);
    sp[bufsize - 1] = '\0';
    return ele;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    struct list_head *node;
    int i = 0;

    list_for_each (node, head)
        i++;

    return i;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;

    struct list_head *slow, *fast;
    slow = fast = head->next;
    for (; fast != head && fast->next != head; fast = fast->next->next)
        slow = slow->next;

    element_t *ele = list_entry(slow, element_t, list);
    list_del(&ele->list);
    q_release_element(ele);
    return true;
}

/* Delete all nodes that have duplicate string */

bool q_delete_dup(struct list_head *head)
{
    if (!head)
        return false;

    bool del = false;
    element_t *cur, *safe;
    list_for_each_entry_safe (cur, safe, head, list) {
        if (&safe->list != head && !strcmp(safe->value, cur->value)) {
            list_del_init(&cur->list);
            q_release_element(cur);
            del = true;
        } else if (del) {
            list_del_init(&cur->list);
            q_release_element(cur);
            del = false;
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    struct list_head **indir = &head;

    do {
        struct list_head *tmp = *indir;
        *indir = (*indir)->next;
        tmp->next = (*indir)->next;
        (*indir)->next = tmp;
        indir = &(*indir)->next->next;
    } while (*indir != head);
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head) {}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend) {}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Remove every node which has a node with a strictly greater value anywhere
 * to the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in
 * ascending/descending order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
