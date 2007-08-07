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
#include <stdlib.h>

#include "pam-config.h"

static int
parse_config_ck_connector (pam_module_t *this, char *args, write_type_t type)
{
  option_set_t *opt_set = this->get_opt_set( this, type );

  if (debug)
    printf("**** parse_config_ck_connector (%s): '%s'\n", type2string(type),
           args?args:"");

  opt_set->enable (opt_set, "is_enabled", TRUE);

  while (args && strlen (args) > 0)
    {
      char *cp = strsep (&args, " \t");
      if (args)
	while (isspace ((int)*args))
        ++args;
      
      if (opt_set->enable (opt_set, cp, TRUE) == FALSE){
	if (strcmp (cp, "debug") == 0)
	  opt_set->enable (opt_set, "debug", TRUE);
	else if (strcmp (cp, "use_first_pass") == 0)
	{ /* will be ignored */ }
	else if (strcmp (cp, "try_first_pass") == 0)
	{ /* will be ignored */ }
	else
	  print_unknown_option_error ("pam_ck_connector.so", cp);
      }
    }
  return 1;
}

static int
session_pred_ck_connector (config_content_t *cfg_content)
{
  int do_insert = FALSE;
  /* insert if current line does not already contain this module */
  do_insert = strcasestr (cfg_content->line, "pam_ck_connector.so") == NULL;
  /* and this line starts with 'password' */
  do_insert &= strcasestr (cfg_content->line, "session") != NULL;
  /* and there is no next line, or the next line is something
   * different than session */
  do_insert &= (  cfg_content->next == NULL || 
		  strcasestr (cfg_content->next->line, "session") == NULL);
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
 * (see session_pred_ck_connector, and password_pred_ck_connector for
 * examples).
 */
static int
write_config_ck_connector (  pam_module_t *this,
		      enum write_type op __attribute__((unused)),
		      FILE *unused __attribute__((unused)))
{
  option_set_t *opt_set = this->get_opt_set (this, SESSION);
  int writeit = opt_set->is_enabled (opt_set, "is_enabled");
  int debug_enabled = opt_set->is_enabled (opt_set, "debug");
  int status = TRUE;
  config_content_t *cfg_content;

  load_single_config (gl_service, &cfg_content);

  if (debug)
    printf ("**** write_config_ck_connector (%s)\n", gl_service);
  if (writeit)
  {
    /* insert pam_ck_connector.so before pam_mount.so in the session
     * stack
     */
    char *line;
    asprintf (&line, "session\t optional\tpam_ck_connector.so%s\n", debug_enabled ? "\t debug" : "" );
    /* this is a bit awkward. we first have to remove the old entry
     * to write out the new one, reflecting changes to options
     */
    remove_module (&cfg_content, "pam_ck_connector.so");
    status &= insert_if (&cfg_content, line , &session_pred_ck_connector, AFTER);
    free (line);
  }
  else{
    /* remove every occurrence of pam_ck_connector.so from the service
     * file 
     */
    remove_module (&cfg_content, "pam_ck_connector.so");
  }
  if (!status)
  {
    fprintf (stderr, _("ERROR: Could not add pam_ck_connector.so to service '%s'"), gl_service);
    return 1;
  }

  return write_single_config (gl_service, &cfg_content);
}



/* ---- contruct module object ---- */
DECLARE_BOOL_OPTS_2( is_enabled, debug );
DECLARE_STRING_OPTS_0;
DECLARE_OPT_SETS;
/* at last construct the complete module object */
pam_module_t mod_pam_ck_connector = { "pam_ck_connector.so", opt_sets,
  &parse_config_ck_connector,
  &def_print_module,
  &write_config_ck_connector,
  &get_opt_set};
