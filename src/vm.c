/*
 *      vm.c
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
#include <vm.h>
#include <expr.h>

#include <vm_inlines.h>

int expr_eval(struct expr *ex, data_t **ram, data_t *args)
{
	int r = -E_OK;
	
	if (*ram == NULL && ((NMALLOC(*ram, ex->max_mem)) == NULL)) {
		r = -E_NOMEM;
	} else {
		int pc;
		data_t *sp = *ram;

#define PUSH(val) { *sp = (val); sp++; }

		for (pc = 0; pc < ex->len; pc++) {
			struct opcode ins = ex->bcode[pc];
			
			switch (ins.type) {
			case OC_DATA_LOAD:
				PUSH(ins.load.value)
				break;
			case OC_FUNC:
				ins.load.fun.f(sp);
				sp += ins.load.fun.stack_n;
				break;
			case OC_ARG_LOAD:
				PUSH(args[ins.load.index])
				break;
			case OC_VAR_LOAD:
				PUSH(*ins.load.location)
				break;
			case OC_VAR_STORE:
				*ins.load.location = sp[-1];
				break;
			case OC_NCLEAR:
				sp -= ins.load.clear_n;
				break;
			case OC_FCLEAR:
				sp = *ram;
				break;
			default:
				pc = ex->len;
				r = -EXPR_UNKNOWN_ERR;
				break;
			}
		}
	}
	
	return r;
}
