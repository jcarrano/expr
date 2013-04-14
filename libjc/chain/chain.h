/*
 *      chain.h
 *      
 *      Copyright 2010 Juan I Carrano <juan@superfreak.com.ar>
 *      
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *      
 *      Listas enlazadas, la manera simple.
 */

#ifndef _CHAIN_H_
#define _CHAIN_H_

#include "../common.h"

#define EMPTY_CHAIN (struct chain){.next=NULL, .last=NULL}

/* datos */

struct ch_node {
	void *data;
	struct ch_node *next;
};

struct chain {
	struct ch_node *next;
	struct ch_node *last;
};

struct ch_iterator {
	struct ch_node *next;
};

/* Funciones */

/* Primer elemento -> ultimo pusheado
*/

extern struct chain new_chain();
	/* Inicializa la cadena */
	
extern void ch_destroy(struct chain *chn);
	/* Destruye la cadena de manera superficial (shallow).
	 *   *Usar con cuidado*, lo ùnico que hace es vaciar
	 * la cadena con pops. no toca los datos. Puede ser 
	 * PELIGROSO (en términos de memory leaks)*/ 

extern int ch_isempty(const struct chain *chn);
	/* devuelve 1 si la lista esta vacia */

/* las sgtes funciones devuelven 0 si tienen exito, o un valor
 * negativo si fallan, estilo -E_NOMEM */

extern int ch_push(struct chain *chn, void *data);
	/* Agrega un elemento al principio 
	 * Si tiene éxito devuelve -E_OK
	 * si falla, un código de error */
	 
extern int ch_append(struct chain *chn, void *data);
	/* inserta un elemento al final */

extern void *ch_pop(struct chain *chn);
	/* Quita un elemento del principio de la cadena
	 * y devuelve su contenido. Si esta vacia devuelve
	 * null. */

extern void *ch_top(const struct chain *chn);
	/* Devuelvelve el primer elemento de la cadena,
	 * sin quitarlo. Si no hay develve null */ 

extern struct ch_iterator ch_iter(const struct chain *chn);
	/* devuelve un elemento que puede usarse para iterar
	 * a traves de la lista */

extern struct ch_iterator ch_cloneiter(struct ch_iterator iter);
	/* Hace un iter que está en la misma posicion */

extern int ch_next(struct ch_iterator *iter, void **ptr);
	/* Itera a traves de los elementos de la cadena.
	 * Devuelve 0 si quedan elementos, 1 si no. Si
	 * hubo error devuelve un nº negativo.
	 * Carga el puntero a los datos en ptr.
	 * <iter> queda destruido. Despues de que
	 * devuelve 0, los contenidos de ptr estan indeterminados
	 * Uso:

struct ch_node *iter = ch_first(&mychain);

while (!(n = ch_next(&iter, &mydata))) {
	do_thing_with(mydata);
	....
}
if(n<0)
	printf("error: %d\n, -n)

*/
	
extern void ch_reverse(struct chain *chn);
	/* invierte el orden de la cadena */

#endif /* _CHAIN_H_ */
