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

struct _lrdata {
	FILE *fp;
	char *oldbuf;
	size_t buflen;
	int lines;
};

static char *_line_read(void *_data)
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

static int _line_number(void *_data)
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
		
		*ci = mk_input(_line_read, _line_number, cdata);
		
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
