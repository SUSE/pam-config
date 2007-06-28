#ifndef _PAM_MODULES_H_
#define _PAM_MODULES_H_ 1

#include <stdio.h>
#include <stdint.h>
#include "option_set.h"

#define TRUE  1
#define FALSE 0

#define STRING_DEFAULT	NULL
#define BOOL_DEFAULT	FALSE

#define DEBUG(args...) fprintf( stderr, args )

#define OPT_NAME(PREFIX, NAME) PREFIX ## _opt_ ## NAME

#define INIT_OPT_1(PREFIX,TYPE,DEFAULT,a)	static TYPE ## _option_t OPT_NAME(PREFIX,a) = { #a, DEFAULT }
#define INIT_OPT_2(PREFIX,TYPE,DEFAULT,a,b)	INIT_OPT_1(PREFIX,TYPE,DEFAULT,a);    INIT_OPT_1(PREFIX,TYPE,DEFAULT,b)
#define INIT_OPT_3(PREFIX,TYPE,DEFAULT,a,b,c)	INIT_OPT_2(PREFIX,TYPE,DEFAULT,a,b);  INIT_OPT_1(PREFIX,TYPE,DEFAULT,c)
#define INIT_OPT_4(PREFIX,TYPE,DEFAULT,a,b,c,d) INIT_OPT_2(PREFIX,TYPE,DEFAULT,a,b);  INIT_OPT_2(PREFIX,TYPE,DEFAULT,c,d)

#define GENERIC_OPT_SET_1(PREFIX,TYPE,DEFAULT,a)	    INIT_OPT_1(PREFIX, TYPE, DEFAULT, a); \
  static TYPE ## _option_t * PREFIX ## _ ## TYPE ## _opts[] = { &OPT_NAME(PREFIX, a),\
								NULL }

#define GENERIC_OPT_SET_3(PREFIX, TYPE, DEFAULT, a, b, c)   INIT_OPT_3(PREFIX,TYPE,DEFAULT, a, b, c );\
  static bool_option_t * PREFIX ## _ ## TYPE ## _opts[] = { &OPT_NAME(PREFIX, a),\
							    &OPT_NAME(PREFIX, b),\
							    &OPT_NAME(PREFIX, c),\
							    NULL }

#define GENERIC_OPT_SET_4(PREFIX, TYPE, DEFAULT, a, b, c, d) INIT_OPT_4(PREFIX,TYPE,DEFAULT, a, b, c, d );\
  static bool_option_t * PREFIX ## _ ## TYPE ## _opts[] = { &OPT_NAME(PREFIX, a),\
							    &OPT_NAME(PREFIX, b),\
							    &OPT_NAME(PREFIX, c),\
							    &OPT_NAME(PREFIX, d),\
							    NULL }

#define DECLARE_BOOL_OPTS_3(OPT_1, OPT_2, OPT_3) \
  GENERIC_OPT_SET_3( auth,	bool, BOOL_DEFAULT, OPT_1, OPT_2, OPT_3);			\
  GENERIC_OPT_SET_3( account,	bool, BOOL_DEFAULT, OPT_1, OPT_2, OPT_3);			\
  GENERIC_OPT_SET_3( password,	bool, BOOL_DEFAULT, OPT_1, OPT_2, OPT_3);			\
  GENERIC_OPT_SET_3( session,	bool, BOOL_DEFAULT, OPT_1, OPT_2, OPT_3);

#define DECLARE_BOOL_OPTS_4(OPT_1, OPT_2, OPT_3, OPT_4) \
  GENERIC_OPT_SET_4( auth,	bool, BOOL_DEFAULT, OPT_1, OPT_2, OPT_3, OPT_4);			\
  GENERIC_OPT_SET_4( account,	bool, BOOL_DEFAULT, OPT_1, OPT_2, OPT_3, OPT_4);			\
  GENERIC_OPT_SET_4( password,	bool, BOOL_DEFAULT, OPT_1, OPT_2, OPT_3, OPT_4);			\
  GENERIC_OPT_SET_4( session,	bool, BOOL_DEFAULT, OPT_1, OPT_2, OPT_3, OPT_4);

#define DECLARE_STRING_OPTS_0 DECLARE_STRING_OPTS_1( empty )
#define DECLARE_STRING_OPTS_1(OPT_1)\
  GENERIC_OPT_SET_1( auth,	string, STRING_DEFAULT, OPT_1 ); \
  GENERIC_OPT_SET_1( account,	string, STRING_DEFAULT, OPT_1 ); \
  GENERIC_OPT_SET_1( password,	string, STRING_DEFAULT, OPT_1 ); \
  GENERIC_OPT_SET_1( session,	string, STRING_DEFAULT, OPT_1 ); 
  
#define DECLARE_OPT_SETS \
  static option_set_t auth_opts	    = { auth_bool_opts,	    auth_string_opts,	  &is_enabled, &enable, &get_opt, &set_opt }; \
  static option_set_t account_opts  = { account_bool_opts,  account_string_opts,  &is_enabled, &enable, &get_opt, &set_opt }; \
  static option_set_t password_opts = { password_bool_opts, password_string_opts, &is_enabled, &enable, &get_opt, &set_opt }; \
  static option_set_t session_opts  = { session_bool_opts,  session_string_opts,  &is_enabled, &enable, &get_opt, &set_opt }; \
  static option_set_t *opt_sets[]   = { &auth_opts, &account_opts, &password_opts, &session_opts, NULL }

typedef enum write_type {AUTH=0, ACCOUNT, PASSWORD, SESSION} write_type_t;

struct config_file_t {
  write_type_t type;
  /* pam_pwcheck is password only.  */
  int use_pwcheck;
  int pwcheck_debug;
  int pwcheck_nullok;
  int pwcheck_cracklib;
  char *pwcheck_cracklib_path;
  int pwcheck_maxlen;
  int pwcheck_minlen;
  int pwcheck_have_minlen;
  int pwcheck_tries;
  int pwcheck_remember;
  char *pwcheck_nisdir;
  int pwcheck_no_obscure_checks;
  /* pam_limits is session only */
  int use_limits;
  /* pam_bioapi is auth only currently */
  int use_bioapi;
  char *bioapi_options;
  /* pam_env is auth and session, we use session.  */
  int use_env;
  /* pam_make is password only.  */
  int use_make;
  char *make_options;
  /* pam_unix2 is account, auth, password and session.  */
  int use_unix2;
  int unix2_debug;
  int unix2_nullok;
  int unix2_trace;
  char *unix2_call_modules; /* XXX */
  /* pam_krb5, used by all types.  */
  int use_krb5;
  int krb5_debug;
  uint32_t krb5_minuid;
  int krb5_ignore_unknown_principals;
  /* pam_ccreds, used by auth.  */
  int use_ccreds;
  /* pam_pkcs11, used by auth.  */
  int use_pkcs11;
  /* pam_cracklib, used by password.  */
  int use_cracklib;
  int cracklib_debug;
  int cracklib_retry;
  char *cracklib_dictpath;
  /* pam_apparmor, used by session.  */
  int use_apparmor;
  /* pam_nam, used by all types.  */
  int use_lum;
  /* pam_winbind, used by all types.  */
  int use_winbind;
  int winbind_debug;
  /* pam_capability, used by session.  */
  int use_capability;
  char *capability_conf;
  int capability_debug;
};
typedef struct config_file_t config_file_t;

typedef struct pam_module {
	char *name;
	option_set_t **option_sets;
	int (*parse_config)(struct pam_module *this, char *arguments, write_type_t type);
	int (*print_module)(struct pam_module *this);
	int (*write_config)(struct pam_module *this, enum write_type op, FILE *fp);
	option_set_t* (*get_opt_set) (struct pam_module *this, write_type_t op);
} pam_module_t;

/* modules need access to this to check, which other modules
   are enabled */
extern pam_module_t *supported_module_list[];

/* default handlers */

/* def_parse_config
 *
 * default parse_option function which just returns false.
 */
int def_parse_config (pam_module_t *this, char *arguments, write_type_t type);

/* def_print_module
 *
 * default print_module function which just prints out the module
 * name. it can do this, because we pass it a reference to the
 * pam_module_t as first argument.
 */
int def_print_module (pam_module_t *this);
int def_write_config (pam_module_t *this, enum write_type op, FILE *fp);

option_set_t* get_opt_set( pam_module_t *this, write_type_t op );
/* lookup
 *
 * searches through module_list to find a match for the module named
 * by the second argument.
 */
pam_module_t* lookup( pam_module_t **module_list, const char *module );

const char* type2string (write_type_t wt);

void print_module_config (pam_module_t **module_list, const char *module);

int is_module_enabled (pam_module_t **module_list, const char *module,
		       write_type_t op);

/* handle_module
 *
 * gets called by load_config for each module-name string it
 * encounters. 
 */
int handle_module( const char *file, char *m, char *arguments , pam_module_t **module_list, write_type_t type );

void print_unknown_option_error (const char *module, const char *option);

#endif
