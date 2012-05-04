/*
 *      optimize.c
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

int optimize(struct objcode *oc)
{
	struct ch_iterator chi = ch_iter(oc->program), chi0, chi1;
	int v_count = 0, max_mem = 0, n, to_cut = 0;
	struct cblock *cb;
	
	while (!(n = ch_next(&chi, &cb))) {
		if (cb->type == BL_DATA || cb->type == BL_CONS) {
			if (v_count == 0)
				chi0 = ch_cloneiter(chi);
			v_count++;
			to_cut++;
		} else if (cb->type == BL_FUNC
			  && (cb->data->func->reentrant)
			  && v_count >= data->func->n_args) {
				to_cut++;
				v_count += data->func->n_rets - 
							   data->func->n_args;
				chi1 = ch_cloneiter(chi);
		} else {
			assemble_n_novars(chi0, to_cut);
			ar_to_ocode( ... v_count);
			v_count = 0;
		}
	}
}
