/* Copyright (C) 2006 Thorsten Kukuk
   Author: Thorsten Kukuk <kukuk@thkukuk.de>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License version 2 as
   published by the Free Software Foundation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#ifndef _PAM_CONFIG_H_
#define _PAM_CONFIG_H_ 1

struct config_file_t {
  /* pam_pwcheck is password only.  */
  int use_pwcheck;
  int pwcheck_debug;
  int pwcheck_nullok;
  int pwcheck_cracklib;
  char *pwcheck_cracklib_path;
  int pwcheck_maxlen;
  int pwcheck_minlen;
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
};
typedef struct config_file_t config_file_t;

extern int debug;

int load_obsolete_conf (config_file_t *account, config_file_t *auth,
			config_file_t *password, config_file_t *session);
int load_config (const char *file, const char *service,
		 config_file_t *conf);
int write_config_account (const char *file, config_file_t *conf);
int write_config_auth (const char *file, config_file_t *conf);
int write_config_password (const char *file, config_file_t *conf);
int write_config_session (const char *file, config_file_t *conf);

int sanitize_check_auth (config_file_t *conf);
int sanitize_check_password (config_file_t *conf);
int check_for_pam_module (const char *name, int force);

void print_module_pwcheck (config_file_t *conf);
void print_module_cracklib (config_file_t *conf);
void print_module_unix2 (config_file_t *account, config_file_t *auth,
			 config_file_t *password, config_file_t *session);
void print_module_krb5 (config_file_t *account, config_file_t *auth,
                        config_file_t *password, config_file_t *session);
void print_module_ldap (config_file_t *account, config_file_t *auth,
                        config_file_t *password, config_file_t *session);



#endif
