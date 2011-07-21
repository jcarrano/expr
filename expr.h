/*
 *      expr.h
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

#ifndef _EXPR_H_
#define _EXPR_H_

/* Public header */

#include "tipos.h"
#include "chain.h"

enum COMPILE_ERRORS {
	EXPR_OK = E_OK,
	EXPR_MANYVALUES = N_ECODES,
	EXPR_FEWVALUES,
	EXPR_FEWARGS,
	EXPR_NOSYM,
	EXPR_UNEXPECTED_INT,
	EXPR_EXPECTING_VALUE,
	EXPR_EXPECTING_VAR,
	EXPR_NOSUCH_VAR,
	EXPR_ARG_OORANGE,
	EXPR_UNKNOWN_ERR /* = 16 */
};

struct expr_func {
	char *nombre; /*nombre de la funcion*/
	char n_args; /*numero de argumentos que toma. ceros si es constante, -n si es una variable*/
	char n_rets; /*número de args que devuelve*/
	int reentrant; /* 1 si la fción es reentrante */
	void ( *f)(data_t *); /* puntero a la función */
};

struct expr_const {
	char *nombre;
	data_t valor;
};

struct expr_var {
	char *nombre;
	data_t *location; /* puntero a algún lugar para almacenar la variable */
};

struct expr_environ {
	struct chain vars;
	struct chain funcs;
	struct chain ctes;
};

typedef struct opcode mem_el;

/* Constructor & destructors */

struct expr_environ *new_env();
void destroy_env(struct expr_environ *env);
	/* It should be safe to destroy an env even if the expressions
	 * created from it are still being used
	 */

/* these functions add a single element to the evironment */

extern int func_load(struct expr_environ *env, const struct expr_func *f);
extern int const_load(struct expr_environ *env, const struct expr_const *f);
extern int var_load(struct expr_environ *env, const struct expr_var *f);

/* The following functions load a entire array of size sz into the environment */

extern int func_multiload(struct expr_environ *env, const struct expr_func *arr,
									int sz);
extern int const_multiload(struct expr_environ *env,
					const struct expr_const *arr, int sz);
extern int var_multiload(struct expr_environ *env, const struct expr_var *arr, 
									int sz);

#endif /* _EXPR_H_ */
