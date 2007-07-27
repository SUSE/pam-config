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
#include "pam-module.h"

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

/* this predicate is needed by the insert_if function.
 * it evaluates to true if the argument line contains pam_mount.so.
 * this leads to pam_crypt.so to be written _before_ pam_mount.so
 */
static int
pred_cryptpass (config_content_t *cfg_content)
{
  return strcasestr (cfg_content->line, "pam_mount.so") != NULL
	    && strcasestr (cfg_content->line, "auth") == NULL;
}

static int
write_config_cryptpass (  pam_module_t *this,
		      enum write_type op __attribute__((unused)),
		      FILE *unused __attribute__((unused)))
{
  option_set_t *opt_set = this->get_opt_set (this, SESSION);
  int writeit = opt_set->is_enabled (opt_set, "is_enabled");
  int status = 0;
  config_content_t *cfg_content;

  load_single_config (gl_service, &cfg_content);

  if (debug)
    printf ("**** write_config_cryptpass (%s)\n", gl_service);
  /* TODO:
   *  - see, that mod_pam_mount also uses this mechanism
   *  - make the insertion point configurable (ATM: just 'before')
   */
  if (writeit)
    status = insert_if ( &cfg_content, "session\toptional\tpam_cryptpass.so\n", &pred_cryptpass );

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
