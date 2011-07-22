/*
 *      expr.c
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

#include <stdio.h>
#include <malloc.h>
#include <expr.h>
#include <libjc/common.h>
#ifndef MALLOC
#define MALLOC __MALLOC
#endif

/* basic load */

#define XLOAD(where, x) ch_push(&(env->where), (void*)x)

int func_load(struct expr_environ *env, const struct expr_func *f)
{
	return XLOAD(funcs, f);
}

int const_load(struct expr_environ *env, const struct expr_const *f)
{
	return XLOAD(ctes, f);
}

int var_load(struct expr_environ *env, const struct expr_var *f)
{
	return XLOAD(vars, f);
}

/* Bulk load */

#define ARR_MULTILOAD(what, arr, ret) \
	int i; \
	int ret; \
	\
	for (i = sz - 1; i >= 0; i--) { \
		if ((ret = what##_load(env, &(arr[i]))) < 0) \
			break; \
	}


int func_multiload(struct expr_environ *env, const struct expr_func *arr, 
									int sz)
{
	ARR_MULTILOAD(func, arr, ret)
	return ret;
}

int const_multiload(struct expr_environ *env, const struct expr_const *arr,
									int sz)
{
	ARR_MULTILOAD(const, arr, ret)
	return ret;
}

int var_multiload(struct expr_environ *env, const struct expr_var *arr, int sz)
{
	ARR_MULTILOAD(var, arr, ret)
	return ret;
}

/* Constructors & destructors*/

struct expr_environ *new_env()
{
	struct expr_environ *new_env;
	
	if ((MALLOC(new_env)) != NULL) {
		new_env->funcs = new_chain();
		new_env->vars = new_chain();
		new_env->ctes = new_chain();
	}
	
	return new_env;
}

void destroy_env(struct expr_environ *env)
{
	if (env != NULL) {
		ch_destroy(&env->funcs);
		ch_destroy(&env->vars);
		ch_destroy(&env->ctes);
		free(env);
	}
}

//struct expr

/* Input Subsystem */


// struct expr *expr_clone(struct expr *expr)
// void expr_destroy(struct expr *expr)
// int expr_ramsize(struct expr *expr)
// mem_el *expr_eval(struct expr *expr, void *ram, data_t *args)
// inline data_t mvalue(mem_el ml)
