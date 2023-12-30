#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/* Compare two string */
int cmp(char *a, char *b, bool descend)
{
    if (descend)
        return (strcmp(b, a));
    else
        return (strcmp(a, b));
}

/* Merge two link list and return new head */
struct list_head *merge(struct list_head *a, struct list_head *b, bool descend)
{
    struct list_head *head = NULL, **tail = &head;
    while (1) {
        element_t *ele_a, *ele_b;
        ele_a = list_entry(a, element_t, list);
        ele_b = list_entry(b, element_t, list);
        if (cmp(ele_b->value, ele_a->value, descend) <= 0) {
            *tail = b;
            tail = &b->next;
            b = b->next;
            if (!b) {
                *tail = a;
                break;
            }
        } else {
            *tail = a;
            tail = &a->next;
            a = a->next;
            if (!a) {
                *tail = b;
                break;
            }
        }
    }
    return head;
}

/* Merge final two link list and insert to old head*/
void merge_final(struct list_head *a,
                 struct list_head *b,
                 struct list_head *head,
                 bool descend)
{
    struct list_head *tail = head;
    while (1) {
        element_t *ele_a, *ele_b;
        ele_a = list_entry(a, element_t, list);
        ele_b = list_entry(b, element_t, list);
        if (cmp(ele_b->value, ele_a->value, descend) <= 0) {
            tail->next = b;
            b->prev = tail;
            tail = b;
            b = b->next;
            if (!b) {
                break;
            }
        } else {
            tail->next = a;
            a->prev = tail;
            tail = a;
            a = a->next;

            if (!a) {
                a = b;
                break;
            }
        }
    }
    tail->next = a;
    while (a != NULL) {
        a->prev = tail;
        tail = a;
        a = a->next;
    }
    tail->next = head;
    head->prev = tail;
    return;
}

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

    element_t *node = malloc(sizeof(element_t));
    if (!node)
        return false;

    node->value = malloc((strlen(s) + 1) * sizeof(char));
    if (!node->value) {
        free(node);
        return false;
    }
    memcpy(node->value, s, strlen(s) + 1);
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

    node->value = malloc((strlen(s) + 1) * sizeof(char));
    if (!node->value) {
        free(node);
        return false;
    }
    memcpy(node->value, s, strlen(s) + 1);
    list_add_tail(&node->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *ele = list_first_entry(head, element_t, list);
    list_del_init(&ele->list);
    if (sp) {
        strncpy(sp, ele->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return ele;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *ele = list_last_entry(head, element_t, list);
    list_del_init(&ele->list);
    if (sp) {
        strncpy(sp, ele->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
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
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    struct list_head *node = head->next, *next = node->next;
    for (; next != head && node != head;) {
        list_move(node, next);
        node = node->next;
        next = node->next;
    }
    return;
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    struct list_head *node, *safe;
    list_for_each_safe (node, safe, head)
        list_move(node, head);
    return;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    if (!head || list_empty(head) || list_is_singular(head) || k <= 1)
        return;

    struct list_head *swap, *safe, *tail, *tmp_h = head;
    int length = q_size(head) / k;

    for (int i = 0; i < length; i++) {
        tail = tmp_h->next;
        swap = tail;
        for (int j = 0; j < k - 1; j++)
            swap = swap->next;
        safe = swap->prev;
        for (; tmp_h != swap && tmp_h != safe;) {
            list_move(swap, tmp_h);
            tmp_h = tmp_h->next;
            swap = safe;
            safe = safe->prev;
        }
        tmp_h = tail;
    }
    return;
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    size_t count = 0;
    head->prev->next = NULL;
    struct list_head *pending = NULL, *list = head->next;
    do {
        size_t bits;
        struct list_head **tail = &pending;
        for (bits = count; bits & 1; bits = bits >> 1)
            tail = &(*tail)->prev;
        if (bits > 0) {
            struct list_head *a = *tail, *b = a->prev;
            a = merge(a, b, descend);
            a->prev = b->prev;
            *tail = a;
        }
        list->prev = pending;
        pending = list;
        list = list->next;
        pending->next = NULL;
        count++;
    } while (list);
    list = pending;
    pending = pending->prev;
    while (1) {
        struct list_head *next = pending->prev;
        if (!next)
            break;
        list = merge(list, pending, descend);
        pending = next;
    }
    merge_final(list, pending, head, descend);
    return;
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;

    element_t *node, *prev;
    node = list_entry(head->prev, element_t, list);
    prev = list_entry(node->list.prev, element_t, list);
    int ret = 1;
    for (; &prev->list != head;) {
        if (strcmp(prev->value, node->value) >= 0) {
            list_del(&prev->list);
            q_release_element(prev);
            prev = list_entry(node->list.prev, element_t, list);
        } else {
            ret++;
            node = prev;
            prev = list_entry(prev->list.prev, element_t, list);
        }
    }
    return ret;
}

/* Remove every node which has a node with a strictly greater value anywhere
 * to the right side of it */
int q_descend(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;

    element_t *node, *prev;
    node = list_entry(head->prev, element_t, list);
    prev = list_entry(node->list.prev, element_t, list);
    int ret = 1;
    for (; &prev->list != head;) {
        if (strcmp(prev->value, node->value) <= 0) {
            list_del(&prev->list);
            q_release_element(prev);
            prev = list_entry(node->list.prev, element_t, list);
        } else {
            ret++;
            node = prev;
            prev = list_entry(prev->list.prev, element_t, list);
        }
    }
    return ret;
}

/* Merge all the queues into one sorted queue, which is in
 * ascending/descending order */
int q_merge(struct list_head *head, bool descend)
{
    if (!head || list_empty(head))
        return 0;

    if (list_is_singular(head)) {
        return list_entry(head->next, queue_contex_t, chain)->size;
    }
    struct list_head *node;
    struct list_head *tmp_head =
        list_entry(head->next, queue_contex_t, chain)->q;
    int ret = list_entry(head->next, queue_contex_t, chain)->size;
    for (node = (head)->next->next; node != (head); node = node->next) {
        queue_contex_t *entry = list_entry(node, queue_contex_t, chain);
        ret += entry->size;
        entry->size = 0;
        list_splice_init(entry->q, tmp_head);
    }

    q_sort(tmp_head, descend);
    list_entry(head->next, queue_contex_t, chain)->size = ret;
    return ret;
}
