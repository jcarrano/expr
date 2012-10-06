/* extra_math.c 
Funciones matematicas*/

/*e->euler, p->pi=3.14..., f->phi el numero dorado*/

#include <extra-math.h>

/* builtin support */

int func_load_builtins(struct expr_environ *env)
{
	return func_multiload(env, builtin_f_dict, builtin_num_fciones);
}

int const_load_builtins(struct expr_environ *env)
{
	return const_multiload(env, builtin_ctes_dict, builtin_num_ctes);
}

int load_builtins(struct expr_environ *env)
{
	int ret;
	if (	((ret = const_load_builtins(env)) >= 0)
	     &&	((ret = func_load_builtins(env)) >= 0) )
	     ;
	return ret;
}

#define F_HEADER(name) static void name (data_t *sp)

#ifdef COMPLEX_SUPPORT
#define CW(f) c##f
#else
#define CW(f) f
#endif

#define u sp[-1]
#define b sp[-1]
#define a sp[-2]

#define _lo_RET(n_args, i) sp[-(n_args) + (i)]
#define RET(x, n_args, i) (_lo_RET((n_args), (i)) = (x))
	/* Use this macro to generate the i-th return value from a function
	 * taking n_arg arguments.
	 * Warning: when writing the i return value, the (n_args+i) input
	 * argument is overwritten.
	 * i starts from 0
	 */
#define RET21 _lo_RET(2, 0) =
#define RET11 _lo_RET(1, 0) =

F_HEADER(suma){
	RET21 a+b;
}
F_HEADER(resta){
	RET21 a-b;
}
F_HEADER(mul){
	RET21 a*b;
}
F_HEADER(x_div) {
	RET21 a/b;
}
F_HEADER(x_abs) {
#ifdef COMPLEX_SUPPORT
	RET11 CW(abs)(u);
#else
	RET11 fabs(u);
#endif
}
F_HEADER(x_exp){ /* e a la a*/
	RET11 CW(exp)(u);
}
F_HEADER(x_pow){ /* a elevado a la b*/
	RET21 CW(pow)(a, b);
}
F_HEADER(x_pow2){ /* a elevado a la b*/
	RET11 u*u;
}
F_HEADER(x_pow3){ /* a elevado a la b*/
	RET11 u*u*u;
}
F_HEADER(sq){
	RET11 CW(sqrt)(u);
}
F_HEADER(rt){ /*raiz b-esima de a*/
	RET21 CW(exp)((log(a))/b);
}
F_HEADER(x_sen){
	RET11 CW(sin)(u);
}
F_HEADER(x_cos){
	RET11 CW(cos)(u);
}
F_HEADER(x_tan){
	RET11 CW(tan)(u);
}
F_HEADER(x_cosh){
	RET11 CW(cosh)(u);
}
F_HEADER(x_senh){
	RET11 CW(sinh)(u);
}
F_HEADER(x_tanh){
	RET11 CW(tanh)(u);
}
F_HEADER(arcsen){
	RET11 CW(asin)(u);
}
F_HEADER(arccos){
	RET11 CW(acos)(u);
}
F_HEADER(arctan){
	RET11 CW(atan)(u);
}
F_HEADER(arcsh){
	RET11 CW(asinh)(u);
}
F_HEADER(arcch){
	RET11 CW(acosh)(u);
}
F_HEADER(arcth){
	RET11 CW(atanh)(u);
}
F_HEADER(x_ln){ /*logaritmo natural de a*/
	RET11 CW(log)(u);
}
F_HEADER(x_log){ /*logaritmo en base b de a */
	RET21 CW(log)(a)/CW(log)(b);
}
F_HEADER(rpolar){ /*rectangular coordinates to polar */
	data_t r;
	r = hypot(a, b);
	RET(atan(b/a), 2, 1);
	RET(r, 2, 0);
}
#ifdef COMPLEX_SUPPORT
F_HEADER(re){ /*logaritmo en base b de a */
	RET11 CW(real)(u);
}
F_HEADER(im){ /*logaritmo en base b de a */
	RET11 CW(imag)(u);
}
F_HEADER(x_conj){
	RET11 conj(u);
}
F_HEADER(x_proj){
	RET11 CW(proj)(u);
}
F_HEADER(x_arg){
	RET11 CW(arg)(u);
}
#endif

#undef a
#undef b
#undef u

#define F21(sym, fp) {(sym), 2, 1, 1, (fp)} /*para funciones puras f: K^2 -> K */
#define F22(sym, fp) {(sym), 2, 2, 1, (fp)} /*K^2 -> K^2 */
#define F11(sym, fp) {(sym), 1, 1, 1, (fp)} /*para funciones puras f: K -> K */

const struct expr_func f_dict[] = { 
	F21("+",	suma	),
	F21("-",	resta	),
	F21("*",	mul	),
	F21("/",	x_div	),
	F11("abs",	x_abs	),
	F21("^",	x_pow	),
	F11("^2",	x_pow2	),
	F11("^3",	x_pow3	),
	F11("sq",	sq	),
	F21("rt",	rt	),
	F11("exp",	x_exp	),
	F11("sin",	x_sen	),
	F11("cos",	x_cos	),
	F11("tg",	x_tan	),
	F11("sh",	x_senh	),
	F11("ch",	x_cosh	),
	F11("th",	x_tanh	),
	F11("as",	arcsen	),
	F11("ac",	arccos	),
	F11("at",	arctan	),
	F11("ash",	arcsh	),
	F11("ach",	arcch	),
	F11("ath",	arcth	),
	F11("ln",	x_ln	),
	F21("log",	x_log	), /*20 elementos*/
#ifndef COMPLEX_SUPPORT 
	F22("rpolar",	rpolar	),
#else
	F11("re",	re ),
	F11("im",	im ),
	F11("conj",	x_conj ),
	F11("proj",	x_proj ),
	F11("arg",	x_arg),
#endif
};

const struct expr_const ctes[] = {
	{ "e", M_E },
	{ "pi", M_PI },
	{ "phi", 1.618033988749894848204586834365},
#ifdef COMPLEX_SUPPORT
	{ "i", I }
	{ "1i", I }
#endif
};


const struct expr_func *builtin_f_dict = f_dict;
const struct expr_const *builtin_ctes_dict = ctes;
const int builtin_num_fciones = ARSIZE(f_dict);
const int builtin_num_ctes = ARSIZE(ctes);
