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
write_config_unix2 (pam_module_t *this, enum write_type op, FILE *fp)
{
  option_set_t *opt_set = this->get_opt_set (this, op);
  int with_krb5, with_ldap, with_lum, with_winbind, with_pwcheck,
    with_cracklib, with_mount, with_sss;

  if (debug)
    debug_write_call (this, op);

  if (!opt_set->is_enabled (opt_set, "is_enabled"))
    return 0;

  with_krb5	= is_module_enabled (common_module_list, "pam_krb5.so"	  , op);
  with_ldap	= is_module_enabled (common_module_list, "pam_ldap.so"	  , op);
  with_lum	= is_module_enabled (common_module_list, "pam_lum.so"	  , op);
  with_winbind	= is_module_enabled (common_module_list, "pam_winbind.so" , op);
  with_sss	= is_module_enabled (common_module_list, "pam_sss.so"	  , op);
  with_pwcheck	= is_module_enabled (common_module_list, "pam_pwcheck.so" , op);
  with_cracklib = is_module_enabled (common_module_list, "pam_cracklib.so", op);
  with_mount	= is_module_enabled (common_module_list, "pam_mount.so"	  , op);

  switch (op)
  {
    case AUTH:
      if (with_krb5 || with_ldap || with_lum || with_winbind || with_sss)
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
      if (with_krb5 || with_ldap || with_lum || with_winbind || with_sss)
	fprintf (fp, "account\trequisite\tpam_unix2.so\t");
      else
	fprintf (fp, "account\trequired\tpam_unix2.so\t");
      break;
    case PASSWORD:
		if (with_krb5)
		{
			fprintf (fp, "password\t[default=ignore success=1]\tpam_succeed_if.so\tuid > 999 ");
			if (opt_set->is_enabled (opt_set, "debug"))
				fprintf (fp, "debug \n");
			else
				fprintf (fp, "quiet \n");
		}
		if (with_krb5 || with_ldap || with_lum || with_sss)
			fprintf (fp, "password\tsufficient\tpam_unix2.so\t");
		else
			fprintf (fp, "password\trequired\tpam_unix2.so\t");
		if (with_pwcheck || with_cracklib)
			fprintf (fp, "use_authtok ");
		break;
	  case SESSION:
		  fprintf (fp, "session\trequired\tpam_unix2.so\t");
		  break;
  }

  WRITE_CONFIG_OPTIONS

  return 0;
}

GETOPT_START_ALL
GETOPT_END_ALL

PRINT_ARGS("unix2");
PRINT_XMLHELP("unix2");

/* ---- contruct module object ---- */
DECLARE_BOOL_OPTS_5( is_enabled, nullok, debug, trace, none );
DECLARE_STRING_OPTS_2( call_modules, nisdir);
DECLARE_OPT_SETS;

module_helptext_t unix2_help[] = {{"", NULL, "Use pam_unix2.so as standard UNIX PAM module."},
				  {"nullok", NULL, "Add <option>nullok</option> option to all pam_unix2.so invocations."},
				  {"debug", NULL, "Add <option>debug</option> option to all pam_unix2.so invocations."},
				  {"trace", NULL, "Add <option>trace</option> option to pam_unix2.so."},
				  {"none", NULL, "Add option <option>none</option> to pam_unix2.so."},
				  {"call_modules", "modules,...",  "Add <option>call_modules=</option><replaceable>list of modules</replaceable> to pam_unix2.so."},
				  {"nisdir", "path", "Add <option>nisdir=</option><replaceable>path</replaceable> to pam_unix2.so."},
				  {NULL, NULL, NULL}};


/* at last construct the complete module object */
pam_module_t mod_pam_unix2 = { "pam_unix2.so", opt_sets, unix2_help,
			       &def_parse_config,
			       &def_print_module,
			       &write_config_unix2,
			       &get_opt_set,
			       &getopt,
			       &print_args,
			       &print_xmlhelp};
