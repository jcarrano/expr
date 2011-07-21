/*
 *      Tipos comunes
 */
#ifndef _TIPOS_H_
#define _TIPOS_H_

#include <math.h>

#ifdef COMPLEX_SUPPORT

#include "complex.h"
typedef complex double data_t;

#else /*COMPLEX_SUPPORT*/

typedef double data_t;

#endif /*COMPLEX_SUPPORT*/

#endif /* _TIPOS_H_ */
