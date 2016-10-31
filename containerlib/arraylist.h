#ifndef JG_ARRAYLIST_H_INCLUDED
#define JG_ARRAYLIST_H_INCLUDED

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

typedef struct JGArrayList JGArrayList;

/*
 * Creates a list
 */
JGArrayList* JGArrayListCreate(int initial_size);

/*
 * Destroys a list optionally calling the free function free_fn on the elements in the list
 */
void JGArrayListDestroy(JGArrayList* list, void (* free_fn)(void*));

/*
 * Returns the number of elements in the list
 */
int JGArrayListCount(JGArrayList* list);

/*
 * gets the element at position idx
 */
void* JGArrayListGet(JGArrayList* list, unsigned int idx);

/*
 * Adds an element to the end
 */
void JGArrayListPush(JGArrayList* list, void* elem);

/*
 * Removes an element from the end
 */
void* JGArrayListPop(JGArrayList* list);

/*
 * Adds element elem to the list at  position idx
 */
void JGArrayListAdd (JGArrayList* list, void* elem, unsigned int idx);

/*
 * Removes and element from the list at position idx
 */
void* JGArrayListRemove(JGArrayList* list, unsigned int idx );

#endif