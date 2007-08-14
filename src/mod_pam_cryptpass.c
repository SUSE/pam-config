/* Copyright (C) 2007 Sven Schober
   Author: Sven Schober <sschober@suse.de>

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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "pam-config.h"

static int
parse_config_cryptpass (pam_module_t *this, char *args, write_type_t type)
{
  option_set_t *opt_set = this->get_opt_set( this, type );

  if (debug)
    printf("**** parse_config_cryptpass (%s): '%s'\n", type2string(type),
           args?args:"");

  opt_set->enable (opt_set, "is_enabled", TRUE);

  while (args && strlen (args) > 0)
    {
      char *cp = strsep (&args, " \t");
      if (args)
	while (isspace ((int)*args))
        ++args;
      
      if (opt_set->enable (opt_set, cp, TRUE) == FALSE){
	if (strcmp (cp, "use_first_pass") == 0)
	{ /* will be ignored */ }
	else if (strcmp (cp, "try_first_pass") == 0)
	{ /* will be ignored */ }
	else
	  print_unknown_option_error ("pam_cryptpass.so", cp);
      }
    }
  return 1;
}

/* These predicates define the place where to insert an entry in a service
 * file
 */
static int
session_pred_cryptpass (config_content_t *cfg_content)
{
  /* insert if current line contains pam_mount.so ... */
  return strcasestr (cfg_content->line, "pam_mount.so") != NULL
    /* and current line contains session */
	    && strcasestr (cfg_content->line, "session") != NULL;
}

static int
password_pred_cryptpass (config_content_t *cfg_content)
{
  int do_insert = FALSE;
  /* insert if current line does not already contain this module */
  do_insert = strcasestr (cfg_content->line, "pam_cryptpass.so") == NULL;
  /* and this line starts with 'password' */
  do_insert &= strcasestr (cfg_content->line, "password") != NULL;
  /* and there is a next line */
  do_insert &= cfg_content->next != NULL;
  /* and that next line does not start with 'password' */
  do_insert &= strcasestr (cfg_content->next->line, "password") == NULL;
  return do_insert;
}

/*
 * This implementation follows a different approach than the other
 * single service modules (lastlog, loginuid and mount):
 *
 * Rather than operating on the file directly, the config/service
 * file is parsed in first into *cfg_content.
 *
 * Then depending on the cmd line switch (-a/d), it operates on the
 * linked list instead, using insert_if() and remove_module().
 *
 * I chose this solution because this way there is more flexibility
 * where to insert the module.
 *
 * You can specify the insertion point by writing a custom predicate
 * (see session_pred_cryptpass, and password_pred_cryptpass for
 * examples).
 */
static int
write_config_cryptpass (  pam_module_t *this,
		      enum write_type op __attribute__((unused)),
		      FILE *unused __attribute__((unused)))
{
  option_set_t *opt_set = this->get_opt_set (this, SESSION);
  int write_session = opt_set->is_enabled (opt_set, "is_enabled");
  opt_set = this->get_opt_set (this, PASSWORD);
  int write_password = opt_set->is_enabled (opt_set, "is_enabled");
  int status = TRUE;
  config_content_t *cfg_content;

  load_single_config (gl_service, &cfg_content);

  if (debug)
    printf ("**** write_config_cryptpass (%s) (%s:%s%s) \n", gl_service, 
	(write_session || write_session) ? "enable" : "disable",
	write_session ? "session" : "", write_password ? ", password" : "");
  /* remove every occurrence of pam_cryptpass.so from the service
   * file 
   */
  remove_module (&cfg_content, "pam_cryptpass.so");

  if (write_session)
  {
    if (!is_module_enabled (service_module_list, "pam_mount.so", AUTH))
    {
      fprintf (stderr, _("ERROR: pam_mount.so is not enabled for service '%s', but needed by pam_cryptpass.so\n"), gl_service);
      return 1;
    }
    /* insert pam_cryptpass.so before pam_mount.so in the session
     * stack
     */
    status &= insert_if (&cfg_content, "session\t optional\tpam_cryptpass.so\n", &session_pred_cryptpass, BEFORE);
  }
  if (write_password)
  {
    /* inset pam_cryptpass.so as the last module of the password
     * stack
     */
    status &= insert_if (&cfg_content, "password optional\tpam_cryptpass.so\tuse_first_pass\n", &password_pred_cryptpass, AFTER);
  }

  if (!status)
  {
    fprintf (stderr, _("ERROR: Could not add pam_cryptpass.so to service '%s'"), gl_service);
    return 1;
  }

  return write_single_config (gl_service, &cfg_content);
}



/* ---- contruct module object ---- */
DECLARE_BOOL_OPTS_1( is_enabled );
DECLARE_STRING_OPTS_0;
DECLARE_OPT_SETS;
/* at last construct the complete module object */
pam_module_t mod_pam_cryptpass = { "pam_cryptpass.so", opt_sets,
  &parse_config_cryptpass,
  &def_print_module,
  &write_config_cryptpass,
  &get_opt_set};
