/*
 *      errors.c
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

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <libjc/common.h>
#include <errors.h>
#include <expr.h>

struct compile_error init_ce()
{
	struct compile_error ret;
	
	ret.type = -EXPR_OK;
	ret.fname = NULL;
	ret.pos = 0;
	ret.lineno = UNKN_LINE;
	ret.n = 0;
	ret.m = 0;
	
	return ret;
}

void destroy_ce(struct compile_error *ce)
{
	free(ce->fname);
	ce->fname = NULL;
}


char *ce_to_str(struct compile_error ce, int include_locinfo, int dual_line)
{
#define SLOC0 "Error at "
	
	char *sloc1 = NULL, *sloc2 = NULL, *serr = NULL, *sret = NULL;
	int len = 0;

	if (include_locinfo) {
		int n;
		
		len += strlen(SLOC0) + 1; /* +1 is to fit the separator */
		
		if (ce.lineno != UNKN_LINE) {
			if ((n=asprintf(&sloc1, "line %d, ", ce.lineno))>0) {
				len += n;
			} else {
				goto ce2str_err;
			}
		}
		if ((n=asprintf(&sloc2, "position %d. ", ce.pos))>0) {
			len += n;
		} else {
			goto ce2str_err;
		}
	}
	
	switch (ce.type) {
	int n;

#define ERR_P(msg...) {if((n = asprintf(&serr, msg))>0) len+=n; else goto ce2str_err;}

	case -EXPR_FEWVALUES:
		ERR_P("Not enough return values (only %d)", ce.n);
		break;
	case -EXPR_MANYVALUES:
		ERR_P("Excess return values (%d)", ce.n);
		break;
	case -EXPR_FEWARGS:
		ERR_P("Function '%s' takes %d values (%d provided)", ce.fname, 
								ce.n, ce.m);
		break;
	case -EXPR_NOSYM:
		ERR_P("Undefined symbol '%s'", ce.fname);
		break;
	case -EXPR_EXPECTING_INT:
		ERR_P("Expecting integer");
		break;
	case -EXPR_EXPECTING_VALUE:
		ERR_P("Expecting value (got '%s')", ce.fname);
		break;
	case -EXPR_EXPECTING_VAR:
		ERR_P("Expecting variable (got '%s')", ce.fname);
		break;
	case -EXPR_NOSUCH_VAR:
		ERR_P("'%s': no such variable", ce.fname);
		break;
	case -EXPR_ARG_OORANGE:
		ERR_P("Argument %d is out of range", ce.n);
		break;
	default:
		ERR_P("Unknown error");
		break;
	}
	
	{
		char *s0, *s1, *s2, *s3, *ssep;
		
		s0 = include_locinfo? SLOC0 : "";
		s1 = sloc1 !=NULL? sloc1 : "";
		s2 = sloc2 !=NULL ? sloc2 : "";
		ssep = include_locinfo? (dual_line? "\n" : " ") : "";
		s3 = serr;
		
		if (asprintf(&sret, "%s%s%s%s%s", s0, s1, s2, ssep, s3)<=0) {
ce2str_err:
			free(sloc1);
			free(sloc2);
			free(serr);
			free(sret);
			sret = NULL;
		}
	}
	
	return sret;
}
