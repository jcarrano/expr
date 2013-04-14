/*
 *      parse.h
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
 
#ifndef _PARSE_H_

#include "expr.h"
#include "errors.h"
#include "input.h"

struct parse_options {
	int auto_clear : 1; /* auto_clear = 1 means to ignore excess values 
			returned, instead of reporting an error*/
	int n_args; /*Number of positional arguments */
	int n_rets; /* Number of values the expression MUST return */
};

extern struct compile_error expr_parse(struct input ci, struct expr_environ *env,
			struct parse_options opts, struct objcode **ocode);

#define _PARSE_H_
#endif /* _PARSE_H_ */
 
