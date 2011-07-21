/*
 *      compile.c
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

#define _GNU_SOURCE
#include <stdio.h>
#include <limits.h>
#include <malloc.h>

#include "expr.h"
#include "compile.h"

#define MALLOC __MALLOC

#define VARSET_ERR_TXT "[Variable set operation]"

static int oc_init(struct objcode *oc, int n_args, int n_ret)
{
	oc->program = new_chain();
	oc->program_size = 0;
	oc->max_mem = 0;
	oc->n_args = n_args;
	oc->n_ret = n_ret;
	oc->n_vars = 0;
/*	oc->assembled = NULL; */
	
	return -E_OK;
}

static int add_block(struct objcode *oc, struct cblock blk)
{
	int r;
	struct cblock *nblock;
	
	if (MALLOC(nblock) != NULL) {
		*nblock = blk;
		if ((r = ch_append(&oc->program, nblock)) != -E_OK)
			free(nblock);
		else
			oc->program_size++;
	} else {
		r = -E_NOMEM;
	}
	
	
	return r;
}

static int vc_add(struct proto_obj *po, int delta_vc)
{
	po->val_count += delta_vc;
	
	if (po->val_count > po->ocode->max_mem)
		po->ocode->max_mem = po->val_count;
	
	return po->val_count;
}

static struct objcode *new_oc(int n_args, int n_ret)
{
	struct objcode *noc;
	
	if ((MALLOC(noc)) != NULL) {
		if (oc_init(noc, n_args, n_ret) < 0) {
			free(noc);
			noc = NULL;
		}
	}
	
	return noc;
}

void destroy_oc(struct objcode *oc)
{
	void *m;
	
	while ((m = ch_pop(&oc->program)) != NULL) {
		free(m);
	}
	
	free(oc);
}

static struct _po_report report_init()
{
	struct _po_report r = {.fname = NULL, .takes = 0, .have = 0, .argn = 0};
	return r;
}

int po_init(struct proto_obj *po, int n_args, int n_rets)
{
	int ret = -E_NOMEM;
	
	if ((po->ocode = new_oc(n_args, n_rets)) != NULL) {
		po->status = -E_OK;
		po->report = report_init();
		po->val_count = 0;
		ret = -E_OK;
	}
	
	return ret;
}

#define IS_OK(ppo) (ppo->status == -E_OK)
#define IFOP if (IS_OK(po))
#define RPOS return po->status
#define POSS(_type, field, value) {\
	struct cblock __cb; \
	__cb.type = _type; \
	__cb.data.field = value; \
	po->status = add_block(po->ocode, __cb); }

int inject_data(struct proto_obj *po, data_t ddata)
{
	IFOP {
		vc_add(po, 1);
		POSS(BL_DATA, data, ddata)
	}
	RPOS;
}

int inject_arg(struct proto_obj *po, int argn)
{
	IFOP {
		if (argn < po->ocode->n_args ) {
			vc_add(po, 1);
			POSS(BL_ARG, arg_index, argn)
		} else {
			po->status = -EXPR_ARG_OORANGE;
			po->report.argn = argn;
		}
	}
	RPOS;
}

int inject_nclear(struct proto_obj *po, int n)
{
	
	IFOP {
		if (n <= po->val_count) {
			vc_add(po, -n);
			POSS(BL_NCLR, clear_n, n)
		} else {
			po->status = -EXPR_FEWARGS;
		}
	}
	RPOS;
}

int inject_clear(struct proto_obj *po)
{
	IFOP {
		po->val_count = 0;
		{	struct cblock __cb; \
			__cb.type = BL_CLR;
			po->status = add_block(po->ocode, __cb); 
		}
	}
	RPOS;
}

int inject_fn(struct proto_obj *po, struct expr_func *fn)
{
	IFOP {
		if (po->val_count >= fn->n_args) {
			vc_add(po, - fn->n_args + fn->n_rets);
			POSS(BL_FUNC, func, fn)
		} else {
			po->status = -EXPR_FEWARGS;
			po->report.fname = fn->nombre;
			po->report.takes = fn->n_args;
			po->report.have = po->val_count;
		}
	}
	RPOS;
}

int inject_const(struct proto_obj *po, struct expr_const *cn)
{
	IFOP {
		vc_add(po, 1);
		POSS(BL_CONS, cons, cn)
	}
	RPOS;
}

int inject_localvar_setdeclare(struct proto_obj *po)
{
	int ret;
	
	IFOP {
		if (po->val_count >= 1 && po->ocode->n_vars < INT_MAX) {
			ret = po->ocode->n_vars;
			po->ocode->n_vars++;
			POSS(BL_LVAR_ST, l_index, ret)
		} else {
			ret = po->status = (po->ocode->n_vars < INT_MAX)?
						-EXPR_FEWARGS : -E_NOMEM;
			po->report.fname = VARSET_ERR_TXT;
			po->report.have = po->val_count;
			po->report.takes = 1;
		}
	} else {
		ret = po->status;
	}
	
	return ret;
}

int inject_localvar_set(struct proto_obj *po, int vid)
{
	int ret;
	
	IFOP {
		if (po->val_count >= 1 && vid < po->ocode->n_vars) {
			POSS(BL_LVAR_ST, l_index, vid)
		} else {
			po->status = (vid < po->ocode->n_vars)?
					    -EXPR_FEWARGS : -EXPR_NOSUCH_VAR;
			po->report.fname = VARSET_ERR_TXT;
			po->report.have = po->val_count;
			po->report.takes = 1;
		}
	} else {
		ret = po->status;
	}
	RPOS;
}

int inject_localvar_get(struct proto_obj *po, int vid)
{
	int ret;
	
	IFOP {
		if (vid < po->ocode->n_vars) {
			vc_add(po, 1);
			POSS(BL_LVAR_LD, l_index, vid)
		} else {
			po->status = -EXPR_NOSUCH_VAR;
			po->report.fname = VARSET_ERR_TXT;
			po->report.have = po->val_count;
			po->report.takes = 1;
		}
	} else {
		ret = po->status;
	}
	RPOS;
}

int inject_globalvar_get(struct proto_obj *po, struct expr_var *ev)
{
	int ret;
	
	IFOP {
		vc_add(po, 1);
		POSS(BL_GVAR_LD, gvar, ev)
	} else {
		ret = po->status;
	}
	RPOS;
}

int inject_globalvar_set(struct proto_obj *po, struct expr_var *ev)
{
	int ret;
	
	IFOP {
		if (po->val_count >= 1) {
			POSS(BL_GVAR_ST, gvar, ev)
		} else {
			po->status = -EXPR_FEWARGS;
			po->report.fname = ev->nombre;
			po->report.have = po->val_count;
			po->report.takes = 1;
		}
	} else {
		ret = po->status;
	}
	RPOS;
}

int query_status(struct proto_obj *po)
{
	return po->status;
}

char *query_bad_fname(struct proto_obj *po)
{
	return po->report.fname;
}

int query_bad_argument(struct proto_obj *po)
{
	return po->report.argn;
}

int query_bf_in(struct proto_obj *po)
{
	return po->report.takes;
}

int query_bf_real(struct proto_obj *po)
{
	return po->report.have;
}

int query_n_rets(struct proto_obj *po)
{
	return po->ocode->n_ret;
}

int query_excess_rets(struct proto_obj *po)
{
	return po->val_count - query_n_rets(po);
}

struct objcode *po_compile(struct proto_obj *po)
{
	struct objcode *r = NULL;
	
	IFOP {
		if (query_excess_rets(po) >= 0) {
			r = po->ocode;
		} else {
			po->status = -EXPR_FEWVALUES;
		}
	}
	return r;
}

int po_abort(struct proto_obj *po)
{
	int r = po->status;
	
	destroy_oc(po->ocode);
	
	return r;
}
