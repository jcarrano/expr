/*extra-math.h*/

#ifndef _EXTRA_MATH_H_
#define _EXTRA_MATH_H_

/* private header */

#include <math.h>
#include <stddef.h>
#include "tipos.h"
#include "expr.h"

/* Call `load_builtins` to load builtin functions and constants
 * Bear in mind that i (the imaginary unit) is a constant, and + - * / are
 * functions, so you will not be able to use these unless you load them.  */
extern int load_builtins(struct expr_environ *env);

extern int func_load_builtins(struct expr_environ *env);
extern int const_load_builtins(struct expr_environ *env);

extern const struct expr_func *builtin_f_dict;
extern const struct expr_const *builtin_ctes_dict;

extern const int builtin_num_ctes;
extern const int builtin_num_fciones;

#endif /* _EXTRA_MATH_H_*/
