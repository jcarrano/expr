/*
 * calc.test.c
 * 
 * Copyright 2012 Juan I Carrano <juan@superfreak.com.ar>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libjc/common.h>
#define MALLOC __MALLOC
#include <expr/expr.h>
#include <expr/compile.h>
#include <expr/assemble.h>
#include <expr/vm.h>
#include <expr/parse.h>
#include <expr/input.h>
#include <expr/extra-math.h>

#define EE PERROR
#define CALC_N_RETS 1

#ifdef COMPLEX_SUPPORT
#define CALC_COMPLEX "Compiled with complex number support\n"
#else
#define CALC_COMPLEX ""
#endif

const char HelpStr[] = 
"Reverse polish notation calculator\n\
Use the 'help' function to see this message\n\
'functions' displays available funcions\n\
'constants' displays available constants\n\
Type 'quit' or 'exit' to terminate\n\
\n\
Special operators:\n\
\t'$ k'\treturns the k-th argument ( k >= 0 )\n\
\t'-> var'\tcopies the current value of the stack into var\n\
\t'clear n'\tclears n values from the stack\
\t';'\tclears all values from the stack\
\n\
Global variables:\n\
\t'ans'\tcontains the result of the last computation\n\n"CALC_COMPLEX
;

struct expr_environ *Genv;
int Gdone = 0;

void print_funcs(data_t *sp)
{
	struct ch_iterator chi = ch_iter(&Genv->funcs);
	struct expr_func *f;
	
	EE("# Name\t# of argumets\t# of return values\tReentant?\n");
	while (!ch_next(&chi, (void**)&f)) {
		EE("%s\t%d\t\t%d\t\t%d\n", f->name, f->n_args, f->n_rets, f->reentrant);
	}
	
	*sp = 0;
}


void print_consts(data_t *sp)
{
	struct ch_iterator chi = ch_iter(&Genv->ctes);
	struct expr_const *k;
	
	EE("# Name\tValue\n");
	while (!ch_next(&chi, (void**)&k)) {
		EE("%s\t%f\n", k->name, k->value);
	}
	
	*sp = 0;
}

void print_help(data_t *sp)
{
	EE(HelpStr);
	*sp = 0;
}

void do_quit(data_t *sp)
{
	Gdone = 1;
	*sp = 0;
}


void do_author(data_t *sp)
{
	EE("Copyright 2012 Juan I Carrano <juan@carrano.com.ar>\n");
	*sp = 94255639;
}

struct expr_func local_funcs[] = {
	{"functions", 0, 1, 0, print_funcs},
	{"constants", 0, 1, 0, print_consts},
	{"help", 0, 1, 0, print_help},
	{"quit", 0, 1, 0, do_quit},
	{"exit", 0, 1, 0, do_quit},
	{"exit", 0, 1, 0, do_quit},
	{"author", 0, 1, 0, do_author},
};

extern data_t eval(const data_t x, struct opcode bcode[], data_t ram[], int size);

#define EVAL(x) (expr_eval(ex, &ram, (x)))
#define GETRET(n) (expr_get_ret(ram, n))

data_t rect_int(data_t a, data_t b, int iter, struct expr *ex) {
	data_t step, x[1] = {0}, ans=0, *ram = NULL;
	int itercount;
	
	step=(b-a)/iter;
	for(*x=a, itercount=0; itercount<iter; itercount++){
		*x+=step;
		if (EVAL(x) < 0) {
			ans = NAN;
			goto rectint_error;
		} else {
			ans+=GETRET(0)*step;
		}
	}
	step=b-*x;
	EVAL(x);
	ans+=EVAL(x)*step;
	
rectint_error:
	free(ram);
	return ans;
}

void uiloop(data_t a, data_t b, int nsteps)
{
	int done = 0;
	struct input inp;
	struct parse_options po;
	struct expr_environ *env = new_env();
	struct expr_var ans;
	data_t ans_data = 45;
	
	Genv = env;
	
	/* setup global vars */
	ans.name = "ans";
	ans.location = &ans_data;
	var_load(env, &ans);
	
	/*setup C funcions & constants*/
	load_builtins(env);
	func_multiload(env, local_funcs, ARSIZE(local_funcs));

	
	mk_lineinput(&inp, stdin);
	
	/*Parser options*/
	po.auto_clear = 0;
	po.n_args = (nsteps != 0);
	po.n_rets = CALC_N_RETS;
	
	while(!done && !linput_done(inp) && !Gdone) {
		fputs("> ", stderr);
		if (linput_prefetch(inp)) {
			struct objcode *oc;
			struct compile_error ce;
			
			ce = expr_parse(inp, env, po, &oc);
			
			if (ce.type == -E_OK) {
				struct expr *ex;
				
				if ((ex = expr_assemble(oc)) != NULL) {
					data_t *rets = NULL;
					int r;
					
					if (!nsteps) {
						r = expr_eval(ex, &rets, NULL);
						if (r == -E_OK) {
							ans_data = expr_get_ret(rets, 0);
						} else {
							EE("Eval failed\n");
						}
						free(rets);
					} else {
						ans_data = rect_int(a, b, nsteps, ex);
					}
					printf("%g ;", ans_data);
					putchar('\n');
					
					destroy_expr(ex);
				} else {
					EE("Assembly failed\n");
				}
				destroy_oc(oc);
			} else {
				char *s;
				s = ce_to_str(ce,1,1);
				fputs(s, stderr);
				fputc('\n', stderr);
				free(s);
			}
			
			destroy_ce(&ce);
		}
	}
	
	destroy_env(env);
	destroy_lineinput(&inp);
}

int main(int argc, char *argv[])
{
	int nsteps = 0;
	data_t a = 0, b = 0;
	
	if (argc != 1 && argc != 4) {
		EE("Usage:\n");
		EE("To integrate each expression between a and b:"
						"%s a b nsteps\n", argv[0]);
		EE("If called without arguments it is a calculator\n");
		return 1;
	} else if (argc == 4) {
		a = strtod(argv[1], NULL);
		b = strtod(argv[2], NULL);
		nsteps = strtol(argv[3], NULL, 0);
	}
		
	uiloop(a, b, nsteps);
	
	return 0;
}

