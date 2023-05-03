/* Copyright (C) 2007, 2008 Thorsten Kukuk
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
write_config_krb5 (pam_module_t *this, enum write_type op, FILE *fp)
{
  option_set_t *opt_set = this->get_opt_set (this, op);
  int with_ldap, with_nam, with_sss, with_winbind, with_ccreds, with_aad;

  if (debug)
    debug_write_call (this, op);

  if (!opt_set->is_enabled (opt_set, "is_enabled"))
    return 0;

  with_ldap = is_module_enabled (common_module_list,
				 "pam_ldap.so", op);
  with_nam = is_module_enabled (common_module_list,
				"pam_nam.so", op);
  with_sss = is_module_enabled (common_module_list,
				    "pam_sss.so", op);
  with_winbind = is_module_enabled (common_module_list,
				    "pam_winbind.so", op);
  with_ccreds = is_module_enabled (common_module_list,
				   "pam_ccreds.so", op);
  with_aad = is_module_enabled (common_module_list,
				"pam_aad.so", op);
  
  switch (op)
    {
    case ACCOUNT:
      /* No special handling in case of with_ldap...
       * new_authtok_reqd=ok is wrong, if a new authtok is required go into password change
       * user_unknown=ignore will be done with the ignore_unknown_principals parameter */
      fprintf (fp, "account\trequired\tpam_krb5.so\tuse_first_pass ");
      if (opt_set->is_enabled (opt_set, "ignore_unknown_principals"))
        fprintf (fp, "ignore_unknown_principals ");
      break;
    case AUTH:
      if (with_ccreds)
        fprintf (fp, "auth\t[authinfo_unavail=ignore success=1 default=2]\tpam_krb5.so\tuse_first_pass ");
      else
        fprintf (fp, "auth\tsufficient\tpam_krb5.so\tuse_first_pass ");
      break;
    case PASSWORD:
      /* no use_authtok parameter. pam_krb5 tries always if the authtok is available
       * use_authtok would result in an error if this is not the case and we cannot
       * be sure that an other module has asked for the new password before we call
       * pam_krb5. */
        fprintf (fp, "password\tsufficient\tpam_krb5.so ");
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

  if (op == AUTH && !(with_ldap || with_nam || with_sss || with_winbind || with_aad))
	  fprintf (fp, "auth\trequired\tpam_deny.so\n");

  /* ldap and nam are behind this module. We write a deny
	 only if we are the last module. */
  if (op == PASSWORD && !(with_ldap || with_nam || with_sss ))
	  fprintf (fp, "password\trequired\tpam_deny.so\n");

  return 0;
}

GETOPT_START_ALL
  else if (strcmp ("minimum_uid", opt) == 0)
    {
      if (g_opt->m_delete)
	{
	  opt_set = this->get_opt_set (this, ACCOUNT);
	  opt_set->set_opt (opt_set, "minimum_uid", NULL);
	  opt_set = this->get_opt_set (this, AUTH);
	  opt_set->set_opt (opt_set, "minimum_uid", NULL);
	  opt_set = this->get_opt_set (this, PASSWORD);
	  opt_set->set_opt (opt_set, "minimum_uid", NULL);
	  opt_set = this->get_opt_set (this, SESSION);
	  opt_set->set_opt (opt_set, "minimum_uid", NULL);
	}
      else
	{
	  opt_set = this->get_opt_set (this, ACCOUNT);
	  opt_set->set_opt (opt_set, "minimum_uid", strdup (optarg));
	  opt_set = this->get_opt_set (this, AUTH);
	  opt_set->set_opt (opt_set, "minimum_uid", strdup (optarg));
	  opt_set = this->get_opt_set (this, PASSWORD);
	  opt_set->set_opt (opt_set, "minimum_uid", strdup (optarg));
	  opt_set = this->get_opt_set (this, SESSION);
	  opt_set->set_opt (opt_set, "minimum_uid", strdup (optarg));
	}
    }
GETOPT_END_ALL

PRINT_ARGS("krb5")
PRINT_XMLHELP("krb5")

/* ---- construct module object ---- */
DECLARE_BOOL_OPTS_3 (is_enabled, debug, ignore_unknown_principals);
DECLARE_STRING_OPTS_1 (minimum_uid);
DECLARE_OPT_SETS;

static module_helptext_t helptext[] = {{NULL, NULL, NULL}};

/* at last construct the complete module object */
pam_module_t mod_pam_krb5 = { "pam_krb5.so", opt_sets, helptext,
			      &def_parse_config,
			      &def_print_module,
			      &write_config_krb5,
			      &get_opt_set,
			      &getopt,
			      &print_args,
			      &print_xmlhelp};
