/*
 *      1.test.c
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
#include <stdlib.h>
#include "../lib/common.h"
#define MALLOC __MALLOC
#include "expr.h"
#include "compile.h"
#include "assemble.h"
#include "vm.h"
#include "parse.h"
#include "input.h"
#include "extra-math.h"

#define EE PERROR

int tester(int n_arg, int n_ret, data_t *args)
{
	int r;
	struct input inp;
	struct objcode *oc;
	struct compile_error ce;
	struct parse_options po;
	struct expr_environ *env = new_env();
	struct expr_var v1;
	data_t data1 = 45;
	
	v1.nombre = "var1";
	v1.location = &data1;
	
	po.auto_clear = 0;
	po.n_args = n_arg;
	po.n_rets = n_ret;
	
	mk_fileinput(&inp, stdin);
	load_builtins(env);
	var_load(env, &v1);
	
	ce = parse(inp, env, po, &oc);
	if (ce.type == -E_OK) {
		struct expr *ex;
		
		if ((ex = expr_assemble(oc)) != NULL) {
			data_t *rets = NULL;
			
			r = expr_eval(ex, &rets, args);
			if (r == -E_OK) {
				int i;
				
				for (i = 0; i < n_ret; i++)
					printf("%f ;", expr_get_ret(rets, i));
				putchar('\n');
			} else {
				EE("Eval failed\n");
			}
			
			free(rets);
			destroy_expr(ex);
		} else {
			EE("Assembly failed\n");
		}
		destroy_oc(oc);
	} else {	
		EE("Error:\n");
		EE("type: %d\n", ce.type);
		EE("pos: %d\n", ce.pos);
		EE("lineno: %d\n", ce.lineno);
		EE("fname: %s\n", ce.fname);
		EE("n: %d\n", ce.n);
		EE("m: %d\n", ce.m);
	}
	
	destroy_ce(&ce);
	destroy_env(env);
	destroy_fileinput(&inp);
	r = ce.type;
	
	return r;
}

int main(int argc, char *argv[])
{
	int r;
	
	if (argc < 3) {
		EE("Usage: test <n_args> <n_rets> <arg1> <arg2> ...\n");
		r = 1;
	} else {
		int n_arg, n_ret;
		
		n_arg =  atoi(argv[1]);
		n_ret =  atoi(argv[2]);
		
		if (argc != 3 + n_arg) {
			EE("you should provide %d arguments\n", n_arg);
			r = 2;
		} else {
			int i;
			data_t *args;
			
			if (n_arg)
				NMALLOC(args, n_arg);
			else
				args = NULL;
				
			for (i = 0; i < n_arg; i++)
				args[i] = strtod(argv[i+3], NULL);
			
			r = tester(n_arg, n_ret, args);
			
			free(args);
		}	
	}
	
	return r;
}

