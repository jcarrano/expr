/*
 *      vm.h
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

#ifndef _VM_H_
#define _VM_H_

#include "tipos.h"

/* opcode data load */

enum OPCODES {OC_DATA_LOAD, OC_VAR_LOAD, OC_ARG_LOAD, OC_VAR_STORE,
						OC_FUNC, OC_NCLEAR, OC_FCLEAR};

union load_t {
	data_t value;
	data_t *location; /* for variables */
	struct {
		void ( *f)(data_t *);
		int stack_n; /* number of position the stack pointer will be
				moved (can be negative)
				stack pointer += n_ret - n_args*/
	} fun;
	int index; /*for arguments*/
	int clear_n; /*for the N-CLEAR command*/
};

struct opcode {
	char type;
	union load_t load;
};

struct expr {
	struct opcode *bcode;
	data_t *varspace;
	int len;
	int max_mem;
/* 	int refcount; only if n_vars == 0 ( => varpace == NULL ) */
};

extern int expr_eval(struct expr *ex, data_t **ram, data_t *args);

#define _VMC_IS_HEADER
#include "vm_inlines.h"
#undef _VMC_IS_HEADER

#endif /* _VM_H_ */
