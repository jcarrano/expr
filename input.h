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

#ifndef _EXPR_INPUT_H_
#define _EXPR_INPUT_H_
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

/* simple string input */

enum {STRINP_NOCOPY, STRINP_COPY};

extern int mk_strinput(struct input *ci, char *s, int make_copy);
	/* make_copy = STRINP_NOCOPY or STRINP_COPY
	 * Specifies whether to copy the string s, or to store a pointer.
	 */

extern void destroy_strinput(struct input *ci);
	/* If STRINP_COPY was selected in mk_strinput, this funcion frees the
	 * memory used by the copied string
	 */

/* simple file input. Based on getline(3) */

extern int mk_fileinput(struct input *ci, FILE *fp);

extern void destroy_fileinput(struct input *ci);

/* Line by line "interactive" input */

extern int mk_lineinput(struct input *ci, FILE *fp);

extern void destroy_lineinput(struct input *ci);

extern int linput_prefetch(struct input ci);
	/* Call this at the beginning of each line
	 * Returns true if the line contained something
	 */

extern int linput_done(struct input ci);
	/* Returns true if the end of file was reached */

#endif /* _EXPR_INPUT_H_ */
