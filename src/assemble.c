/*
 *      assemble.c
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
#include <libjc/common.h>
#define MALLOC __MALLOC
#include <assemble.h>

static struct expr *new_expr(struct objcode *oc)
{
	struct expr *ret = NULL;
	
	if (oc->program_size > 0 && (MALLOC(ret) != NULL)) {
		ret->varspace = NULL;
		if ((NMALLOC(ret->bcode, oc->program_size) != NULL)
		   && ((oc->n_vars == 0) 
		       || (NMALLOC(ret->varspace, oc->n_vars) != NULL))){
			ret->len = oc->program_size;
			ret->max_mem = oc->max_mem;
		} else {
			free(ret->bcode);
			free(ret->varspace);
			free(ret);
			ret = NULL;
		}
	}
	
	return ret;
}

void destroy_expr(struct expr *ex)
{
	free(ex->bcode);
	free(ex->varspace);
	free(ex);
}

struct expr *expr_assemble(struct objcode *oc)
{
	struct expr *ret;
	
	if ((ret = new_expr(oc)) != NULL) {
		struct ch_iterator chi = ch_iter(&oc->program);
		struct cblock *cb;
		int n;
		int plc = 0;
		
		while (!(n = ch_next(&chi, (void**)&cb))) {
			struct opcode op;
			int malformation = 0;
			
			switch (cb->type) {
			case BL_ARG:
				op.type = OC_ARG_LOAD;
				op.load.index = cb->data.arg_index;
				break;
			case BL_CLR:
				op.type = OC_FCLEAR;
				break;
			case BL_CONS:
				op.type = OC_DATA_LOAD;
				op.load.value = cb->data.cons->value;
				break;
			case BL_DATA:
				op.type = OC_DATA_LOAD;
				op.load.value = cb->data.data;
				break;
			case BL_FUNC:
				op.type = OC_FUNC;
				op.load.fun.f = cb->data.func->f;
				op.load.fun.stack_n = cb->data.func->n_rets -
							cb->data.func->n_args;
				break;
			case BL_GVAR_LD:
				op.type = OC_VAR_LOAD;
				op.load.location = cb->data.gvar->location;
				break;
			case BL_GVAR_ST:
				op.type = OC_VAR_STORE;
				op.load.location = cb->data.gvar->location;
				break;
			case BL_LVAR_LD:
				op.type = OC_VAR_LOAD;
				op.load.location = ret->varspace +
							     cb->data.l_index;
				break;
			case BL_LVAR_ST:
				op.type = OC_VAR_STORE;
				op.load.location = ret->varspace +
							     cb->data.l_index;
				break;
			case BL_NCLR:
				op.type = OC_NCLEAR;
				op.load.clear_n = cb->data.clear_n;
				break;
			default:
				malformation = 1;
				break;
			}
			if (malformation) {
				n = -EXPR_UNKNOWN_ERR;
				break;
			}
			ret->bcode[plc++] = op;
		}
		if (n < 0) {
			destroy_expr(ret);
			ret = NULL;
		}
	}
	
	return ret;
}
