/* Copyright (C) 2006, 2007 Thorsten Kukuk
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

#include "pam-module.h"


extern int debug;

int load_obsolete_conf (config_file_t *account, config_file_t *auth,
			config_file_t *password, config_file_t *session);

int load_config (const char *file, const char *service,
		 write_type_t wtype, pam_module_t **module_list );
int write_config_account (const char *file, config_file_t *conf);
int write_config_auth (const char *file, config_file_t *conf);
int write_config_password (const char *file, config_file_t *conf);
int write_config_session (const char *file, config_file_t *conf);

int sanitize_check_account (config_file_t *conf);
int sanitize_check_auth (config_file_t *conf);
int sanitize_check_password (config_file_t *conf);
int sanitize_check_session (config_file_t *conf);
int check_for_pam_module (const char *name, int force);

void print_module_pwcheck (config_file_t *conf);
void print_module_cracklib (config_file_t *conf);
void print_module_krb5 (config_file_t *account, config_file_t *auth,
                        config_file_t *password, config_file_t *session);
void print_module_krb5afs (config_file_t *account, config_file_t *auth,
			   config_file_t *password, config_file_t *session);
void print_module_ldap (config_file_t *account, config_file_t *auth,
                        config_file_t *password, config_file_t *session);
void print_module_winbind (config_file_t *account, config_file_t *auth,
			   config_file_t *password, config_file_t *session);
void print_module_umask (config_file_t *conf);
void print_module_capability (config_file_t *conf);

#endif
