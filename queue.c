#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
struct list_head *q_new()
{
    struct list_head *q = malloc(sizeof(struct list_head));
    if (q) {
        INIT_LIST_HEAD(q);
    }
    return q;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;
    element_t *n, *s;
    list_for_each_entry_safe (n, s, l, list)
        q_release_element(n);
    free(l);
}

/*
 * New an element for s,
 * It will allocate memory for s
 * Return null if allocation failed.
 */
element_t *new_element(char *s)
{
    element_t *new_ele = malloc(sizeof(element_t));
    if (!new_ele)
        return NULL;
    new_ele->value = strdup(s);
    if (!new_ele->value) {
        free(new_ele);
        return NULL;
    }
    return new_ele;
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *new_ele = new_element(s);
    if (!new_ele)
        return false;
    list_add(&new_ele->list, head);
    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *new_ele = new_element(s);
    if (!new_ele)
        return false;
    list_add_tail(&new_ele->list, head);
    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return target element.
 * Return NULL if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 *
 * NOTE: "remove" is different from "delete"
 * The space used by the list element and the string should not be freed.
 * The only thing "remove" need to do is unlink it.
 *
 * REF:
 * https://english.stackexchange.com/questions/52508/difference-between-delete-and-remove
 */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head)) {
        return NULL;
    }
    element_t *n = list_first_entry(head, element_t, list);
    list_del(head->next);
    if (sp) {
        strncpy(sp, n->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return n;
}

/*
 * Attempt to remove element from tail of queue.
 * Other attribute is as same as q_remove_head.
 */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    return q_remove_head(head->prev->prev, sp, bufsize);
}

/*
 * WARN: This is for external usage, don't modify it
 * Attempt to release element.
 */
void q_release_element(element_t *e)
{
    free(e->value);
    free(e);
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(struct list_head *head)
{
    int len = 0;
    if (head) {
        struct list_head *li;
        list_for_each (li, head)
            len++;
    }
    return len;
}

/*
 * Delete the middle node in list.
 * The middle node of a linked list of size n is the
 * ⌊n / 2⌋th node from the start using 0-based indexing.
 * If there're six element, the third member should be return.
 * Return true if successful.
 * Return false if list is NULL or empty.
 */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head)) {
        return false;
    }
    struct list_head *h = head->next;
    struct list_head *t = head->prev;
    while (true) {
        if (h == t) {
            // length is odd number, delete the one.
            list_del(h);
            q_release_element(list_entry(h, element_t, list));
            break;
        } else if (h->next == t) {
            // length is even number, delete the bigger one.
            list_del(t);
            q_release_element(list_entry(t, element_t, list));
            break;
        }
        h = h->next;
        t = t->prev;
    }
    return true;
}

/*
 * Delete all nodes that have duplicate string,
 * leaving only distinct strings from the original list.
 * Return true if successful.
 * Return false if list is NULL.
 *
 * Note: this function always be called after sorting, in other words,
 * list is guaranteed to be sorted in ascending order.
 */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head)
        return false;
    struct list_head *node;
    for (node = head->next; node->next != head;) {
        element_t *current = list_entry(node, element_t, list);
        element_t *next = list_entry(node->next, element_t, list);
        if (strcmp(current->value, next->value) == 0) {
            list_del(&next->list);
            q_release_element(next);
        } else {
            node = node->next;
        }
    }
    return true;
}

/*
 * Attempt to swap every two adjacent nodes.
 */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head)
        return;
    struct list_head *node;
    for (node = head->next; (node->next != head) && (node != head);
         node = node->next) {
        struct list_head *next = node->next;
        list_del(node);
        list_add(node, next);
    }
}

/* Swap two elements' data */
void swap_element_value(element_t *a, element_t *b)
{
    char *tmp = a->value;
    a->value = b->value;
    b->value = tmp;
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head)) {
        return;
    }
    struct list_head *h = head->next;
    struct list_head *t = head->prev;
    while (h != t) {
        // swap two element.
        swap_element_value(list_entry(h, element_t, list),
                           list_entry(t, element_t, list));
        if (h->next == t) {
            // swaped latest two element, over.
            break;
        }
        h = h->next;
        t = t->prev;
    }
}

struct list_head *my_merge(struct list_head *a, struct list_head *b)
{
    struct list_head *head = NULL, **tail = &head;
    for (;;) {
        /* if equal, take 'a' -- important for sort stability */
        if (strcmp(list_entry(a, element_t, list)->value,
                   list_entry(b, element_t, list)->value) <= 0) {
            *tail = a;
            tail = &a->next;
            a = a->next;
            if (!a) {
                *tail = b;
                break;
            }
        } else {
            *tail = b;
            tail = &b->next;
            b = b->next;
            if (!b) {
                *tail = a;
                break;
            }
        }
    }
    return head;
}

struct list_head *my_mergeSortList(struct list_head *head)
{
    if (!head || !head->next)
        return head;
    // use cycle detection to split list at middle
    struct list_head *slow = head;
    struct list_head *fast = head->next;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    fast = slow->next;
    slow->next = NULL;
    return my_merge(my_mergeSortList(head), my_mergeSortList(fast));
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void q_sort(struct list_head *head)
{
    if (q_size(head) <= 1) {
        return;
    }
    struct list_head *list = head->next;
    head->prev->next = NULL;
    list = my_mergeSortList(list);
    head->next = list;

    // rebuild prev link
    struct list_head *i = head;
    while (i->next != NULL) {
        i->next->prev = i;
        i = i->next;
    }
    head->prev = i;
    i->next = head;
}