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
 * Copyright (C) 2007 - Thorsten Kukuk, Sven Schober
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

#define DECLARE_STRING_OPTS_6(OPT_1,OPT_2,OPT_3,OPT_4,OPT_5,OPT_6)	\
  GENERIC_OPT_SET_6( auth,	string, STRING_DEFAULT, OPT_1, OPT_2,OPT_3,OPT_4,OPT_5,OPT_6 );	\
  GENERIC_OPT_SET_6( account,	string, STRING_DEFAULT, OPT_1, OPT_2,OPT_3,OPT_4,OPT_5,OPT_6 );	\
  GENERIC_OPT_SET_6( password,	string, STRING_DEFAULT, OPT_1, OPT_2,OPT_3,OPT_4,OPT_5,OPT_6 );	\
  GENERIC_OPT_SET_6( session,	string, STRING_DEFAULT, OPT_1, OPT_2,OPT_3,OPT_4,OPT_5,OPT_6 );


#define DECLARE_OPT_SETS						\
  static option_set_t auth_opts	    = { auth_bool_opts,	    auth_string_opts,	  &is_enabled, &enable, &get_opt, &set_opt }; \
  static option_set_t account_opts  = { account_bool_opts,  account_string_opts,  &is_enabled, &enable, &get_opt, &set_opt }; \
  static option_set_t password_opts = { password_bool_opts, password_string_opts, &is_enabled, &enable, &get_opt, &set_opt }; \
  static option_set_t session_opts  = { session_bool_opts,  session_string_opts,  &is_enabled, &enable, &get_opt, &set_opt }; \
  static option_set_t *opt_sets[]   = { &auth_opts, &account_opts, &password_opts, &session_opts, NULL }


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
	/** Pointer to parse function. */
	int (*parse_config)(struct pam_module *this, char *arguments, write_type_t type);
	/** Pointer to print function, used for debuging output. */
	int (*print_module)(struct pam_module *this);
	/** Pointer to write function */
	int (*write_config)(struct pam_module *this, enum write_type op, FILE *fp);
	/** Accessor function for option_sets. */
	option_set_t* (*get_opt_set) (struct pam_module *this, write_type_t op);
} pam_module_t;

/**
 *  @brief Modules need access to this to check, which other modules
 *  are enabled 
 *  */
extern pam_module_t *common_module_list[];

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


#endif
