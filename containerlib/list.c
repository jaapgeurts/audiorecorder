/*
 *   Copyright © 2016 Jaap Geurts
 *
 *   Permission to use, copy, modify, distribute, and sell this software and its
 *   documentation for any purpose is hereby granted without fee, provided that
 *   the above copyright notice appear in all copies and that both that
 *   copyright notice and this permission notice appear in supporting
 *   documentation.  No representations are made about the suitability of this
 *   software for any purpose.  It is provided "as is" without express or
 *   implied warranty.
 *
 *   This is file is part of the JGContainer library
 *
 *   This module implements a standard single linked list.
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "list.h"

struct node
{
    struct node* next;
    void*        elem;
};

struct JGList
{
    struct node* first;
    struct node* last;
    int          count;
};

struct node* JGINewNode(void* elem)
{
    struct node* node = (struct node*)calloc(1, sizeof(struct node));

    if (node == NULL)
    {
        fprintf(stderr, "Out of Memory when allocating node.\n");
        return NULL;
    }

    node->elem = elem;
    return node;
}

/*
 * Creates a list
 */
JGList* JGListCreate()
{
    JGList* list = (JGList*)calloc(1, sizeof(JGList));

    if (list == NULL)
        fprintf(stderr, "Out of Memory when allocating list\n");
    return list;
}

/*
 * Destroys a list optionally calling the free function free_fn on the elements in the list
 */
void JGListDestroy(JGList* list, void (* free_fn)(void*))
{
    if (list == NULL)
    {
        fprintf(stderr, "Illegal argument for parameter 1: NULL\n");
        return;
    }

    struct node* cursor, * tmp = list->first;

    while (cursor != NULL)
    {
        tmp = cursor->next;

        if (free_fn != NULL)
            free_fn(cursor->elem);
        free(cursor);
        cursor = tmp;
    }
    list->count = 0;
    list->first = NULL;
    list->last  = NULL;
    free(list);
}

/*
 * Returns the number of elements in the list. returns -1 on error
 */
int JGListCount(JGList* list)
{
    if (list == NULL)
    {
        fprintf(stderr, "Illegal argument for parameter 1: NULL\n");
        return -1;
    }

    return list->count;
}

/*
 * Adds an element to the end
 */
void JGListPush(JGList* list, void* elem)
{
    if (list == NULL)
    {
        fprintf(stderr, "Illegal argument for parameter 1: NULL\n");
        return;
    }

    struct node* node = JGINewNode(elem);

    if (node == NULL)
    {
        return;
    }

    if (list->first == NULL)
    {
        list->first = list->last = node;
        list->count++;
        return;
    }
    list->last->next = node;
    list->last       = node;
    list->count++;
}

/*
 * Removes an element from the end
 */
void* JGListPop(JGList* list)
{
    void*        elem = NULL;

    if (list->last == NULL)
    {
        fprintf(stderr, "Attempt to pop item from empty list\n");
        return NULL;
    }

    // only 1 element in the list
    if (list->last == list->first)
    {
        list->count--;
        elem = list->last->elem;
        free(list->first);
        list->first = list->last = NULL;
        return elem;
    }
    // find the element before last
    struct node* cursor = list->first;

    while (cursor->next != NULL)
    {
        if (cursor->next->next == NULL)
        {
            // last item found
            break;
        }
        cursor = cursor->next;
    }
    elem = cursor->next->elem;
    free(cursor->next);
    list->last   = cursor;
    cursor->next = NULL;
    list->count--;
    return elem;
}

/*
 * Adds element elem to the list at  position idx
 */
void JGListAdd (JGList* list, void* elem, int idx)
{
    fprintf(stderr, "Not yet implemented!\n");
}

/*
 * Removes and element from the list at position idx
 */
void* JGListRemove(JGList* list, int idx )
{
    fprintf(stderr, "Not yet implemented!\n");
    return NULL;
}