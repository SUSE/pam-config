#ifndef _PAM_MODULES_H_
#define _PAM_MODULES_H_ 1

#include <stdint.h>

struct config_file_t {
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
  /* pam_mkhomedir is session only.  */
  int use_mkhomedir;
  /* pam_limits is session only */
  int use_limits;
  /* pam_bioapi is auth only currently */
  int use_bioapi;
  char *bioapi_options;
  /* pam_env is auth and session, we use session.  */
  int use_env;
  /* pam_xauth is auth and session, session preferred.  */
  int use_xauth;
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
  /* pam_ldap, used by all types.  */
  int use_ldap;
  int ldap_debug;
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
  /* pam_umask, used by session.  */
  int use_umask;
  int umask_debug;
  /* pam_capability, used by session.  */
  int use_capability;
  char *capability_conf;
  int capability_debug;
};
typedef struct config_file_t config_file_t;

enum write_type {AUTH, ACCOUNT, PASSWORD, SESSION};


typedef struct pam_module {
	char *name;
	int (*parse_option)( struct pam_module *this, char *arguments, config_file_t *conf );
	int (*print_module)( struct pam_module *this, config_file_t *conf );
	int (*write_config)( struct pam_module *this, enum write_type op, config_file_t *conf);
} pam_module_t;


/* default handlers */

/* err_parse_option 
 *
 * default parse_option function which just returns false.
 */
int err_parse_option( pam_module_t *this, char *arguments, config_file_t *conf  );

/* def_print_module
 *
 * default print_module function which just prints out the module
 * name. it can do this, because we pass it a reference to the
 * pam_module_t as first argument.
 */
int def_print_module( pam_module_t *this, config_file_t *conf );
int def_write_config( pam_module_t *this, enum write_type op, config_file_t *conf );

/* lookup
 *
 * searches through module_list to find a match for the module named
 * by the second argument.
 */
pam_module_t* lookup( pam_module_t **module_list, char *module );

/* handle_module
 *
 * gets called by load_config for each module-name string it
 * encounters. 
 */
int handle_module( const char *file, char *m, char *arguments , pam_module_t **module_list, config_file_t *conf );

void print_unknown_option_error (const char *module, const char *option);

#endif
