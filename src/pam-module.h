
/**
 * @file pam-module.h
 * @brief The pam-config module API.
 *
 * The following code shows a usage example of the pam-config module
 * API. It defines a new pam-config module \c fnord.
 * \include mod_pam_fnord.c
 *
 * @author Thorsten Kukuk, Sven Schober
 * @date 2007-07-23
 */
/*
 * Copyright (C) 2007, 2008, 2009, 2012, 2018, 2020 - Thorsten Kukuk, Sven Schober
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */
#ifndef _PAM_MODULES_H_
#define _PAM_MODULES_H_ 1

#include <stdio.h>
#include <stdint.h>
#include "option_set.h"
#include "module_priorities.h"

/**
 * @typedef global_opt_t
 * @brief Creates a type to hold global commandline options
 */
typedef struct gobal_opt {
  int m_add, m_create, m_delete, m_init, m_update;
  int m_query, m_verify;
  int force;
  int opt_val;
} global_opt_t;


/**
 * @ struct module_helptext_t
 * @brief Contains the help text for a module option
 */
typedef struct {
  char *option;
  char *arg;
  char *helptxt;
} module_helptext_t;


/**
 * @def TRUE
 * @brief Conveniance definition of boolean value TRUE.
 */
#define TRUE  1
/**
 * @def FALSE
 * @brief Conveniance definition of boolean value FALSE.
 */
#define FALSE 0

#define STRING_DEFAULT	NULL
#define BOOL_DEFAULT	FALSE

#define DEBUG(args...)  fprintf( stderr, "%s [%d]: ", __FILE__, __LINE__ );fprintf( stderr, args )

#define OPT_NAME(PREFIX, NAME) PREFIX ## _opt_ ## NAME

#define INIT_OPT_1(PREFIX,TYPE,DEFAULT,a)	        static TYPE ## _option_t OPT_NAME(PREFIX,a) = { #a, DEFAULT }
#define INIT_OPT_2(PREFIX,TYPE,DEFAULT,a,b)	        INIT_OPT_1(PREFIX,TYPE,DEFAULT,a);       INIT_OPT_1(PREFIX,TYPE,DEFAULT,b)
#define INIT_OPT_3(PREFIX,TYPE,DEFAULT,a,b,c)	        INIT_OPT_2(PREFIX,TYPE,DEFAULT,a,b);     INIT_OPT_1(PREFIX,TYPE,DEFAULT,c)
#define INIT_OPT_4(PREFIX,TYPE,DEFAULT,a,b,c,d)         INIT_OPT_2(PREFIX,TYPE,DEFAULT,a,b);     INIT_OPT_2(PREFIX,TYPE,DEFAULT,c,d)
#define INIT_OPT_5(PREFIX,TYPE,DEFAULT,a,b,c,d,e)       INIT_OPT_4(PREFIX,TYPE,DEFAULT,a,b,c,d); INIT_OPT_1(PREFIX,TYPE,DEFAULT,e)
#define INIT_OPT_6(PREFIX,TYPE,DEFAULT,a,b,c,d,e,f)     INIT_OPT_4(PREFIX,TYPE,DEFAULT,a,b,c,d); INIT_OPT_2(PREFIX,TYPE,DEFAULT,e,f)
#define INIT_OPT_7(PREFIX,TYPE,DEFAULT,a,b,c,d,e,f,g)   INIT_OPT_4(PREFIX,TYPE,DEFAULT,a,b,c,d); INIT_OPT_3(PREFIX,TYPE,DEFAULT,e,f,g)
#define INIT_OPT_8(PREFIX,TYPE,DEFAULT,a,b,c,d,e,f,g,h) INIT_OPT_4(PREFIX,TYPE,DEFAULT,a,b,c,d); INIT_OPT_4(PREFIX,TYPE,DEFAULT,e,f,g,h)
#define INIT_OPT_9(PREFIX,TYPE,DEFAULT,a,b,c,d,e,f,g,h,i)      INIT_OPT_5(PREFIX,TYPE,DEFAULT,a,b,c,d,e); INIT_OPT_4(PREFIX,TYPE,DEFAULT,f,g,h,i)
#define INIT_OPT_10(PREFIX,TYPE,DEFAULT,a,b,c,d,e,f,g,h,i,j)   INIT_OPT_5(PREFIX,TYPE,DEFAULT,a,b,c,d,e); INIT_OPT_5(PREFIX,TYPE,DEFAULT,f,g,h,i,j)
#define INIT_OPT_11(PREFIX,TYPE,DEFAULT,a,b,c,d,e,f,g,h,i,j,k) INIT_OPT_6(PREFIX,TYPE,DEFAULT,a,b,c,d,e,f); INIT_OPT_5(PREFIX,TYPE,DEFAULT,g,h,i,j,k)
#define INIT_OPT_14(PREFIX,TYPE,DEFAULT,a,b,c,d,e,f,g,h,i,j,k,l,m,n) INIT_OPT_8(PREFIX,TYPE,DEFAULT,a,b,c,d,e,f,g,h); INIT_OPT_6(PREFIX,TYPE,DEFAULT,i,j,k,l,m,n)
#define INIT_OPT_15(PREFIX,TYPE,DEFAULT,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o) INIT_OPT_8(PREFIX,TYPE,DEFAULT,a,b,c,d,e,f,g,h); INIT_OPT_7(PREFIX,TYPE,DEFAULT,i,j,k,l,m,n,o)
#define INIT_OPT_16(PREFIX,TYPE,DEFAULT,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p) INIT_OPT_8(PREFIX,TYPE,DEFAULT,a,b,c,d,e,f,g,h); INIT_OPT_8(PREFIX,TYPE,DEFAULT,i,j,k,l,m,n,o,p)
#define INIT_OPT_17(PREFIX,TYPE,DEFAULT,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q) INIT_OPT_8(PREFIX,TYPE,DEFAULT,a,b,c,d,e,f,g,h); INIT_OPT_9(PREFIX,TYPE,DEFAULT,i,j,k,l,m,n,o,p,q)

#define OPT_SET_START(PREFIX, TYPE) static TYPE ## _option_t * PREFIX ## _ ## TYPE ## _opts[] = {
#define OPT_SET_END NULL }

#define OPT_SET_BODY_1(PREFIX, a)	    &OPT_NAME(PREFIX, a)
#define OPT_SET_BODY_2(PREFIX, a,b)	    OPT_SET_BODY_1(PREFIX,a),   OPT_SET_BODY_1(PREFIX, b)
#define OPT_SET_BODY_3(PREFIX, a,b,c)	    OPT_SET_BODY_2(PREFIX,a,b), OPT_SET_BODY_1(PREFIX, c)
#define OPT_SET_BODY_4(PREFIX, a,b,c,d)	    OPT_SET_BODY_2(PREFIX,a,b), OPT_SET_BODY_2(PREFIX, c,d)
#define OPT_SET_BODY_5(PREFIX, a,b,c,d,e)   OPT_SET_BODY_2(PREFIX,a,b), OPT_SET_BODY_3(PREFIX, c,d,e)
#define OPT_SET_BODY_6(PREFIX, a,b,c,d,e,f) OPT_SET_BODY_2(PREFIX,a,b), OPT_SET_BODY_4(PREFIX, c,d,e,f)
#define OPT_SET_BODY_7(PREFIX, a,b,c,d,e,f,g) OPT_SET_BODY_2(PREFIX,a,b), OPT_SET_BODY_5(PREFIX, c,d,e,f,g)
#define OPT_SET_BODY_8(PREFIX, a,b,c,d,e,f,g,h) OPT_SET_BODY_2(PREFIX,a,b), OPT_SET_BODY_6(PREFIX, c,d,e,f,g,h)
#define OPT_SET_BODY_9(PREFIX, a,b,c,d,e,f,g,h,i) OPT_SET_BODY_3(PREFIX,a,b,c), OPT_SET_BODY_6(PREFIX, d,e,f,g,h,i)
#define OPT_SET_BODY_10(PREFIX, a,b,c,d,e,f,g,h,i,j) OPT_SET_BODY_5(PREFIX,a,b,c,d,e), OPT_SET_BODY_5(PREFIX, f,g,h,i,j)
#define OPT_SET_BODY_11(PREFIX, a,b,c,d,e,f,g,h,i,j,k) OPT_SET_BODY_5(PREFIX,a,b,c,d,e), OPT_SET_BODY_6(PREFIX, f,g,h,i,j,k)
#define OPT_SET_BODY_14(PREFIX, a,b,c,d,e,f,g,h,i,j,k,l,m,n) OPT_SET_BODY_8(PREFIX,a,b,c,d,e,f,g,h), OPT_SET_BODY_6(PREFIX, i,j,k,l,m,n)
#define OPT_SET_BODY_15(PREFIX, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o) OPT_SET_BODY_8(PREFIX,a,b,c,d,e,f,g,h), OPT_SET_BODY_7(PREFIX, i,j,k,l,m,n,o)
#define OPT_SET_BODY_16(PREFIX, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p) OPT_SET_BODY_8(PREFIX,a,b,c,d,e,f,g,h), OPT_SET_BODY_8(PREFIX, i,j,k,l,m,n,o,p)
#define OPT_SET_BODY_17(PREFIX, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q) OPT_SET_BODY_8(PREFIX,a,b,c,d,e,f,g,h), OPT_SET_BODY_9(PREFIX, i,j,k,l,m,n,o,p,q)

#define GENERIC_OPT_SET_1(PREFIX,TYPE,DEFAULT,a)	      INIT_OPT_1(PREFIX, TYPE, DEFAULT, a); \
							      OPT_SET_START(PREFIX, TYPE) \
							      OPT_SET_BODY_1(PREFIX,a),\
							      OPT_SET_END

#define GENERIC_OPT_SET_2(PREFIX,TYPE,DEFAULT, a, b)			\
  INIT_OPT_2(PREFIX, TYPE, DEFAULT, a, b);				\
  OPT_SET_START(PREFIX, TYPE)						\
       OPT_SET_BODY_2(PREFIX,a, b),					\
    OPT_SET_END

#define GENERIC_OPT_SET_3(PREFIX, TYPE, DEFAULT, a, b, c)     INIT_OPT_3(PREFIX,TYPE,DEFAULT, a, b, c );\
							      OPT_SET_START(PREFIX, TYPE) \
							      OPT_SET_BODY_3(PREFIX,a,b,c),\
							      OPT_SET_END
#define GENERIC_OPT_SET_4(PREFIX, TYPE, DEFAULT, a, b, c, d)  INIT_OPT_4(PREFIX,TYPE,DEFAULT, a, b, c, d );\
							      OPT_SET_START(PREFIX, TYPE) \
							      OPT_SET_BODY_4(PREFIX,a,b,c,d),\
							      OPT_SET_END
#define GENERIC_OPT_SET_5(PREFIX, TYPE, DEFAULT, a, b,c,d,e)  INIT_OPT_5(PREFIX,TYPE,DEFAULT, a, b, c, d, e ); \
							      OPT_SET_START(PREFIX, TYPE) \
							      OPT_SET_BODY_5(PREFIX,a,b,c,d,e), \
							      OPT_SET_END
#define GENERIC_OPT_SET_6(PREFIX, TYPE, DEFAULT,a,b,c,d,e,f)  INIT_OPT_6(PREFIX,TYPE,DEFAULT,a,b,c,d,e,f); \
							      OPT_SET_START(PREFIX, TYPE) \
							      OPT_SET_BODY_6(PREFIX,a,b,c,d,e,f), \
							      OPT_SET_END
#define GENERIC_OPT_SET_7(PREFIX, TYPE, DEFAULT,a,b,c,d,e,f,g) INIT_OPT_7(PREFIX,TYPE,DEFAULT,a,b,c,d,e,f,g); \
                                                               OPT_SET_START(PREFIX, TYPE)		      \
							       OPT_SET_BODY_7(PREFIX,a,b,c,d,e,f,g), \
							       OPT_SET_END
#define GENERIC_OPT_SET_8(PREFIX, TYPE, DEFAULT,a,b,c,d,e,f,g,h)  INIT_OPT_8(PREFIX,TYPE,DEFAULT,a,b,c,d,e,f,g,h); \
							          OPT_SET_START(PREFIX, TYPE) \
								  OPT_SET_BODY_8(PREFIX,a,b,c,d,e,f,g,h), \
								  OPT_SET_END
#define GENERIC_OPT_SET_10(PREFIX, TYPE, DEFAULT,a,b,c,d,e,f,g,h,i,j)  INIT_OPT_10(PREFIX,TYPE,DEFAULT,a,b,c,d,e,f,g,h,i,j); \
							          OPT_SET_START(PREFIX, TYPE) \
							          OPT_SET_BODY_10(PREFIX,a,b,c,d,e,f,g,h,i,j), \
								  OPT_SET_END
#define GENERIC_OPT_SET_11(PREFIX, TYPE, DEFAULT,a,b,c,d,e,f,g,h,i,j,k)  INIT_OPT_11(PREFIX,TYPE,DEFAULT,a,b,c,d,e,f,g,h,i,j,k); \
							          OPT_SET_START(PREFIX, TYPE) \
							          OPT_SET_BODY_11(PREFIX,a,b,c,d,e,f,g,h,i,j,k), \
								  OPT_SET_END
#define GENERIC_OPT_SET_14(PREFIX, TYPE, DEFAULT,a,b,c,d,e,f,g,h,i,j,k,l,m,n)  INIT_OPT_14(PREFIX,TYPE,DEFAULT,a,b,c,d,e,f,g,h,i,j,k,l,m,n); \
							          OPT_SET_START(PREFIX, TYPE) \
							          OPT_SET_BODY_14(PREFIX,a,b,c,d,e,f,g,h,i,j,k,l,m,n), \
								  OPT_SET_END

#define GENERIC_OPT_SET_15(PREFIX, TYPE, DEFAULT,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o) \
                                                                  INIT_OPT_15(PREFIX,TYPE,DEFAULT,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o);     \
                                                                  OPT_SET_START(PREFIX, TYPE) \
                                                                  OPT_SET_BODY_15(PREFIX,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o), \
                                                                  OPT_SET_END
#define GENERIC_OPT_SET_16(PREFIX, TYPE, DEFAULT,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p) \
                                                                  INIT_OPT_16(PREFIX,TYPE,DEFAULT,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p);	\
							          OPT_SET_START(PREFIX, TYPE) \
								  OPT_SET_BODY_16(PREFIX,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p), \
								  OPT_SET_END
#define GENERIC_OPT_SET_17(PREFIX, TYPE, DEFAULT,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q) \
                                                                  INIT_OPT_17(PREFIX,TYPE,DEFAULT,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q);	\
							          OPT_SET_START(PREFIX, TYPE) \
								  OPT_SET_BODY_17(PREFIX,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q), \
								  OPT_SET_END

#define DECLARE_BOOL_OPTS_1(OPT_1)					\
  GENERIC_OPT_SET_1( auth,	bool, BOOL_DEFAULT, OPT_1);		\
  GENERIC_OPT_SET_1( account,	bool, BOOL_DEFAULT, OPT_1);		\
  GENERIC_OPT_SET_1( password,	bool, BOOL_DEFAULT, OPT_1);		\
  GENERIC_OPT_SET_1( session,	bool, BOOL_DEFAULT, OPT_1);

#define DECLARE_BOOL_OPTS_2(OPT_1, OPT_2)			   \
  GENERIC_OPT_SET_2( auth,	bool, BOOL_DEFAULT, OPT_1, OPT_2); \
  GENERIC_OPT_SET_2( account,	bool, BOOL_DEFAULT, OPT_1, OPT_2); \
  GENERIC_OPT_SET_2( password,	bool, BOOL_DEFAULT, OPT_1, OPT_2); \
  GENERIC_OPT_SET_2( session,	bool, BOOL_DEFAULT, OPT_1, OPT_2);

#define DECLARE_BOOL_OPTS_3(OPT_1, OPT_2, OPT_3)			\
  GENERIC_OPT_SET_3( auth,	bool, BOOL_DEFAULT, OPT_1, OPT_2, OPT_3); \
  GENERIC_OPT_SET_3( account,	bool, BOOL_DEFAULT, OPT_1, OPT_2, OPT_3); \
  GENERIC_OPT_SET_3( password,	bool, BOOL_DEFAULT, OPT_1, OPT_2, OPT_3); \
  GENERIC_OPT_SET_3( session,	bool, BOOL_DEFAULT, OPT_1, OPT_2, OPT_3);

#define DECLARE_BOOL_OPTS_4(OPT_1, OPT_2, OPT_3, OPT_4)			\
  GENERIC_OPT_SET_4( auth,	bool, BOOL_DEFAULT, OPT_1, OPT_2, OPT_3, OPT_4); \
  GENERIC_OPT_SET_4( account,	bool, BOOL_DEFAULT, OPT_1, OPT_2, OPT_3, OPT_4); \
  GENERIC_OPT_SET_4( password,	bool, BOOL_DEFAULT, OPT_1, OPT_2, OPT_3, OPT_4); \
  GENERIC_OPT_SET_4( session,	bool, BOOL_DEFAULT, OPT_1, OPT_2, OPT_3, OPT_4);

#define DECLARE_BOOL_OPTS_5(OPT_1, OPT_2, OPT_3, OPT_4, OPT_5)		\
  GENERIC_OPT_SET_5( auth,	bool, BOOL_DEFAULT, OPT_1, OPT_2, OPT_3, OPT_4, OPT_5); \
  GENERIC_OPT_SET_5( account,	bool, BOOL_DEFAULT, OPT_1, OPT_2, OPT_3, OPT_4, OPT_5); \
  GENERIC_OPT_SET_5( password,	bool, BOOL_DEFAULT, OPT_1, OPT_2, OPT_3, OPT_4, OPT_5); \
  GENERIC_OPT_SET_5( session,	bool, BOOL_DEFAULT, OPT_1, OPT_2, OPT_3, OPT_4, OPT_5);

#define DECLARE_BOOL_OPTS_6(OPT_1, OPT_2, OPT_3, OPT_4, OPT_5, OPT_6)		\
  GENERIC_OPT_SET_6( auth,	bool, BOOL_DEFAULT, OPT_1, OPT_2, OPT_3, OPT_4, OPT_5, OPT_6); \
  GENERIC_OPT_SET_6( account,	bool, BOOL_DEFAULT, OPT_1, OPT_2, OPT_3, OPT_4, OPT_5, OPT_6); \
  GENERIC_OPT_SET_6( password,	bool, BOOL_DEFAULT, OPT_1, OPT_2, OPT_3, OPT_4, OPT_5, OPT_6); \
  GENERIC_OPT_SET_6( session,	bool, BOOL_DEFAULT, OPT_1, OPT_2, OPT_3, OPT_4, OPT_5, OPT_6);
#define DECLARE_BOOL_OPTS_7(OPT_1, OPT_2, OPT_3, OPT_4, OPT_5, OPT_6, OPT_7)	\
  GENERIC_OPT_SET_7( auth,	bool, BOOL_DEFAULT, OPT_1, OPT_2, OPT_3, OPT_4, OPT_5, OPT_6, OPT_7); \
  GENERIC_OPT_SET_7( account,	bool, BOOL_DEFAULT, OPT_1, OPT_2, OPT_3, OPT_4, OPT_5, OPT_6, OPT_7); \
  GENERIC_OPT_SET_7( password,	bool, BOOL_DEFAULT, OPT_1, OPT_2, OPT_3, OPT_4, OPT_5, OPT_6, OPT_7); \
  GENERIC_OPT_SET_7( session,	bool, BOOL_DEFAULT, OPT_1, OPT_2, OPT_3, OPT_4, OPT_5, OPT_6, OPT_7);
#define DECLARE_BOOL_OPTS_8(OPT_1, OPT_2, OPT_3, OPT_4, OPT_5, OPT_6, OPT_7, OPT_8) \
  GENERIC_OPT_SET_8( auth,	bool, BOOL_DEFAULT, OPT_1, OPT_2, OPT_3, OPT_4, OPT_5, OPT_6, OPT_7, OPT_8); \
  GENERIC_OPT_SET_8( account,	bool, BOOL_DEFAULT, OPT_1, OPT_2, OPT_3, OPT_4, OPT_5, OPT_6, OPT_7, OPT_8); \
  GENERIC_OPT_SET_8( password,	bool, BOOL_DEFAULT, OPT_1, OPT_2, OPT_3, OPT_4, OPT_5, OPT_6, OPT_7, OPT_8); \
  GENERIC_OPT_SET_8( session,	bool, BOOL_DEFAULT, OPT_1, OPT_2, OPT_3, OPT_4, OPT_5, OPT_6, OPT_7, OPT_8);
#define DECLARE_BOOL_OPTS_10(OPT_1, OPT_2, OPT_3, OPT_4, OPT_5, OPT_6, OPT_7, OPT_8, OPT_9, OPT_10) \
  GENERIC_OPT_SET_10( auth,	bool, BOOL_DEFAULT, OPT_1, OPT_2, OPT_3, OPT_4, OPT_5, OPT_6, OPT_7, OPT_8, OPT_9, OPT_10); \
  GENERIC_OPT_SET_10( account,	bool, BOOL_DEFAULT, OPT_1, OPT_2, OPT_3, OPT_4, OPT_5, OPT_6, OPT_7, OPT_8, OPT_9, OPT_10); \
  GENERIC_OPT_SET_10( password,	bool, BOOL_DEFAULT, OPT_1, OPT_2, OPT_3, OPT_4, OPT_5, OPT_6, OPT_7, OPT_8, OPT_9, OPT_10); \
  GENERIC_OPT_SET_10( session,	bool, BOOL_DEFAULT, OPT_1, OPT_2, OPT_3, OPT_4, OPT_5, OPT_6, OPT_7, OPT_8, OPT_9, OPT_10);
#define DECLARE_BOOL_OPTS_11(OPT_1, OPT_2, OPT_3, OPT_4, OPT_5, OPT_6, OPT_7, OPT_8, OPT_9, OPT_10, OPT_11) \
  GENERIC_OPT_SET_11( auth,	bool, BOOL_DEFAULT, OPT_1, OPT_2, OPT_3, OPT_4, OPT_5, OPT_6, OPT_7, OPT_8, OPT_9, OPT_10, OPT_11); \
  GENERIC_OPT_SET_11( account,	bool, BOOL_DEFAULT, OPT_1, OPT_2, OPT_3, OPT_4, OPT_5, OPT_6, OPT_7, OPT_8, OPT_9, OPT_10, OPT_11); \
  GENERIC_OPT_SET_11( password,	bool, BOOL_DEFAULT, OPT_1, OPT_2, OPT_3, OPT_4, OPT_5, OPT_6, OPT_7, OPT_8, OPT_9, OPT_10, OPT_11); \
  GENERIC_OPT_SET_11( session,	bool, BOOL_DEFAULT, OPT_1, OPT_2, OPT_3, OPT_4, OPT_5, OPT_6, OPT_7, OPT_8, OPT_9, OPT_10, OPT_11);

#define DECLARE_BOOL_OPTS_15(OPT_1, OPT_2, OPT_3, OPT_4, OPT_5, OPT_6, OPT_7, OPT_8, OPT_9, OPT_10, OPT_11, OPT_12, OPT_13, OPT_14, OPT_15) \
  GENERIC_OPT_SET_15( auth,     bool, BOOL_DEFAULT, OPT_1, OPT_2, OPT_3, OPT_4, OPT_5, OPT_6, OPT_7, OPT_8, OPT_9, OPT_10, OPT_11, OPT_12, OPT_13, OPT_14, OPT_15); \
  GENERIC_OPT_SET_15( account,  bool, BOOL_DEFAULT, OPT_1, OPT_2, OPT_3, OPT_4, OPT_5, OPT_6, OPT_7, OPT_8, OPT_9, OPT_10, OPT_11, OPT_12, OPT_13, OPT_14, OPT_15); \
  GENERIC_OPT_SET_15( password, bool, BOOL_DEFAULT, OPT_1, OPT_2, OPT_3, OPT_4, OPT_5, OPT_6, OPT_7, OPT_8, OPT_9, OPT_10, OPT_11, OPT_12, OPT_13, OPT_14, OPT_15); \
  GENERIC_OPT_SET_15( session,  bool, BOOL_DEFAULT, OPT_1, OPT_2, OPT_3, OPT_4, OPT_5, OPT_6, OPT_7, OPT_8, OPT_9, OPT_10, OPT_11, OPT_12, OPT_13, OPT_14, OPT_15);
#define DECLARE_BOOL_OPTS_16(OPT_1, OPT_2, OPT_3, OPT_4, OPT_5, OPT_6, OPT_7, OPT_8, OPT_9, OPT_10, OPT_11, OPT_12, OPT_13, OPT_14, OPT_15, OPT_16) \
  GENERIC_OPT_SET_16( auth,	bool, BOOL_DEFAULT, OPT_1, OPT_2, OPT_3, OPT_4, OPT_5, OPT_6, OPT_7, OPT_8, OPT_9, OPT_10, OPT_11, OPT_12, OPT_13, OPT_14, OPT_15, OPT_16); \
  GENERIC_OPT_SET_16( account,	bool, BOOL_DEFAULT, OPT_1, OPT_2, OPT_3, OPT_4, OPT_5, OPT_6, OPT_7, OPT_8, OPT_9, OPT_10, OPT_11, OPT_12, OPT_13, OPT_14, OPT_15, OPT_16); \
  GENERIC_OPT_SET_16( password,	bool, BOOL_DEFAULT, OPT_1, OPT_2, OPT_3, OPT_4, OPT_5, OPT_6, OPT_7, OPT_8, OPT_9, OPT_10, OPT_11, OPT_12, OPT_13, OPT_14, OPT_15, OPT_16); \
  GENERIC_OPT_SET_16( session,	bool, BOOL_DEFAULT, OPT_1, OPT_2, OPT_3, OPT_4, OPT_5, OPT_6, OPT_7, OPT_8, OPT_9, OPT_10, OPT_11, OPT_12, OPT_13, OPT_14, OPT_15, OPT_16);

#define DECLARE_STRING_OPTS_0 DECLARE_STRING_OPTS_1( empty )
#define DECLARE_STRING_OPTS_1(OPT_1)				 \
  GENERIC_OPT_SET_1( auth,	string, STRING_DEFAULT, OPT_1 ); \
  GENERIC_OPT_SET_1( account,	string, STRING_DEFAULT, OPT_1 ); \
  GENERIC_OPT_SET_1( password,	string, STRING_DEFAULT, OPT_1 ); \
  GENERIC_OPT_SET_1( session,	string, STRING_DEFAULT, OPT_1 );

#define DECLARE_STRING_OPTS_2(OPT_1,OPT_2)				\
  GENERIC_OPT_SET_2( auth,	string, STRING_DEFAULT, OPT_1, OPT_2 );	\
  GENERIC_OPT_SET_2( account,	string, STRING_DEFAULT, OPT_1, OPT_2 );	\
  GENERIC_OPT_SET_2( password,	string, STRING_DEFAULT, OPT_1, OPT_2 );	\
  GENERIC_OPT_SET_2( session,	string, STRING_DEFAULT, OPT_1, OPT_2 );

#define DECLARE_STRING_OPTS_3(OPT_1,OPT_2,OPT_3)			\
  GENERIC_OPT_SET_3( auth,	string, STRING_DEFAULT,OPT_1,OPT_2,OPT_3); \
  GENERIC_OPT_SET_3( account,	string, STRING_DEFAULT,OPT_1,OPT_2,OPT_3); \
  GENERIC_OPT_SET_3( password,	string, STRING_DEFAULT,OPT_1,OPT_2,OPT_3); \
  GENERIC_OPT_SET_3( session,	string, STRING_DEFAULT,OPT_1,OPT_2,OPT_3);

#define DECLARE_STRING_OPTS_4(OPT_1,OPT_2,OPT_3,OPT_4)    \
  GENERIC_OPT_SET_4( auth,      string, STRING_DEFAULT, OPT_1, OPT_2,OPT_3,OPT_4);       \
  GENERIC_OPT_SET_4( account,   string, STRING_DEFAULT, OPT_1, OPT_2,OPT_3,OPT_4);       \
  GENERIC_OPT_SET_4( password,  string, STRING_DEFAULT, OPT_1, OPT_2,OPT_3,OPT_4);       \
  GENERIC_OPT_SET_4( session,   string, STRING_DEFAULT, OPT_1, OPT_2,OPT_3,OPT_4);

#define DECLARE_STRING_OPTS_5(OPT_1,OPT_2,OPT_3,OPT_4,OPT_5)	\
  GENERIC_OPT_SET_5( auth,	string, STRING_DEFAULT, OPT_1, OPT_2,OPT_3,OPT_4,OPT_5);	\
  GENERIC_OPT_SET_5( account,	string, STRING_DEFAULT, OPT_1, OPT_2,OPT_3,OPT_4,OPT_5);	\
  GENERIC_OPT_SET_5( password,	string, STRING_DEFAULT, OPT_1, OPT_2,OPT_3,OPT_4,OPT_5);	\
  GENERIC_OPT_SET_5( session,	string, STRING_DEFAULT, OPT_1, OPT_2,OPT_3,OPT_4,OPT_5);

#define DECLARE_STRING_OPTS_6(OPT_1,OPT_2,OPT_3,OPT_4,OPT_5,OPT_6)	\
  GENERIC_OPT_SET_6( auth,	string, STRING_DEFAULT, OPT_1, OPT_2,OPT_3,OPT_4,OPT_5,OPT_6 );	\
  GENERIC_OPT_SET_6( account,	string, STRING_DEFAULT, OPT_1, OPT_2,OPT_3,OPT_4,OPT_5,OPT_6 );	\
  GENERIC_OPT_SET_6( password,	string, STRING_DEFAULT, OPT_1, OPT_2,OPT_3,OPT_4,OPT_5,OPT_6 );	\
  GENERIC_OPT_SET_6( session,	string, STRING_DEFAULT, OPT_1, OPT_2,OPT_3,OPT_4,OPT_5,OPT_6 );

#define DECLARE_STRING_OPTS_7(OPT_1,OPT_2,OPT_3,OPT_4,OPT_5,OPT_6,OPT_7)	\
  GENERIC_OPT_SET_7( auth,	string, STRING_DEFAULT, OPT_1, OPT_2,OPT_3,OPT_4,OPT_5,OPT_6,OPT_7 ); \
  GENERIC_OPT_SET_7( account,	string, STRING_DEFAULT, OPT_1, OPT_2,OPT_3,OPT_4,OPT_5,OPT_6,OPT_7 ); \
  GENERIC_OPT_SET_7( password,	string, STRING_DEFAULT, OPT_1, OPT_2,OPT_3,OPT_4,OPT_5,OPT_6,OPT_7 ); \
  GENERIC_OPT_SET_7( session,	string, STRING_DEFAULT, OPT_1, OPT_2,OPT_3,OPT_4,OPT_5,OPT_6,OPT_7 );

#define DECLARE_STRING_OPTS_8(OPT_1,OPT_2,OPT_3,OPT_4,OPT_5,OPT_6,OPT_7,OPT_8)        \
  GENERIC_OPT_SET_8( auth,      string, STRING_DEFAULT, OPT_1, OPT_2,OPT_3,OPT_4,OPT_5,OPT_6,OPT_7,OPT_8 ); \
  GENERIC_OPT_SET_8( account,   string, STRING_DEFAULT, OPT_1, OPT_2,OPT_3,OPT_4,OPT_5,OPT_6,OPT_7,OPT_8 ); \
  GENERIC_OPT_SET_8( password,  string, STRING_DEFAULT, OPT_1, OPT_2,OPT_3,OPT_4,OPT_5,OPT_6,OPT_7,OPT_8 ); \
  GENERIC_OPT_SET_8( session,   string, STRING_DEFAULT, OPT_1, OPT_2,OPT_3,OPT_4,OPT_5,OPT_6,OPT_7,OPT_8 );

#define DECLARE_STRING_OPTS_11(OPT_1,OPT_2,OPT_3,OPT_4,OPT_5,OPT_6,OPT_7,OPT_8,OPT_9,OPT_10,OPT_11) \
  GENERIC_OPT_SET_11( auth,	string, STRING_DEFAULT, OPT_1, OPT_2,OPT_3,OPT_4,OPT_5,OPT_6,OPT_7,OPT_8,OPT_9,OPT_10,OPT_11 ); \
  GENERIC_OPT_SET_11( account,	string, STRING_DEFAULT, OPT_1, OPT_2,OPT_3,OPT_4,OPT_5,OPT_6,OPT_7,OPT_8,OPT_9,OPT_10,OPT_11 ); \
  GENERIC_OPT_SET_11( password,	string, STRING_DEFAULT, OPT_1, OPT_2,OPT_3,OPT_4,OPT_5,OPT_6,OPT_7,OPT_8,OPT_9,OPT_10,OPT_11 ); \
  GENERIC_OPT_SET_11( session,	string, STRING_DEFAULT, OPT_1, OPT_2,OPT_3,OPT_4,OPT_5,OPT_6,OPT_7,OPT_8,OPT_9,OPT_10,OPT_11 );

#define DECLARE_STRING_OPTS_14(OPT_1,OPT_2,OPT_3,OPT_4,OPT_5,OPT_6,OPT_7,OPT_8,OPT_9,OPT_10,OPT_11,OPT_12,OPT_13,OPT_14) \
  GENERIC_OPT_SET_14( auth, string, STRING_DEFAULT, OPT_1, OPT_2,OPT_3,OPT_4,OPT_5,OPT_6,OPT_7,OPT_8,OPT_9,OPT_10,OPT_11,OPT_12,OPT_13,OPT_14 ); \
  GENERIC_OPT_SET_14( account,	string, STRING_DEFAULT, OPT_1, OPT_2,OPT_3,OPT_4,OPT_5,OPT_6,OPT_7,OPT_8,OPT_9,OPT_10,OPT_11,OPT_12,OPT_13,OPT_14 ); \
  GENERIC_OPT_SET_14( password,	string, STRING_DEFAULT, OPT_1, OPT_2,OPT_3,OPT_4,OPT_5,OPT_6,OPT_7,OPT_8,OPT_9,OPT_10,OPT_11,OPT_12,OPT_13,OPT_14 ); \
  GENERIC_OPT_SET_14( session,	string, STRING_DEFAULT, OPT_1, OPT_2,OPT_3,OPT_4,OPT_5,OPT_6,OPT_7,OPT_8,OPT_9,OPT_10,OPT_11,OPT_12,OPT_13,OPT_14 );

#define DECLARE_STRING_OPTS_15(OPT_1,OPT_2,OPT_3,OPT_4,OPT_5,OPT_6,OPT_7,OPT_8,OPT_9,OPT_10,OPT_11,OPT_12,OPT_13,OPT_14,OPT_15) \
  GENERIC_OPT_SET_15( auth, string, STRING_DEFAULT, OPT_1, OPT_2,OPT_3,OPT_4,OPT_5,OPT_6,OPT_7,OPT_8,OPT_9,OPT_10,OPT_11,OPT_12,OPT_13,OPT_14,OPT_15 ); \
  GENERIC_OPT_SET_15( account,  string, STRING_DEFAULT, OPT_1, OPT_2,OPT_3,OPT_4,OPT_5,OPT_6,OPT_7,OPT_8,OPT_9,OPT_10,OPT_11,OPT_12,OPT_13,OPT_14,OPT_15 ); \
  GENERIC_OPT_SET_15( password, string, STRING_DEFAULT, OPT_1, OPT_2,OPT_3,OPT_4,OPT_5,OPT_6,OPT_7,OPT_8,OPT_9,OPT_10,OPT_11,OPT_12,OPT_13,OPT_14,OPT_15 ); \
  GENERIC_OPT_SET_15( session,  string, STRING_DEFAULT, OPT_1, OPT_2,OPT_3,OPT_4,OPT_5,OPT_6,OPT_7,OPT_8,OPT_9,OPT_10,OPT_11,OPT_12,OPT_13,OPT_14,OPT_15 );

#define DECLARE_STRING_OPTS_16(OPT_1,OPT_2,OPT_3,OPT_4,OPT_5,OPT_6,OPT_7,OPT_8,OPT_9,OPT_10,OPT_11,OPT_12,OPT_13,OPT_14,OPT_15,OPT_16) \
  GENERIC_OPT_SET_16( auth, string, STRING_DEFAULT, OPT_1, OPT_2,OPT_3,OPT_4,OPT_5,OPT_6,OPT_7,OPT_8,OPT_9,OPT_10,OPT_11,OPT_12,OPT_13,OPT_14,OPT_15,OPT_16 ); \
  GENERIC_OPT_SET_16( account,  string, STRING_DEFAULT, OPT_1, OPT_2,OPT_3,OPT_4,OPT_5,OPT_6,OPT_7,OPT_8,OPT_9,OPT_10,OPT_11,OPT_12,OPT_13,OPT_14,OPT_15,OPT_16 ); \
  GENERIC_OPT_SET_16( password, string, STRING_DEFAULT, OPT_1, OPT_2,OPT_3,OPT_4,OPT_5,OPT_6,OPT_7,OPT_8,OPT_9,OPT_10,OPT_11,OPT_12,OPT_13,OPT_14,OPT_15,OPT_16 ); \
  GENERIC_OPT_SET_16( session,  string, STRING_DEFAULT, OPT_1, OPT_2,OPT_3,OPT_4,OPT_5,OPT_6,OPT_7,OPT_8,OPT_9,OPT_10,OPT_11,OPT_12,OPT_13,OPT_14,OPT_15,OPT_16 );


#define DECLARE_STRING_OPTS_17(OPT_1,OPT_2,OPT_3,OPT_4,OPT_5,OPT_6,OPT_7,OPT_8,OPT_9,OPT_10,OPT_11,OPT_12,OPT_13,OPT_14,OPT_15,OPT_16,OPT_17) \
  GENERIC_OPT_SET_17( auth, string, STRING_DEFAULT, OPT_1, OPT_2,OPT_3,OPT_4,OPT_5,OPT_6,OPT_7,OPT_8,OPT_9,OPT_10,OPT_11,OPT_12,OPT_13,OPT_14,OPT_15,OPT_16,OPT_17 ); \
  GENERIC_OPT_SET_17( account,  string, STRING_DEFAULT, OPT_1, OPT_2,OPT_3,OPT_4,OPT_5,OPT_6,OPT_7,OPT_8,OPT_9,OPT_10,OPT_11,OPT_12,OPT_13,OPT_14,OPT_15,OPT_16,OPT_17 ); \
  GENERIC_OPT_SET_17( password, string, STRING_DEFAULT, OPT_1, OPT_2,OPT_3,OPT_4,OPT_5,OPT_6,OPT_7,OPT_8,OPT_9,OPT_10,OPT_11,OPT_12,OPT_13,OPT_14,OPT_15,OPT_16,OPT_17 ); \
  GENERIC_OPT_SET_17( session,  string, STRING_DEFAULT, OPT_1, OPT_2,OPT_3,OPT_4,OPT_5,OPT_6,OPT_7,OPT_8,OPT_9,OPT_10,OPT_11,OPT_12,OPT_13,OPT_14,OPT_15,OPT_16,OPT_17 );

#define DECLARE_OPT_SETS						\
  static option_set_t auth_opts	    = { auth_bool_opts,	    auth_string_opts,	  &is_enabled, &enable, &get_opt, &set_opt }; \
  static option_set_t account_opts  = { account_bool_opts,  account_string_opts,  &is_enabled, &enable, &get_opt, &set_opt }; \
  static option_set_t password_opts = { password_bool_opts, password_string_opts, &is_enabled, &enable, &get_opt, &set_opt }; \
  static option_set_t session_opts  = { session_bool_opts,  session_string_opts,  &is_enabled, &enable, &get_opt, &set_opt }; \
  static option_set_t *opt_sets[]   = { &auth_opts, &account_opts, &password_opts, &session_opts, NULL }


#define WRITE_CONFIG_OPTIONS				\
  bool_option_t **cur_bool_opt = opt_set->bool_opts;	\
  string_option_t **cur_str_opt = opt_set->string_opts;	\
							\
  while (*cur_bool_opt != NULL)				\
    {								\
      if ((strcmp ((*cur_bool_opt)->key, "is_enabled") != 0)	\
	  && ((*cur_bool_opt)->value == TRUE))			\
	fprintf (fp, "%s ", (*cur_bool_opt)->key);		\
      cur_bool_opt++;						\
    }								\
								\
  while (*cur_str_opt != NULL)					\
    {								\
      if ((strcmp ((*cur_str_opt)->key, "empty") != 0)		\
	  && ((*cur_str_opt)->value))					\
	fprintf (fp, "%s=%s ", (*cur_str_opt)->key, (*cur_str_opt)->value); \
      cur_str_opt++;							\
    }									\
									\
  fprintf (fp, "\n");

#define PARSE_CONFIG_OPTIONS \
  while (args && strlen (args) > 0)		\
    {						\
      char *key = strsep (&args, " \t");	\
      char *val;				\
						\
      if (args)					\
	while (isspace ((int) *args))		\
	  ++args;				\
						\
      if (strcmp (key, "use_first_pass") == 0)	\
	{ /* will be ignored */ }			\
      else if (strcmp (key, "try_first_pass") == 0)	\
	{ /* will be ignored */ }			\
      else if (strcmp (key, "use_authtok") == 0)		\
        { /* will be ignored */ }			\
      else if (NULL != (val = strchr (key, '=')))	\
	{								\
	  *val++='\0';							\
	  if (opt_set->set_opt (opt_set, key, strdup (val)) == FALSE)	\
	    print_unknown_option_error (this->name, key);		\
	}								\
      else if (opt_set->enable (opt_set, key, TRUE) == FALSE)		\
	print_unknown_option_error (this->name, key);			\
    }

#define PRINT_ARGS(modname) \
static void			\
print_args (pam_module_t *this)		\
{                                        \
  option_set_t *opt_set = this->get_opt_set (this, AUTH); \
  bool_option_t **cur_bool_opt = opt_set->bool_opts; \
  string_option_t **cur_str_opt = opt_set->string_opts; \
\
  printf ("   --%s\n", modname);			\
\
  while (*cur_bool_opt != NULL) \
    { \
      if (strcmp ((*cur_bool_opt)->key, "is_enabled") != 0) \
	printf ("   --%s-%s\n", modname, (*cur_bool_opt)->key);	\
      cur_bool_opt++; \
    } \
\
  while (*cur_str_opt != NULL) \
    { \
      if (strcmp ((*cur_str_opt)->key, "empty") != 0) \
	printf ("   --%s-%s=<value>\n", modname, (*cur_str_opt)->key); \
      cur_str_opt++; \
    } \
}


#define PRINT_XMLHELP(modname) \
static module_helptext_t *search_key (pam_module_t *this, const char *key) \
{									\
  module_helptext_t *ptr = this->helptxt;				\
									\
  while (ptr->option != NULL)						\
    {									\
      if (strcmp (key, ptr->option) == 0)				\
	return ptr;							\
      else								\
	++ptr;								\
    }									\
  return NULL;								\
}									\
									\
static void								\
print_xmlhelp (pam_module_t *this)					\
{									\
  option_set_t *opt_set = this->get_opt_set (this, AUTH);		\
  bool_option_t **cur_bool_opt = opt_set->bool_opts;			\
  string_option_t **cur_str_opt = opt_set->string_opts;			\
  module_helptext_t *helptxt;						\
									\
  helptxt = search_key (this, "");					\
									\
  printf ("          <varlistentry>\n");				\
  printf ("            <term><option>--%s</option></term>\n", modname);	\
  printf ("            <listitem>\n");					\
  printf ("              <para>\n");					\
  if (helptxt && helptxt->helptxt)							\
    printf ("                %s\n", helptxt->helptxt);			\
  else									\
    printf ("                Enable/Disable %s\n", this->name);	\
  printf ("              </para>\n");					\
  printf ("            </listitem>\n");					\
  printf ("          </varlistentry>\n");				\
									\
  while (*cur_bool_opt != NULL)						\
    {									\
      if (strcmp ((*cur_bool_opt)->key, "is_enabled") != 0)		\
	{								\
	  helptxt = search_key (this, (*cur_bool_opt)->key);		\
									\
	  printf ("          <varlistentry>\n");			\
	  printf ("            <term><option>--%s-%s</option></term>\n", \
		  modname, (*cur_bool_opt)->key);			\
	  printf ("            <listitem>\n");				\
	  printf ("              <para>\n");				\
	  if (helptxt && helptxt->helptxt)				\
	    printf ("                %s\n", helptxt->helptxt);		\
	  else								\
	    printf ("                Add <option>%s</option> option to all %s invocations.\n", (*cur_bool_opt)->key, this->name); \
	  printf ("              </para>\n");				\
	  printf ("            </listitem>\n");				\
	  printf ("          </varlistentry>\n");			\
	}								\
									\
      cur_bool_opt++;							\
    }									\
  while (*cur_str_opt != NULL)						\
    {									\
      if ((strcmp ((*cur_str_opt)->key, "is_enabled") != 0) &&		\
	(strcmp ((*cur_str_opt)->key, "empty") != 0))			\
	{								\
	  helptxt = search_key (this, (*cur_str_opt)->key);		\
									\
	  printf ("          <varlistentry>\n");			\
	  if (helptxt && helptxt->arg)					\
	    printf ("            <term><option>--%s-%s=</option><replaceable>%s</replaceable></term>\n", \
		    modname, (*cur_str_opt)->key, helptxt->arg);	\
	  else								\
	    printf ("            <term><option>--%s-%s=</option><replaceable>value</replaceable></term>\n", \
		    modname, (*cur_str_opt)->key);			\
          printf ("            <listitem>\n");				\
          printf ("              <para>\n");			       	\
	  if (helptxt && helptxt->helptxt)				\
	    printf ("                %s\n", helptxt->helptxt);		\
	  else						\
	    printf ("                Add <option>%s=</option><replaceable>value</replaceable> option to %s.\n", (*cur_str_opt)->key, this->name); \
	  printf ("              </para>\n");				\
	  printf ("            </listitem>\n");				\
	  printf ("          </varlistentry>\n");			\
	}								\
									\
      cur_str_opt++;							\
    }									\
}

/*
 * Return values:
 * 0: Ok
 * 1: Option does not exist
 * 2: Ohter error, error message already printed
 */


#define GETOPT_START_1(type) \
static int \
getopt (pam_module_t *this, char *opt, char *optarg, global_opt_t *g_opt) \
{ \
  option_set_t *opt_set; \
\
  if (debug) \
    printf ("**** %s->getopt: '%s'='%s'\n", this->name, opt, optarg); \
\
  if (strcmp ("", opt) == 0) \
    { \
      if (g_opt->m_query) \
	this->print_module (this); \
      else \
	{ \
	  if (!g_opt->m_delete && \
	      check_for_pam_module (this->name, g_opt->force, 0) != 0) \
	    return 2; \
	  opt_set = this->get_opt_set (this, type); \
	  opt_set->enable (opt_set, "is_enabled", g_opt->opt_val); \
	} \
    }


#define GETOPT_END_1(type)						\
  else									\
    {									\
      opt_set = this->get_opt_set (this, type);				\
      if (opt_set->enable (opt_set, opt, g_opt->opt_val) == FALSE)	\
	{								\
	if (optarg && strlen(optarg) > 0 )	\
	    {								\
	      if (opt_set->set_opt (opt_set, opt, strdup(optarg)) == FALSE) \
		return 1;						\
	    }								\
	  else								\
	    {								\
	      if (opt_set->set_opt (opt_set, opt, NULL) == FALSE)	\
		return 1;						\
	    }								\
	}								\
    }									\
									\
  return 0;								\
}


#define GETOPT_START_ALL \
static int \
getopt (pam_module_t *this, char *opt, char *optarg, global_opt_t *g_opt) \
{ \
  option_set_t *opt_set; \
\
  if (debug) \
    printf ("**** %s->getopt: '%s'='%s'\n", this->name, opt, optarg); \
\
  if (strcmp ("", opt) == 0) \
    { \
      if (g_opt->m_query) \
	this->print_module (this); \
      else \
	{ \
	  if (!g_opt->m_delete && \
	      check_for_pam_module (this->name, g_opt->force, 0) != 0) \
	    return 2; \
	  opt_set = this->get_opt_set (this, ACCOUNT); \
	  opt_set->enable (opt_set, "is_enabled", g_opt->opt_val); \
	  opt_set = this->get_opt_set (this, AUTH); \
	  opt_set->enable (opt_set, "is_enabled", g_opt->opt_val); \
	  opt_set = this->get_opt_set (this, PASSWORD); \
	  opt_set->enable (opt_set, "is_enabled", g_opt->opt_val); \
	  opt_set = this->get_opt_set (this, SESSION); \
	  opt_set->enable (opt_set, "is_enabled", g_opt->opt_val); \
	} \
    }


#define GETOPT_END_ALL						\
  else									\
    {									\
      write_type_t type;						\
									\
    for (type = AUTH; type <= SESSION; type++)				\
	{								\
	  opt_set = this->get_opt_set (this, type);			\
	  if (opt_set->enable (opt_set, opt, g_opt->opt_val) == FALSE)	\
	    {								\
	if (optarg && strlen(optarg) > 0 )		\
		{							\
		  if (opt_set->set_opt (opt_set, opt, strdup(optarg)) == FALSE) \
		    return 1;						\
		}							\
	      else							\
		{							\
		  if (opt_set->set_opt (opt_set, opt, NULL) == FALSE)	\
		    return 1;						\
		}							\
	    }								\
	}								\
    }									\
									\
  return 0;								\
}

/**
 * @enum write_type
 * @brief Defines the service type.
 */

/**
 * @typedef write_type_t
 * @brief Creates a type for write_type.
 */
typedef enum write_type {
  AUTH=0,
  ACCOUNT,
  PASSWORD,
  SESSION
} write_type_t;

/**
 * @struct configurable_module
 * @brief Fields parsed from config
 *
 * This struct defines the values parsed from a configurable pam module.
 */
/**
 * @typedef configurable_module_t
 * @brief Creates a type for configurable_module
 */
typedef struct configurable_module {
  char *modname;
  /* Raw lines for stacks; if NULL, stack is not emitted */
  char *auth_line;
  char *account_line;
  char *password_line;
  char *session_line;
} configurable_module_t;

/**
 * @struct pam_module
 * @brief Layout of a pam-config module.
 *
 * This struct defines the layout of a pam-config module.
 */
/**
 * @typedef pam_module_t
 * @brief Creates a type for pam_module
 */
typedef struct pam_module {
	char *name;		      /**< The name of the module. */
	option_set_t **option_sets;   /**< Pointer to a NULL terminated list of options_set_t's. */
        module_helptext_t *helptxt;  /**< Pointer to struct with help text. */
	/** Pointer to parse function. */
	int (*parse_config)(struct pam_module *this, char *arguments, write_type_t type);
	/** Pointer to print function, used for debuging output. */
	int (*print_module)(struct pam_module *this);
	/** Pointer to write function */
	int (*write_config)(struct pam_module *this, enum write_type op,
			    FILE *fp);
	/** Accessor function for option_sets. */
	option_set_t* (*get_opt_set) (struct pam_module *this,
				      write_type_t op);
	/** Pointer to getopt function */
        int (*getopt)(struct pam_module *this, char *opt, char *arg,
		      global_opt_t *g_opt);
	/** Pointer to print function, used for commandline argument output. */
	void (*print_args)(struct pam_module *this);
	/** Pointer to print function, used for XML output. */
	void (*print_xmlhelp)(struct pam_module *this);

	/** Optional user supplied configuration **/
	configurable_module_t *config;

	/** Stack priorities **/
	int priority_auth;
	int priority_account;
	int priority_password;
	int priority_session;
} pam_module_t;

/**
 *  @brief Modules need access to these lists to check, which other modules
 *  are enabled
 *  */
extern pam_module_t **common_module_list;
extern pam_module_t *service_module_list[];

/* default handlers */

/**
 * @brief default parse_option function which just returns false.
 *
 * @param this the module instance
 * @param arguments line of arguments
 * @param type service type
 */
int def_parse_config (pam_module_t *this, char *arguments, write_type_t type);

/**
 * @brief default printing function.
 *
 * default print_module function which just prints out the module
 * name. it can do this, because we pass it a reference to the
 * pam_module_t as first argument.
 *
 * @param this the module instance
 */
int def_print_module (pam_module_t *this);

/**
 * @brief default write out function.
 *
 * Just prints it the module name to stdout.
 *
 * @param this the module instance
 * @param op service type
 * @param fp not used
 *
 * @return
 */
int def_write_config (pam_module_t *this, enum write_type op, FILE *fp);

/**
 * @brief Accessor function.
 *
 * Used to access the \a option_set_t contained in the module instance
 * specified by \a this and \a op.
 *
 * @param this the module instance
 * @param op service type
 *
 * @return The option_set_t specified by \a this and \a op.
 */
option_set_t* get_opt_set( pam_module_t *this, write_type_t op );


/**
 * @brief Look up a pam-config module by its name.
 *
 * Searches through \a module_list to find a match for the module named
 * by the second argument.
 *
 * @param module_list the module_list to search
 * @param module the name of the module to look for
 *
 * @return A pointer to the pam-config module if found, NULL
 * otherwise.
 */
pam_module_t* lookup( pam_module_t **module_list, const char *module );

/**
 * @brief Converts a write_type_t to its string representation.
 *
 * @param wt the write_type_t to convert
 *
 * @return string representation of \a wt
 */
const char* type2string (write_type_t wt);

/**
 * @brief Searches through \a module_list and calls print_module()
 * on \a module if found.
 *
 * @param module_list Pointer to the list of modules to search.
 * @param module Name of module to look for.
 */
void print_module_config (pam_module_t **module_list, const char *module);

/**
 * @brief Check if \a module is enabled in \a module_list and
 * service type \a op.
 *
 * @param *module_list the list of modules to searcg
 * @param module the module to look for
 * @param op the service type of interest
 *
 * @return TRUE if found, FALSE otherwise
 */
int is_module_enabled (pam_module_t **module_list, const char *module,
		       write_type_t op);

/**
 * @brief Prints out a error message to stderr for \a module and \a
 * option.
 *
 * @param module the module instance
 * @param option the unknown option
 */
void print_unknown_option_error (const char *module, const char *option);

/**
 * @brief Searches through \a module_list and calls getopt()
 * on \a module if found.
 *
 * @param module_list Pointer to the list of modules to search.
 * @param optarg Pointer to the option
 * @param opt Global commandline options
 *
 * @return 0 if module option is valid, 1 otherwise.
 */
int module_getopt (pam_module_t **module_list, const char *optarg,
		   global_opt_t *opt);

/**
 * @brief command line argument printing function.
 *
 * Prints all commandline options a module in the list understands
 *
 * @param module_list the list of modules
 */
void print_module_args (pam_module_t **module_list);

/**
 * @brief command line argument in XML printing function.
 *
 * Prints all commandline options a module in the list understands
 * in XML format to generate the manual page.
 *
 * @param module_list the list of modules
 */
void print_module_xmlhelp (pam_module_t **module_list);

/**
 * @brief default config file parser
 *
 * Generic parser useable for most PAM modules
 *
 * @param this the module instance
 * @param args arguments for PAM module
 * @param type service type
 *
 * @return
 */
int def_parse_config (pam_module_t *this, char *args, write_type_t type);

/**
 * @brief print debug message for write functions
 *
 * Generic function to print information about writing module data.
 *
 * @param this the module instance
 * @param type service type
 */
void debug_write_call (pam_module_t *this, enum write_type type);

/**
 * @brief free an instance of a configurable_module_t
 *
 * Function for freeing instances of configurable_module_t allocated for
 * configurable pam modules.
 *
 * @param sp the configurable_module_t to be freed
 */
void configurable_module_free(configurable_module_t *sp);

#endif
