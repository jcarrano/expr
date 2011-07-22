/*
 *      errors.c
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
#include <errors.h>
#include <expr.h>

struct compile_error init_ce()
{
	struct compile_error ret;
	
	ret.type = -EXPR_OK;
	ret.fname = NULL;
	ret.pos = 0;
	ret.lineno = UNKN_LINE;
	ret.n = 0;
	ret.m = 0;
	
	return ret;
}

void destroy_ce(struct compile_error *ce)
{
	free(ce->fname);
	ce->fname = NULL;
}
