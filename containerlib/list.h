#ifndef JG_LIST_H_INCLUDED
#define JG_LIST_H_INCLUDED

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

typedef struct JGList JGList;

/*
 * Creates a list
 */
JGList* JGListCreate();

/*
 * Destroys a list optionally calling the free function free_fn on the elements in the list
 */
void JGListDestroy(JGList* list, void (* free_fn)(void*));

/*
 * Returns the number of elements in the list
 */
int JGListCount(JGList* list);

/*
 * Adds an element to the end
 */
void JGListPush(JGList* list, void* elem);

/*
 * Removes an element from the end
 */
void* JGListPop(JGList* list);

/*
 * Adds element elem to the list at  position idx
 */
void JGListAdd (JGList* list, void* elem, int idx);

/*
 * Removes and element from the list at position idx
 */
void* JGListRemove(JGList* list, int idx );

#endif