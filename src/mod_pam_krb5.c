/* Copyright (C) 2007 Thorsten Kukuk
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "pam-config.h"
#include "pam-module.h"

static int
parse_config_krb5 (pam_module_t * this, char *args, write_type_t type)
{
  option_set_t *opt_set = this->get_opt_set (this, type);

  if (debug)
    printf ("**** parse_config_krb5 (%s): '%s'\n", type2string (type),
	    args ? args : "");

  opt_set->enable (opt_set, "is_enabled", TRUE);

  while (args && strlen (args) > 0)
    {
      char *cp = strsep (&args, " \t");
      if (args)
	while (isspace ((int) *args))
	  ++args;

      if (strcmp (cp, "debug") == 0)
	opt_set->enable (opt_set, "debug", TRUE);
      else if (strcmp (cp, "ignore_unknown_principals") == 0)
	opt_set->enable (opt_set, "ignore_unknown_principals", TRUE);
      else if (strncmp (cp, "minimum_uid=", 12) == 0)
	opt_set->set_opt (opt_set, "minimum_uid", strdup (&cp[12]));
      else if (strcmp (cp, "use_first_pass") == 0 ||
	       strcmp (cp, "try_first_pass") == 0 ||
	       strcmp (cp, "use_authtok") == 0)
	/* Do nothing, this are handled by pam-config if necessary. */ ;
      else
	print_unknown_option_error ("pam_krb5.so", cp);
    }
  return 1;
}

static int
write_config_krb5 (pam_module_t * this, enum write_type op, FILE * fp)
{
  option_set_t *opt_set = this->get_opt_set (this, op);
  int with_ldap, with_nam, with_winbind, with_ccreds;

  if (debug)
    printf ("**** write_config_krb5 (...)\n");

  if (!opt_set->is_enabled (opt_set, "is_enabled"))
    return 0;

  with_ldap = is_module_enabled (common_module_list,
				 "pam_ldap.so", op);
  with_nam = is_module_enabled (common_module_list,
				"pam_nam.so", op);
  with_winbind = is_module_enabled (common_module_list,
				    "pam_winbind.so", op);
  with_ccreds = is_module_enabled (common_module_list,
				   "pam_ccreds.so", op);

  switch (op)
    {
    case ACCOUNT:
      if (with_ldap || with_nam || with_winbind)
        fprintf (fp, "account\t[success=ok new_authtok_reqd=ok ignore=ignore default=bad user_unknown=ignore]\tpam_krb5.so\tuse_first_pass "); /* required with additional user_unknown mapping to ignore */
      else
        fprintf (fp, "account\trequired\tpam_krb5.so\tuse_first_pass ");
      if (opt_set->is_enabled (opt_set, "ignore_unknown_principals"))
        fprintf (fp, "ignore_unknown_principals ");
      break;
    case AUTH:
      if (with_ccreds)
        fprintf (fp, "auth\t[authinfo_unavail=ignore success=1 default=2]\tpam_krb5.so\tuse_first_pass ");
      else if (with_ldap || with_nam || with_winbind)
        fprintf (fp, "auth\tsufficient\tpam_krb5.so\tuse_first_pass ");
      else
        fprintf (fp, "auth\trequired\tpam_krb5.so\tuse_first_pass ");
      break;
    case PASSWORD:
      if (with_ldap || with_nam)
        fprintf (fp, "password\tsufficient\tpam_krb5.so\tuse_authtok ");
      else
        fprintf (fp, "password\trequired\tpam_krb5.so\tuse_authtok ");
      break;
    case SESSION:
      fprintf (fp, "session\toptional\tpam_krb5.so\t");
      break;
    }

  if (opt_set->is_enabled (opt_set, "debug"))
    fprintf (fp, "debug ");
  const char *cp = opt_set->get_opt (opt_set, "minimum_uid");
  if (cp)
    fprintf (fp, "minimum_uid=%s ", cp);
  fprintf (fp, "\n");

  if (op == AUTH && with_ccreds)
    {
      fprintf (fp,
	       "auth\t[default=done]\tpam_ccreds.so\taction=validate use_first_pass\n");
      fprintf (fp, "auth\t[default=done]\tpam_ccreds.so\taction=store\n");
      fprintf (fp, "auth\t[default=bad]\tpam_ccreds.so\taction=update\n");
    }

  if (op == AUTH && !(with_ldap || with_nam || with_winbind))
	  fprintf (fp, "auth\trequired\tpam_deny.so\n");

  return 0;
}



/* ---- contruct module object ---- */
DECLARE_BOOL_OPTS_3 (is_enabled, debug, ignore_unknown_principals);
DECLARE_STRING_OPTS_1 (minimum_uid);
DECLARE_OPT_SETS;
/* at last construct the complete module object */
pam_module_t mod_pam_krb5 = { "pam_krb5.so", opt_sets,
  &parse_config_krb5,
  &def_print_module,
  &write_config_krb5,
  &get_opt_set
};
