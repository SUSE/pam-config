/* Copyright (C) 2007, 2008 Sven Schober
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
parse_config_unix2 (pam_module_t *this, char *args, write_type_t type)
{
  option_set_t *opt_set = this->get_opt_set( this, type );

  if (debug)
    printf("**** parse_config_unix2 (%s): '%s'\n", type2string(type),
           args?args:"");

  opt_set->enable (opt_set, "is_enabled", TRUE);

  while (args && strlen (args) > 0)
    {
      char *cp = strsep (&args, " \t");
      if (args)
	while (isspace ((int)*args))
        ++args;

      if (strcmp (cp, "debug") == 0)
	opt_set->enable( opt_set, "debug", TRUE );
      else if (strcmp (cp, "nullok") == 0)
	opt_set->enable( opt_set, "nullok", TRUE );
      else if (strcmp (cp, "trace") == 0)
	opt_set->enable( opt_set, "trace", TRUE );
      else if (strcmp (cp, "use_first_pass") == 0)
	{ /* will be ignored */ }
      else if (strcmp (cp, "use_authtok") == 0)
	{ /* will be ignored */ }
      else if (strncmp (cp, "call_modules=", 13) == 0){
	/* XXX strip krb5 and ldap modules from it */
	if( ! opt_set->set_opt( opt_set, "call_modules", strdup (&cp[13])) ){
	  DEBUG( "call_modules option recognized but couldn't be added to option set!\n" );
	}
      }
      else
	print_unknown_option_error ("pam_unix2.so", cp);
    }
  return 1;
}

static int
write_config_unix2 (pam_module_t *this, enum write_type op, FILE *fp)
{
  option_set_t *opt_set = this->get_opt_set (this, op);
  int with_krb5, with_ldap, with_lum, with_winbind, with_pwcheck,
    with_cracklib, with_mount;

  if (debug)
    printf ("**** write_config_unix2 (...)\n");

  if (!opt_set->is_enabled (opt_set, "is_enabled"))
    return 0;

  with_krb5	= is_module_enabled (common_module_list, "pam_krb5.so"	  , op);
  with_ldap	= is_module_enabled (common_module_list, "pam_ldap.so"	  , op);
  with_lum	= is_module_enabled (common_module_list, "pam_lum.so"	  , op);
  with_winbind	= is_module_enabled (common_module_list, "pam_winbind.so" , op);
  with_pwcheck	= is_module_enabled (common_module_list, "pam_pwcheck.so" , op);
  with_cracklib = is_module_enabled (common_module_list, "pam_cracklib.so", op);
  with_mount	= is_module_enabled (common_module_list, "pam_mount.so"	  , op);

  switch (op)
  {
    case AUTH:
      if (with_krb5 || with_ldap || with_lum || with_winbind)
	/* Only sufficient if other modules follow */
	fprintf (fp, "auth\tsufficient\tpam_unix2.so\t");
      else
	fprintf (fp, "auth\trequired\tpam_unix2.so\t");
      if (with_mount)
	/* if pam_mount is enabled it asks for a pw so we use that
	 * one.
	 * */
	fprintf (fp, "use_first_pass ");
      break;
    case ACCOUNT:
      if (with_krb5 || with_ldap || with_lum || with_winbind)
	fprintf (fp, "account\trequisite\tpam_unix2.so\t");
      else
	fprintf (fp, "account\trequired\tpam_unix2.so\t");
      break;
    case PASSWORD:
      if (with_krb5 || with_ldap || with_lum)
	fprintf (fp, "password\tsufficient\tpam_unix2.so\t");
      else
	fprintf (fp, "password\trequired\tpam_unix2.so\t");
      if (with_pwcheck || with_cracklib)
	fprintf (fp, "use_authtok ");
      break;
    case SESSION:
      fprintf (fp, "session\trequired\tpam_unix2.so\t");
      if (opt_set->is_enabled (opt_set, "trace"))
	fprintf (fp, "trace ");
      break;
  }

  if (opt_set->is_enabled (opt_set, "nullok"))
    fprintf (fp, "nullok ");
  if (opt_set->is_enabled (opt_set, "debug"))
    fprintf (fp, "debug ");

  char *call_modules = opt_set->get_opt( opt_set, "call_modules");
  if (call_modules)
    fprintf (fp, "call_modules=%s ", call_modules);

  fprintf (fp, "\n");

  return 0;
}

PRINT_ARGS("unix2")

/* ---- contruct module object ---- */
DECLARE_BOOL_OPTS_4( is_enabled, debug, nullok, trace );
DECLARE_STRING_OPTS_1( call_modules );
DECLARE_OPT_SETS;
/* at last construct the complete module object */
pam_module_t mod_pam_unix2 = { "pam_unix2.so", opt_sets,
			       &parse_config_unix2,
			       &def_print_module,
			       &write_config_unix2,
			       &get_opt_set,
			       NULL,
			       &print_args};
