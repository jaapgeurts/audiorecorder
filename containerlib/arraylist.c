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
 *   This module implements behaviour of a list, but is implemented as a dynamic array
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "arraylist.h"

#define INCREMENT_SIZE 128

struct JGArrayList
{
    void** elems;
    int size;
    int   count;
};

/*
 * Creates a list
 */
JGArrayList* JGArrayListCreate(int initial_size)
{
    JGArrayList* arraylist = (JGArrayList*)calloc(initial_size, sizeof(struct JGArrayList));
    
    if (arraylist == NULL) {
        fprintf(stderr, "Out of Memory when allocating arraylist\n");
        return NULL;
    }
    
    arraylist->elems = (void**)malloc(sizeof(void*)*initial_size);
    if (arraylist->elems == NULL) {
        fprintf(stderr, "Out of Memory when allocating arraylist\n");
        return NULL;
    }
    
    arraylist->size = initial_size;
    return arraylist;
}

/*
 * Destroys a list optionally calling the free function free_fn on the elements in the list
 */
void JGArrayListDestroy(JGArrayList* list, void (* free_fn)(void*))
{
    int i;

    if (free_fn != NULL)
    {
        for (i = 0; i < list->count; i++)
        {
            free_fn(list->elems[i]);
        }
    }
    free(list);
}

/*
 * Returns the number of elements in the list
 */
int JGArrayListCount(JGArrayList* list)
{
    return list->count;
}

/*
 * Adds an element to the end
 */
void JGArrayListPush(JGArrayList* list, void* elem)
{
    if (list->count == list->size)
    {
        void* newelems = (void**)realloc(list->elems,list->size + INCREMENT_SIZE);
        if (newelems == NULL ) {
            fprintf(stderr,"Out of Memory error when enlarging list\n");
            return;
        }else {
            list->elems = newelems;
        }
    }
    list->elems[list->count] = elem;
    list->count++;
}

/*
 * Removes an element from the end
 */
void* JGArrayListPop(JGArrayList* list)
{
    list->count--;
    if (list->count == 0)
    {
        fprintf(stderr,"Attempt to pop element from empty list\n");
        return NULL;
    }
    return list->elems[list->count];
}

/*
 * Adds element elem to the list at  position idx
 */
void JGArrayListAdd (JGArrayList* list, void* elem, int idx)
{
    fprintf(stderr,"Not yet implemented\n");
}
/*
 * Removes and element from the list at position idx
 */
void* JGArrayListRemove(JGArrayList* list, int idx )
{
    fprintf(stderr,"Not yet implemented\n");
    return NULL;
}

