/*
 *      chain.c
 *
 *      Copyright 2010 Juan I Carrano <juan@superfreak.com.ar>
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *
 */

#include <malloc.h>
#include <libjc/chain/chain.h>

/* node operations */

static struct ch_node *mknode()
{
	return malloc(sizeof(struct ch_node));
}

static void rmnode( struct ch_node *node)
{
	free(node);
}

/* chain operations */

struct chain new_chain()
{
	return EMPTY_CHAIN;
}

inline void ch_destroy(struct chain *chn)
{
	while(!ch_isempty(chn))
		ch_pop(chn);
}

int ch_isempty(const struct chain *chn)
{
	return chn->next == NULL;
}

int ch_push(struct chain *chn, void *data)
{
	struct ch_node *new_node;
	int ret;

	if( (new_node = mknode())!=NULL){
		ret = SUCCESS;

		new_node->data = data;
		if( chn->last == NULL)
			chn->last = new_node;
		new_node->next = chn->next;
		chn->next = new_node;

	} else {
		ret = -E_NOMEM;
	}

	return ret;
}

int ch_append(struct chain *chn, void *data)
{
	struct ch_node *new_node;
	int ret;

	if ((new_node = mknode())!=NULL ){
		new_node->data = data;
		new_node->next = NULL;

		if (ch_isempty(chn))
			chn->next = new_node;
		else
			chn->last->next = new_node;
		chn->last = new_node;
		ret = SUCCESS;
	} else {
		ret = -E_NOMEM;
	}

	return ret;
}

void *ch_pop(struct chain *chn)
{
	void *ret;
	struct ch_node *old;

	if(!ch_isempty(chn))
	{
		ret = chn->next->data;
		old = chn->next;
		chn->next = chn->next->next;
		if(chn->next == NULL)
			chn->last = NULL;
		rmnode(old);
	} else {
		ret = NULL;
	}

	return ret;
}

void *ch_top(const struct chain *chn)
{
	void *ret;

	if(!ch_isempty(chn))
		ret = chn->next->data;
	else
		ret = NULL;

	return ret;
}

struct ch_iterator ch_iter(const struct chain *chn)
{
	struct ch_iterator new_iter;

	new_iter.next = chn->next;
	return new_iter;
}

struct ch_iterator ch_cloneiter(struct ch_iterator iter)
{
	return iter;
}
int ch_next(struct ch_iterator *iter, void **ptr)
{
	int done;

	if ( iter->next != NULL ){
		*ptr = iter->next->data;
		iter->next = iter->next->next;
		done = 0;
	} else {
		done = 1;
	}

	return done;
}

void ch_reverse(struct chain *chn)
{
	struct ch_node *previous=NULL, *current = chn->next, *tmp;

	tmp = chn->last;
	chn->last = chn->next;
	chn->next = tmp;

	while(current != NULL) {
		tmp = current->next;
		current->next = previous;
		previous = current;
		current = tmp;
	}
}
