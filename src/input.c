/*
 *      input.c
 *      
 *      Copyright 2011 Juan I Carrano <juan@superfreak.com.ar>
 *      
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *      
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *      
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *      MA 02110-1301, USA.
 */

#include <malloc.h>
#include <string.h>
#include <ctype.h>

#include <input.h>
#include <libjc/common.h>
#define MALLOC __MALLOC

char *get_input(struct input ci)
{
	return ci.input(ci.cdata);
}

int get_lineno(struct input ci)
{
	return (ci.lineno != NULL)? ci.lineno(ci.cdata) : UNKN_LINE;
}

struct input mk_input(char *(*input)(void *), int (*lineno)(void *), 
								void *cdata)
{
	struct input ni;
	
	ni.input = input;
	ni.cdata = cdata;
	ni.lineno = lineno;
	return ni;
}

/*	*	* 	SIMPLE FILE INPUT 	*	*	*	*/

struct _lrdata {
	FILE *fp;
	char *oldbuf;
	size_t buflen;
	int lines;
};

static char *_fileinput_read(void *_data)
{
	struct _lrdata *data = (struct _lrdata*)_data;
	
	if (getline(&(data->oldbuf), &(data->buflen), data->fp) < 0) {
		free(data->oldbuf);
		data->oldbuf = NULL;
		data->buflen = 0;
	} else {
		data->lines++;
	}
	
	return data->oldbuf;
}

static int _fileinput_number(void *_data)
{
	struct _lrdata *data = (struct _lrdata*)_data;
	
	return data->lines;
}

int mk_fileinput(struct input *ci, FILE *fp)
{
	struct _lrdata *cdata;
	int ret = -E_NOMEM;
	
	if ((MALLOC(cdata)) != NULL) {
		cdata->fp = fp;
		cdata->oldbuf = NULL;
		cdata->buflen = 0;
		cdata->lines = 0;
		
		*ci = mk_input(_fileinput_read, _fileinput_number, cdata);
		
		ret = -E_OK;
	}
	
	return ret;
}

void destroy_fileinput(struct input *ci)
{
	struct _lrdata *cdata = (struct _lrdata *)(ci->cdata);
	
	if (cdata != NULL) {
		free(cdata->oldbuf);
		cdata->oldbuf = NULL;
		free(ci->cdata);
		ci->cdata = NULL;
	}
}

/*	*	*	LINE INPUT 	*	*	*	*	*/

struct _lineinput {
	int continue_reading, donefile;
	char *L;
	struct input fi;
};

static int _linput_lineno(void *_data)
{
	return UNKN_LINE;
}

static char *_linput_fetch(void *_data)
{
	struct _lineinput *li = (struct _lineinput*)_data;
	char *L;
	
	L = get_input(li->fi);
	if (L == NULL) {
		li->donefile = 1;
	} else {
		int len;
		len = strlen(L);
		if (len > 1 && L[len-2] == '\\' && L[len-1] == '\n') {
			L[len-2] = '\0';
			li->continue_reading = 1;
		} else {
			li->continue_reading = 0;
		}
	}
	return L;
}

static char *_linput_read(void *_data)
{
	struct _lineinput *data = (struct _lineinput*)_data;
	char *L;
	
	if (data->continue_reading) {
		if (data->L != NULL) {
			L = data->L;
			data->L = NULL;
			data->continue_reading = 0;
		} else {
			L = _linput_fetch(data);
			data->L = NULL;
		}
	} else {
		L = NULL;
	}
	
	return L;
}

int linput_prefetch(struct input ci)
{
	struct _lineinput *li = (struct _lineinput *)(ci.cdata);
	
	li->L = _linput_fetch(li);
	li->continue_reading = 1;
	return (li->L != NULL) && (strlen(li->L) != 0) 
				&& !(strlen(li->L) == 1 && isspace(li->L[0]));
}

int linput_done(struct input ci)
{
	return ((struct _lineinput *)ci.cdata)->donefile;
}

int mk_lineinput(struct input *ci, FILE *fp)
{
	struct _lineinput *li;
	int ret = -E_NOMEM;
	
	if ((MALLOC(li) != NULL) && ((ret = mk_fileinput(&li->fi, fp)) >= 0)) {
		li->continue_reading = 0;
		li->donefile = 0;
		li->L = NULL;
		
		ci->input = _linput_read;
		ci->lineno = _linput_lineno;
		ci->cdata = li;
	} else {
		if (li != NULL)
			destroy_fileinput(&li->fi);
		free(li);
	}
	
	return ret;
}

void destroy_lineinput(struct input *ci)
{
	if (ci->cdata != NULL) {
		destroy_fileinput(&((struct _lineinput *)ci->cdata)->fi);
		free(ci->cdata);
		ci->cdata = NULL;
	}
}
