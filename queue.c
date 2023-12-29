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
    struct list_head *q_head = malloc(sizeof(*q_head));
    if (!q_head)  // allocated failed
        return NULL;
    INIT_LIST_HEAD(q_head);
    return q_head;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;
    element_t *c, *n;
    list_for_each_entry_safe (c, n, l, list) {
        list_del(&c->list);
        q_release_element(c);
    }
    free(l);  // q_new functiopn has malloced it
}
/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *new_element = malloc(sizeof(element_t));  // new head element
    if (!new_element)                                    // allocated failed
        return false;
    new_element->value = strdup(s);
    if (!new_element->value) {  // allocated failed
        free(new_element);
        return false;
    }
    list_add(&new_element->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;
    return q_insert_head(head->prev, s);
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *rm_element = list_entry(head->next, element_t, list);
    if (sp) {
        strncpy(sp, rm_element->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    list_del(head->next);
    return rm_element;
}
/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *rm_element = list_last_entry(head, element_t, list);
    if (sp) {
        strncpy(sp, rm_element->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    list_del(head->prev);
    return rm_element;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;
    int cnt = 0;
    struct list_head *n;
    list_for_each (n, head)
        cnt++;

    return cnt;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;
    struct list_head *n = head->next;
    for (struct list_head *p = head->prev; n->next != p && n != p;) {
        n = n->next;
        p = p->prev;
    }
    list_del(n);
    q_release_element(list_entry(n, element_t, list));
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;
    element_t *c, *n;  // current and next element
    bool is_dup = false;
    list_for_each_entry_safe (c, n, head,
                              list) {  // current node (iterator) is allowed to
                                       // be removed from the list.
        if (c->list.next != head &&
            strcmp(c->value, n->value) == 0)  // duplicate string detection
        {
            list_del(&c->list);  // delete node
            q_release_element(c);
            is_dup = true;
        } else if (is_dup) {
            list_del(&c->list);
            q_release_element(c);
            is_dup = false;
        }
    }

    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    struct list_head *n = head->next;
    while (n != head && n->next != head) {
        struct list_head *t = n;
        list_move(n, t->next);
        n = n->next;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    struct list_head *n, *s, *t;
    list_for_each_safe (n, s, head) {
        t = n->next;
        n->next = n->prev;
        n->prev = t;
    }
    t = head->next;
    head->next = head->prev;
    head->prev = t;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    if (!head || list_empty(head))
        return;
    struct list_head *n, *s, iter, *tmp_head = head;
    int i = 0;
    INIT_LIST_HEAD(&iter);
    list_for_each_safe (n, s, head) {
        i++;
        if (i == k) {
            list_cut_position(&iter, tmp_head, n);
            q_reverse(&iter);
            list_splice_init(&iter, tmp_head);
            i = 0;
            tmp_head = s->prev;
        }
    }
}

/* Sort elements of queue in ascending order */

struct list_head *merge_two_list(struct list_head *left,
                                 struct list_head *right)
{
    struct list_head head;
    struct list_head *h = &head;
    if (!left)
        return right;
    else if (!right)
        return left;

    while (left && right) {
        if (strcmp(list_entry(left, element_t, list)->value,
                   list_entry(right, element_t, list)->value) < 0) {
            h->next = left;
            left = left->next;
            h = h->next;
        } else {
            h->next = right;
            right = right->next;
            h = h->next;
        }
    }
    // after merge, there are still one node still not connect yet

    if (left) {
        h->next = left;
    } else if (right) {
        h->next = right;
    }
    return head.next;
}

struct list_head *merge_recur(struct list_head *head)
{
    if (!head->next)
        return head;

    struct list_head *slow = head;
    // split list
    for (struct list_head *fast = head->next; fast && fast->next;
         fast = fast->next->next) {
        slow = slow->next;
    }

    struct list_head *mid = slow->next;  // the start node of right part
    slow->next = NULL;

    struct list_head *left = merge_recur(head);
    struct list_head *right = merge_recur(mid);

    return merge_two_list(left, right);
}

void q_sort(struct list_head *head)
{
    // Leetcode #148 Sort List
    // https://leetcode.com/problems/sort-list/
    if (!head || list_empty(head))
        return;
    // disconnect the circular structure
    head->prev->next = NULL;
    head->next = merge_recur(head->next);
    // reconnect the list (prev and circular)
    struct list_head *c = head, *n = head->next;
    while (n) {
        n->prev = c;
        c = n;
        n = n->next;
    }
    c->next = head;
    head->prev = c;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    if (!head)
        return 0;
    struct list_head *c = head->prev;
    element_t *c_ele = list_entry(c, element_t, list);
    while (c_ele->list.prev != head) {
        element_t *n_ele = list_entry(c_ele->list.prev, element_t, list);
        if (strcmp(n_ele->value, c_ele->value) < 0) {
            list_del(&n_ele->list);
            q_release_element(n_ele);
        } else {
            c_ele = n_ele;
        }
    }
    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending order */
int q_merge(struct list_head *head)
{
    if (!head)
        return 0;
    queue_contex_t *c_cont;
    queue_contex_t *n_cont;
    struct list_head *sorted = NULL;

    list_for_each_entry_safe (c_cont, n_cont, head, chain) {  // iterate context
        c_cont->q->prev->next = NULL;
        c_cont->q->prev = NULL;
        sorted = merge_two_list(sorted, c_cont->q->next);
        INIT_LIST_HEAD(c_cont->q);  // reconnect the lists which are moved and
                                    // merged to "sorted" list;
    }
    LIST_HEAD(tmp);
    struct list_head *t = &tmp;
    t->next = sorted;
    struct list_head *c = t;
    while (sorted) {
        sorted->prev = c;
        c = sorted;
        sorted = sorted->next;
    }
    c->next = t;
    t->prev = c;
    int size = q_size(t);  // store size before splice to main queue
    list_splice(t, list_first_entry(head, queue_contex_t, chain)->q);
    return size;
}

void q_shuffle(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    int size = q_size(head);

    // shuffle
    for (int i = 0; i < size;) {  // not iterate i , iterate size
        struct list_head *start = head->next;
        int rand_idx = rand() % size;  // random number in range 0~ (size-1)
        for (int j = 0; j < rand_idx; j++) {
            start = start->next;
        }
        list_del(start);
        list_add_tail(start, head);
        size--;
    }
    return;
}
