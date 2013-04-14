/*
 *      parse.c
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
		
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <tipos.h>
#include <compile.h>
#include <parse.h>
#include <libjc/common.h>
#define MALLOC __MALLOC

/*CONSTANTES SIMBOLICAS*/

#define BUF_SIZE 256
#define TERM '\0'
#define UNDEFINED -1
#define INVALIDO -1
#define _NO_SUCH_LOCAL (-1)

#ifdef COMPLEX_SUPPORT

#define PRINTR(r) printf("%.16f %+.16f i\n", creal(r), cimag(r))
#define N_ABLE(x) (N_able(cabs(x)))

#else /*COMPLEX_SUPPORT*/

#define PRINTR(r) printf("%.16f\n", r)
#define N_ABLE(x) (N_able(x))

#endif /*COMPLEX_SUPPORT*/

enum STATES {NADA, DATA, ARG, VARSET, NCLEAR};
enum KWS {NOT_A_KW, KW_VARSET ,KW_NCLEAR, KW_FULLCLEAR, KW_ARG};

struct kw_t {
	const char *name;
	int type;
};

const struct kw_t Kws[] = {
{ "->", KW_VARSET},
{ "clear", KW_NCLEAR},
{ ";", KW_FULLCLEAR},
{ "$", KW_ARG}
};

struct local_var {
	char *name;
	int index;
};

struct compile_error parse(struct input ci, struct expr_environ *env,
			struct parse_options opts, struct objcode **ocode);

/*GLOBALS*/

static int N_able(x) {
	return isfinite(x) && (x < INT_MAX) && (x >= 0);
}

#define ATODATA strtod

static int atodata_load(char *ptr, char **endptr, data_t *dest)
{
	data_t tmp;
	int ret = 0;
	
	tmp = ATODATA(ptr, endptr);
	if (ptr != *endptr) {
		*dest = tmp;
		ret = 1;
	}
	return ret;
}

static int atoint_load(char *ptr, char **endptr, int *dest)
{
	int tmp;
	int ret = 0;
	
	tmp = strtol(ptr, endptr, 0);
	if (ptr != *endptr) {
		*dest = tmp;
		ret = 1;
	}
	
	return ret;
}

static int loadtok(char **strp, char str[], char **endptr)
{	/* Reads a token from str, and copies it to a string it allocates
	 * If endptr is not null, it stores a pointer to the character after
	 * the last character read.
	 * The string *strp should be freed after use. As a special case, if
	 * no characters are read, no string is allocated and *strp is set to
	 * NULL
	 */

	int i;
	
	for(i = 0; isgraph(str[i]); i++)
		/*nothing*/;
	
	*strp = i? strndup(str, i) : NULL;
	
	if (endptr != NULL)
		*endptr = &(str[i]);
	
	return i;
}

static void * str_lookup(int (*match)(void *, char *), struct chain *ch, 
								char str[]){
	/*  */
	struct ch_iterator chi = ch_iter(ch);
	void *ret = NULL;
	int n = -1;
	
	
	while (!(n = ch_next(&chi, &ret))) {
		if (match(ret, str) == 0)
			break;
	}
	if (n != 0)
		ret = NULL;
	
	return ret;
}

static int _fn_cmp(void *_fcion, char *str)
{
	struct expr_func *fcion = (struct expr_func*)_fcion;
	return strcmp(fcion->name, str);
}

static struct expr_func *strtoFun(struct expr_environ *env, char str[]) {
	return (struct expr_func*)str_lookup(_fn_cmp, &env->funcs, str);
}

static int _ct_cmp(void *_ct, char *str)
{
	struct expr_const *ct = (struct expr_const*)_ct;
	return strcmp(ct->name, str);
}

static struct expr_const *strtoConst(struct expr_environ *env, char str[]) {
	return (struct expr_const*)str_lookup(_ct_cmp, &env->ctes, str);
}

static int _var_cmp(void *_var, char *str)
{
	struct expr_var *ct = (struct expr_var*)_var;
	return strcmp(ct->name, str);
}

static struct expr_var *strtoVar(struct chain *table, char str[]) {
	return (struct expr_var *)str_lookup(_var_cmp, table, str);
}

static int _lvar_cmp(void *_var, char *str)
{
	struct local_var *var = (struct local_var*)_var;
	return strcmp(var->name, str);
}

static int strtoLocal(struct chain *table, char str[]) {
	struct local_var *lv = (struct local_var *)str_lookup(_lvar_cmp, table, 
									str);
	
	return (lv != NULL)? lv->index : _NO_SUCH_LOCAL;
}

//static struct opcode build_op(

static int strtoKW(char str[])
{
	int i, ret = NOT_A_KW;
	
	for (i = 0; i < ARSIZE(Kws); i++) {
		if (strcmp(str, Kws[i].name) == 0) {
			ret = Kws[i].type;
			break;
		}
	}
	
	return ret;
}

static int eatspace(char str[], char **endptr)
{
	int r = 0;
	
	while (isspace(*str)) {
		str++;
		r++;
	}
	
	*endptr = str;
	
	return r;	
}
/*
static int top_index(struct chain *locals)
{
	return ((struct local_var *)ch_top(locals))->index;
}
*/
static int ins_local(struct chain *locals, char *str, int index) {
	struct local_var *nlv = NULL;
	int ins = -E_NOMEM;
	
	if ((MALLOC(nlv))){
		nlv->name = strdup(str);
		nlv->index = index;
		ins = ch_push(locals, nlv);
	} else {
		free(nlv);
	}
	
	return ins;
}

static void destroy_locals(struct chain *locals)
{
	struct local_var *lv;
	
	while ((lv = ch_pop(locals)) != NULL) {
		free(lv->name);
		free(lv);
	}
}

struct compile_error expr_parse(struct input ci, struct expr_environ *env,
			     struct parse_options opts, struct objcode **ocode)
{
	struct compile_error ret;
	struct chain locals = new_chain();
	struct proto_obj outp;
	char *end_ptr;
	int error = -E_OK, prev_stat=NADA, input_counter = 0;
	
	ret = init_ce();
	*ocode = NULL;
	
	end_ptr = ""; /*force an input read ("" != NULL) */
	
	//error = load_expr(&outp, expr);
	error = po_init(&outp, opts.n_args, opts.n_rets);
	
	while (error == -E_OK && end_ptr != NULL) {
		char *startptr = end_ptr;
		data_t tmp_data;
		int tmp_int;
		char *ident = NULL;
		
		if (eatspace(startptr, &end_ptr)) {
			/* do nothing */
		} else if (*startptr == TERM) {
			startptr = end_ptr = get_input(ci);
		} else if (prev_stat == NADA && 
				atodata_load(startptr, &end_ptr, &tmp_data)){
			error = inject_data(&outp, tmp_data);
			prev_stat = NADA;
		} else if (prev_stat == ARG || prev_stat == NCLEAR) {
			if (atoint_load(startptr, &end_ptr, &tmp_int)) {
				if (prev_stat == ARG)
					error = inject_arg(&outp, tmp_int);
				else
					error = inject_nclear(&outp, tmp_int);	
			} else {
				error = -EXPR_EXPECTING_INT;
			}
			prev_stat = NADA;
		} else if (loadtok(&ident, startptr, &end_ptr)) {
			int l_index;
			struct expr_var *tmp_var;
			
			if (prev_stat == NADA) {
				int kwn = strtoKW(ident);
				
				switch (kwn) {
				case KW_VARSET: prev_stat = VARSET; break;
				case KW_ARG: prev_stat = ARG; break;
				case KW_FULLCLEAR:
					error = inject_clear(&outp);
					prev_stat = NADA;
					break;
				case KW_NCLEAR: prev_stat = NCLEAR; break;
				default: /* not a kw */
 				{
				    struct expr_func *tmp_fn;
				    struct expr_const *tmp_const;
				    
				    int l_index;
				    if ((l_index = strtoLocal(&locals, ident )) 
							!= _NO_SUCH_LOCAL) {
					    error = inject_localvar_get(&outp, 
								      l_index);
				    } else if ((tmp_var = strtoVar(&env->vars, 
							    ident)) != NULL) {
					    error = inject_globalvar_get(&outp,
								      tmp_var);
				    } else if ((tmp_fn = strtoFun(env, ident)) 
								   != NULL) {
					    error = inject_fn(&outp, tmp_fn);
				    } else if ((tmp_const = strtoConst(env, 
							     ident)) != NULL) {
					    error = inject_const(&outp, 
								  tmp_const);
				    } else {
					    error = -EXPR_NOSYM;
				    }
				}
				}
			} else {
				if (strtoKW(ident) != NOT_A_KW) {
					if (prev_stat == VARSET)
						error = -EXPR_EXPECTING_VAR;
					else
						error = -EXPR_EXPECTING_VALUE;
				} else if (prev_stat == VARSET) {
				    if ((l_index = strtoLocal(&locals, ident )) 
							!= _NO_SUCH_LOCAL) {
					    error = inject_localvar_set(&outp, 
								      l_index);
				    } else if ((tmp_var = strtoVar(&env->vars, 
							    ident)) != NULL) {
					    error = inject_globalvar_set(&outp,
								      tmp_var);
				    } else {
					    l_index =inject_localvar_setdeclare(
									&outp);
					    if (l_index < 0) {
						    error = l_index;
					    } else {
						    error = ins_local(&locals, 
								ident, l_index);
					    }
				    }
				} else {
					error = -EXPR_NOSYM;
				}
				prev_stat = NADA;
			}
		} else {
			error = -EXPR_NOSYM;
		}
		input_counter += end_ptr - startptr;
		if (error != -E_OK) {
			ret.pos = input_counter;
			ret.lineno = get_lineno(ci);
			ret.type = error;
			if (ident != NULL) {
				ret.fname = ident;
				ident = NULL; /* prevent free() */
			} else {
				ret.fname = strndup(startptr, 
							    end_ptr - startptr);
			}
		}
		free(ident);
	}
	
	{
		char *bad_string;
		switch (ret.type) {
			case -EXPR_ARG_OORANGE:
				ret.n = query_bad_argument(&outp);
				break;
			case -EXPR_FEWARGS:
				ret.n = query_bf_in(&outp);
				ret.m = query_bf_real(&outp);
				break;
			default: /* <-- includes EXPECTING_STH */
				break;
			case -E_OK:
			    {
				int nr = query_excess_rets(&outp);
				
				if (!opts.auto_clear && nr > 0)
					ret.type = -EXPR_MANYVALUES;
				
				if (ret.type != -E_OK)
					ret.n = query_n_rets(&outp);
				else if ((*ocode = po_compile(&outp))==NULL)
					ret.type = query_status(&outp);
			    }
			    break;
		}
			
		if (ret.fname == NULL
		    && (bad_string = query_bad_fname(&outp)) != NULL) {
			ret.fname = strdup(bad_string);
		}
	}
	
	destroy_locals(&locals);
	
	if (*ocode == NULL) {
		po_abort(&outp);
	}
	
	return ret;
}
