/*
 * native.test.c
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


#include <stdio.h>
#include <stdlib.h>
#include <libjc/common.h>
#define EE PERROR
#include <expr/tipos.h>
#include <math.h>
/*
#define EVAL(x) (expr_eval(ex, &ram, (x)))
#define GETRET(n) (expr_get_ret(ram, n))
*/
data_t rect_int(data_t a, data_t b, int iter, data_t (*f)(data_t)) {
	data_t step, x[1] = {0}, ans=0;/* *ram = NULL;*/
	int itercount;
	
	step=(b-a)/iter;
	for(*x=a, itercount=0; itercount<iter; itercount++){
		*x+=step;
	/*	if (EVAL(x) < 0) {
			ans = NAN;
			goto rectint_error;
		} else {
			ans+=GETRET(0)*step;
		}
	*/
		ans+=f(*x)*step;
	}
	step=b-*x;
/*	EVAL(x);
	ans+=EVAL(x)*step;
*/
	ans+=f(*x)*step;
/*	
rectint_error:
	free(ram);
*/	return ans;
}

data_t f1(data_t x)
{
	return exp(sin(x) * 0.1 + 1) - cos(x);
}


data_t f2(data_t x)
{
	return x? sin(M_PI*x)/(M_PI*x) : 1;
}

int main(int argc, char *argv[])
{	
	int r = 0;
	
	if (argc == 5) {
		int nsteps = 0, fn;
		data_t a = 0, b = 0;
		data_t (*funcs[])(data_t) = {f1, f2};
		
		fn =  strtol(argv[1], NULL, 0);
		a = strtod(argv[2], NULL);
		b = strtod(argv[3], NULL);
		nsteps = strtol(argv[4], NULL, 0);
		
		fn = (fn > 1)? 1: ((fn < 0)? 0 : fn);
		printf("%g\n", rect_int(a, b, nsteps, funcs[fn]));
	} else {
		EE("Usage:\n");
		EE("%s funcnumber a b nsteps\n", argv[0]);
		r = 1;
	}
	
	return r;
}

