/**
 * @file pam-config.h
 * @brief Definitions of functions used by pam-config main().
 *
 * @author Thorsten Kukuk
 * @date 2007-07-23
 */
/* Copyright (C) 2006, 2007, 2009, 2019 Thorsten Kukuk
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

/**
 * @struct config_content_t
 * @brief Represents a service file as a singly linked list.
 */
struct config_content_t {
  char *line;
  struct config_content_t *next;
};
typedef struct config_content_t config_content_t;


extern int debug;
extern char *gl_service;
extern char *confdir;

#define CONF_FALLBACK_DIR1 "/usr/lib"
#define CONF_FALLBACK_DIR2 "/usr/etc"

#define CONF_ACCOUNT "common-account"
#define CONF_ACCOUNT_PC "common-account-pc"
#define CONF_AUTH "common-auth"
#define CONF_AUTH_PC "common-auth-pc"
#define CONF_PASSWORD "common-password"
#define CONF_PASSWORD_PC "common-password-pc"
#define CONF_SESSION "common-session"
#define CONF_SESSION_PC "common-session-pc"
#define CONF_SESSION_NONLOGIN "common-session-nonlogin"
#define CONF_SESSION_NONLOGIN_PC "common-session-nonlogin-pc"

int load_obsolete_conf (pam_module_t **module_list);

int load_config (const char *confdir, const char *file, write_type_t wtype,
		 pam_module_t **module_list, int warn_unknown_mod);
int write_config (const char *confdir, const char *file, write_type_t op,
		  pam_module_t **module_list);

int load_single_config (const char *config_name, config_content_t **ptr);
int write_single_config (const char *service, config_content_t **cfg_content);

/**
 * @typedef predicate_fptr
 * @brief Hide function pointer syntax.
 */
typedef int (*predicate_fptr)(config_content_t *next);

/**
 * @brief Specifies the insert position for insert_if()
 */
enum insert_pos_t {
  BEFORE = 1,
  AFTER
};
typedef enum insert_pos_t insert_pos_t;

/**
 * @brief Iterates over every line of the given service file and inserts the given line
 * if the predicate is true. The insertion postion can be specified
 * with the last argument.
 *
 * @see write_config_cryptpass() for example usage.
 * @see src/single_config.c for definition.
 *
 * @param cfg A pointer to the linked list representing the service
 * file to work on.
 * @param line A pointer to the line that is to be inserted.
 * @param predicate A function that decides when to insert the given
 * line.
 * @param position either BEFORE, or AFTER.
 *
 * @return TRUE if insertion was successfull or the entry was
 * already present, FALSE otherwise.
 */

int insert_if (config_content_t **cfg, const char *line, predicate_fptr predicate, insert_pos_t position);

/**
 * @brief Remove every occurrence of lines containing 'module_name' from service file
 *
 * @param *cfg A pointer to the linked list representing the service
 * file to work on.
 * @param module_name Name of the module which shall be purged from
 * the file.
 *
 * @return The number of lines that were removed.
 */
int remove_module (config_content_t **cfg, const char *module_name);

/**
 * @brief Creates a temporary service file and returns a handle to
 * it.
 *
 * This function creates a temp. service file at
 * \code CONFDIR"/pam.d/pam-config.tmpXXXXXX" \endcode and returns a \a FILE handle
 * to it.
 *
 * @param service Name of the service file to create
 *
 * @return FILE handle to the newly created file
 */
FILE *create_service_file (const char *service);


/**
 * @brief Closes the file creates with create_service_file().
 *
 * Use this function to close a service file created with
 * create_service_file(). If a service file for \a service already
 * existed it gets back-upped as \c <service-name>.old.
 *
 * @param fp FILE handle from create_service_file()
 * @param service the name of the service
 *
 * @return FALSE in case of failure, TRUE otherwise.
 */
int close_service_file (FILE *fp, const char *service);

int sanitize_check_account (pam_module_t **module_list, int verify);
int sanitize_check_auth (pam_module_t **module_list, int verify);
int sanitize_check_password (pam_module_t **module_list, int verify);
int sanitize_check_session (pam_module_t **module_list, int verify);
int check_for_pam_module (const char *name, int force, int check_only);

#endif
