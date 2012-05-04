/*
 *      errors.h
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

#ifndef _ERRORS_H_
#define _ERRORS_H_

#include "input.h"

struct compile_error {
	int type; /* the type of the error (negative value, or -E_OK) */
	char *fname; /* name of the function or token that triggered the error,
			or null*/
	int pos, lineno, n, m;
		/* pos:	absolute position of the piece of input which
		 * 	  triggered the error
		 * lineno: line on which the error ocurred, or UNKN_LINE if line
		 * 		counting is not possible
		 */
};

/*
 * All errors contain the absolute position of the offending token on "pos"
 * and (if the input provides a lineno callback) the line number on lineno;
 * type indicates the type of error. If parsing and compilation succeeds, it is
 * set to -E_OK.
 * fname is set as indicated below, and it may also be set to some string in
 * other cases, so as to provide nore information.
 * n and m are general purpose fields.
EXPR_MANYVALUES 	=>  	n: number of values
EXPR_FEWVALUES 		=>  	n: number of values
EXPR_FEWARGS		=>	fname: name of function, n: n_args of function,
				m: args provided
EXPR_NOSYM		=> 	undefined symbol
EXPR_EXPECTING_INT 	=>	
EXPR_EXPECTING_VALUE	=>	fname: copy of offending string
EXPR_EXPECTING_VAR
EXPR_NOSUCH_VAR		=>	fname: copy of unidentified string
EXPR_ARG_OORANGE	=>	n: argument number which is out of range.
EXPR_UNEXPECTED_INT     =>     	n: integer
*/

extern struct compile_error init_ce();
	/* Called internally by parse()-like functions to initialize */

extern char *ce_to_str(struct compile_error ce, int include_locinfo, int dual_line);
	/* Return a human-readable description of the error.
	 * The caller must free() the returned string
	 * Return NULL on error
	 * include_loconfo: include location of the error
	 * dual line: separate location and description by a newline  */

extern void destroy_ce(struct compile_error *ce);
	/* must be called by users after parse()-like functions, to free any 
	 * strings */


#endif /*_ERRORS_H_*/
