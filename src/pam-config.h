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


struct config_content_t {
  char *line;
  struct config_content_t *next;
};
typedef struct config_content_t config_content_t;


extern int debug;
extern char *gl_service;


int load_obsolete_conf (pam_module_t **module_list);

int load_config (const char *file, write_type_t wtype,
                 pam_module_t **module_list, int warn_unknown_mod);
int write_config (write_type_t op, const char *file,
		  pam_module_t **module_list);

int load_single_config (const char *config_name, config_content_t **ptr);
FILE *create_service_file (const char *service);
int close_service_file (FILE *fp, const char *service);

int sanitize_check_account (pam_module_t **module_list);
int sanitize_check_auth (pam_module_t **module_list);
int sanitize_check_password (pam_module_t **module_list);
int sanitize_check_session (pam_module_t **module_list);
int check_for_pam_module (const char *name, int force);
int check_for_unix_conflict (pam_module_t **module_list, write_type_t op);

#endif
