/*
 * ultrafast.test.c
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

inline data_t f(data_t x)
{
	return exp(sin(x) * 0.1 + 1) - cos(x);
}


int main(int argc, char *argv[])
{	
	int r = 0;
	
	if (argc == 4) {
		int iter = 0, itercount;
		data_t a = 0, b = 0;
		data_t step, x=0, ans=0;
		
		a = strtod(argv[1], NULL);
		b = strtod(argv[2], NULL);
		iter = strtol(argv[3], NULL, 0);
		
		step=(b-a)/iter;
		for(x=a, itercount=0; itercount<iter; itercount++){
			x+=step;
			ans+=f(x)*step;
		}
		step=b-x;
	
		ans+=f(x)*step;
		
		printf("%g\n", ans);
	} else {
		EE("Usage:\n");
		EE("%s a b nsteps\n", argv[0]);
		r = 1;
	}
	
	return r;
}

