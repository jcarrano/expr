/*
 *      compile.h
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

#ifndef _COMPILE_H_
#define _COMPILE_H_

#include "expr.h"

struct objcode {
	struct chain program;
	int program_size;
	int max_mem;
	int n_args;
	int n_ret;
	int n_vars;
 /* 	struct expr *assembled; only if n_vars == 0 */
};

enum BLOCKTYPES { BL_CONS, BL_FUNC, BL_GVAR_LD, BL_GVAR_ST, BL_LVAR_LD,
				BL_LVAR_ST, BL_ARG, BL_CLR, BL_NCLR, BL_DATA};

struct cblock {
	int type;
	union {
		struct expr_const *cons;
		struct expr_func *func;
		struct expr_var *gvar;
		int l_index;
		int arg_index;
		int clear_n;
		data_t data;
	} data;
};

struct _po_report {
	char *fname;
	int takes;
	int have;
	int argn;
};

struct proto_obj {
	struct objcode *ocode;
	int val_count;
	int status;
	struct _po_report report;
};

extern int po_init(struct proto_obj *po, int n_args, int n_rets);

void destroy_oc(struct objcode *oc);

extern int po_init(struct proto_obj *po, int n_args, int n_rets);

extern int inject_data(struct proto_obj *po, data_t ddata);
extern int inject_arg(struct proto_obj *po, int argn);
extern int inject_nclear(struct proto_obj *po, int n);
extern int inject_clear(struct proto_obj *po);
extern int inject_fn(struct proto_obj *po, struct expr_func *fn);
extern int inject_const(struct proto_obj *po, struct expr_const *cn);
extern int inject_localvar_setdeclare(struct proto_obj *po);
extern int inject_localvar_set(struct proto_obj *po, int vid);
extern int inject_localvar_get(struct proto_obj *po, int vid);
extern int inject_globalvar_get(struct proto_obj *po, struct expr_var *ev);
extern int inject_globalvar_set(struct proto_obj *po, struct expr_var *ev);

extern int query_status(struct proto_obj *po);
extern char *query_bad_fname(struct proto_obj *po);
extern int query_bad_argument(struct proto_obj *po);
extern int query_bf_in(struct proto_obj *po);
extern int query_bf_real(struct proto_obj *po);
extern int query_n_rets(struct proto_obj *po);
extern int query_excess_rets(struct proto_obj *po);

extern struct objcode *po_compile(struct proto_obj *po);
extern int po_abort(struct proto_obj *po);


#endif /* _COMPILE_H_ */
