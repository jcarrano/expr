/*
 *      input.h
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

#ifndef _INPUT_H_
#define _INPUT_H_
/* Public header */

#include <stdio.h>

#define UNKN_LINE (-1)

struct input {
	char *(*input)(void *);
	int (*lineno)(void *);
	void *cdata;
};

extern char *get_input(struct input ci);

extern int get_lineno(struct input ci);

extern struct input mk_input(char *(*input)(void *), int (*lineno)(void *), 
								void *cdata);

extern int mk_fileinput(struct input *ci, FILE *fp);

extern void destroy_fileinput(struct input *ci);

#endif /* _INPUT_H_ */
